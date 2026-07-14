/*
 * File:   ProgressToClinicalEvent.cpp
 * Author: Merlin
 *
 * Created on July 30, 2013, 2:36 PM
 */

#include "ProgressToClinicalEvent.h"

#include <algorithm>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Events/ReportTreatmentFailureDeathEvent.h"
#include "MDC/ModelDataCollector.h"
#include "Population/ClinicalUpdateFunction.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/Person/Person.h"
#include "Population/Population.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Simulation/Model.h"
#include "Treatment/ITreatmentCoverageModel.h"
#include "Treatment/Strategies/IStrategy.h"
#include "Treatment/Strategies/NestedMFTMultiLocationStrategy.h"
#include "Treatment/Strategies/NestedMFTStrategy.h"
#include "Utils/Random.h"

// OBJECTPOOL_IMPL(ProgressToClinicalEvent)

bool ProgressToClinicalEvent::should_receive_treatment(Person* person) {
  const double base_p = Model::get_treatment_coverage()->get_probability_to_be_treated(
      person->get_location(), person->get_age());
  const auto &ep = Model::get_config()->get_epidemiological_parameters();
  const double modifier =
      ep.get_age_based_probability_of_seeking_treatment().evaluate_for_age(person->get_age());
  const double effective_p = std::clamp(base_p * modifier, 0.0, 1.0);
  return Model::get_random()->random_flat(0.0, 1.0) <= effective_p;
}

void ProgressToClinicalEvent::handle_no_treatment(Person* person) {
  // did not receive treatment
  Model::get_mdc()->record_1_tf(person->get_location(), false);
  Model::get_mdc()->record_1_non_treated_case(person->get_location(), person->get_age(),
                                              person->get_age_class());

  if (person->will_progress_to_death_when_receive_no_treatment()) {
    person->cancel_all_events_except(nullptr);
    person->set_host_state(Person::DEAD);
    Model::get_mdc()->record_1_malaria_death(person->get_location(), person->get_age(), false);
    return;
  }
}

std::pair<Therapy*, bool> ProgressToClinicalEvent::determine_therapy(Person* person,
                                                                     const bool is_recurrence) {
  if (person == nullptr) { throw std::invalid_argument("Person must not be null"); }

  auto* treatment_strategy = Model::get_treatment_strategy();
  if (treatment_strategy == nullptr) {
    throw std::runtime_error("Treatment strategy is not configured");
  }

  // Second-line therapy is used for recurrent cases treated through
  // the public sector.
  auto* second_line_strategy = Model::get_second_line_strategy();

  // Select a therapy from a nested strategy using the supplied treatment
  // distribution. The distribution must correspond one-to-one with the
  // nested strategy list.
  const auto select_nested_therapy =
      [&](const auto* nested_strategy,
          const std::vector<double> &distribution) -> std::pair<Therapy*, bool> {
    if (nested_strategy->strategy_list.empty()) {
      throw std::runtime_error("Nested treatment strategy has no child strategies");
    }

    if (distribution.size() != nested_strategy->strategy_list.size()) {
      throw std::runtime_error("Treatment distribution size does not match strategy list size");
    }

    // Select a nested strategy using cumulative weighted sampling.
    // The last strategy is the fallback for small floating-point gaps
    // between the cumulative probability and 1.0.
    const auto probability = Model::get_random()->random_flat(0.0, 1.0);

    double cumulative_probability = 0.0;
    std::size_t strategy_id = distribution.size() - 1;

    for (std::size_t i = 0; i < distribution.size(); ++i) {
      cumulative_probability += distribution[i];
      if (probability < cumulative_probability) {
        strategy_id = i;
        break;
      }
    }

    // By convention, strategy index 0 represents treatment delivered
    // through the public sector. Other indices represent non-public
    // treatment channels, such as the private sector.
    const bool is_public_sector_treatment = strategy_id == 0;

    // A recurrent case initially assigned to the public sector receives
    // second-line therapy when a second-line strategy is configured.
    if (is_public_sector_treatment && is_recurrence && second_line_strategy != nullptr) {
      return {second_line_strategy->get_therapy(person), true};
    }

    auto* selected_strategy = nested_strategy->strategy_list[strategy_id];
    if (selected_strategy == nullptr) {
      throw std::runtime_error("Selected nested treatment strategy is null");
    }

    // The bool indicates whether the selected treatment was delivered
    // through the public sector.
    return {
        selected_strategy->get_therapy(person),
        is_public_sector_treatment,
    };
  };

  // A standard nested MFT uses the same public/private treatment
  // distribution for every location.
  if (const auto* strategy = dynamic_cast<NestedMFTStrategy*>(treatment_strategy);
      strategy != nullptr) {
    return select_nested_therapy(strategy, strategy->distribution);
  }

  // A multi-location nested MFT uses a location-specific public/private
  // treatment distribution.
  if (const auto* strategy = dynamic_cast<NestedMFTMultiLocationStrategy*>(treatment_strategy);
      strategy != nullptr) {
    const auto location = person->get_location();

    if (location < 0 || static_cast<std::size_t>(location) >= strategy->distribution.size()) {
      throw std::out_of_range("Person location is outside treatment distribution range");
    }

    return select_nested_therapy(strategy,
                                 strategy->distribution[static_cast<std::size_t>(location)]);
  }

  // A non-nested treatment strategy is treated as public-sector treatment.
  // Recurrent cases receive second-line therapy when available.
  if (is_recurrence && second_line_strategy != nullptr) {
    return {second_line_strategy->get_therapy(person), true};
  }

  return {treatment_strategy->get_therapy(person), true};
}

void ProgressToClinicalEvent::apply_therapy(Person* person, Therapy* therapy,
                                            bool is_public_sector) {
  person->receive_therapy(therapy, clinical_caused_parasite_, false, is_public_sector);

  clinical_caused_parasite_->set_update_function(Model::having_drug_update_function());

  person->schedule_update_by_drug_event(clinical_caused_parasite_);
  // check if the person will progress to death despite of the treatment, this should be
  // 90% lower than the no treatment case
  if (person->will_progress_to_death_when_recieve_treatment()) {
    person->cancel_all_events_except(nullptr);
    person->set_host_state(Person::DEAD);
    Model::get_mdc()->record_1_malaria_death(person->get_location(), person->get_age(), true);

    person->schedule_report_treatment_failure_death_event(
        therapy->get_id(), Model::get_config()->get_therapy_parameters().get_tf_testing_day());
    return;
  }
}

void ProgressToClinicalEvent::do_execute() {
  // spdlog::info("ProgressToClinicalEvent::do_execute");
  auto* person = get_person();

  if (person == nullptr) { throw std::runtime_error("Person is nullptr"); }
  if (person->get_all_clonal_parasite_populations()->size() == 0) {
    // parasites might be cleaned by immune system or other things else
    return;
  }

  // if the clinical_caused_parasite eventually removed then do nothing
  if (!person->get_all_clonal_parasite_populations()->contain(clinical_caused_parasite_)) {
    // spdlog::info("ProgressToClinicalEvent::do_execute: parasite removed");
    return;
  }

  if (person->get_host_state() == Person::CLINICAL) {
    // spdlog::info("ProgressToClinicalEvent::do_execute: Person is already Clinical");
    clinical_caused_parasite_->set_update_function(Model::immunity_clearance_update_function());
    return;
  }

  transition_to_clinical_state(person);
}

void ProgressToClinicalEvent::transition_to_clinical_state(Person* person) {
  const auto density =
      Model::get_random()->random_uniform<double>(Model::get_config()
                                                      ->get_parasite_parameters()
                                                      .get_parasite_density_levels()
                                                      .get_log_parasite_density_clinical_from(),
                                                  Model::get_config()
                                                      ->get_parasite_parameters()
                                                      .get_parasite_density_levels()
                                                      .get_log_parasite_density_clinical_to());

  clinical_caused_parasite_->set_last_update_log10_parasite_density(density);

  // Person change state to Clinical
  person->set_host_state(Person::CLINICAL);

  // TODO: what is the best option to apply here?
  // on one hand we don't what an individual have multiple clinical episodes
  // consecutively, on the other hand we don't want all the other clinical
  // episode to be cancled (i.e recrudescence epidsodes)
  int count = 0;
  // std::string event_time;
  // for (const auto& pair : person->get_events()) {
  //   if ( typeid(*(pair.second)).name() == typeid(ProgressToClinicalEvent).name()
  //     && pair.second->is_executable()) {
  //     event_time += std::to_string(pair.first) + " ";
  //     count++;
  //   }
  // }
  // if (count > 1) {
  //   spdlog::warn("Person {} has {} ProgressToClinicalEvent, time {}, cancel all but this one",
  //                person->get_age(), count, event_time);
  // }
  person->cancel_all_other_progress_to_clinical_events_except(this);
  count = 0;
  std::string event_time;
  for (const auto &pair : person->get_events()) {
    if (dynamic_cast<ProgressToClinicalEvent*>(pair.second.get()) != nullptr
        && pair.second->is_executable()) {
      event_time += std::to_string(pair.first) + " ";
      count++;
    }
  }
  if (count > 1) {
    spdlog::warn("Person {} has {} ProgressToClinicalEvent, time {} after canceling",
                 person->get_age(), count, event_time);
  }

  person->change_all_parasite_update_function(Model::progress_to_clinical_update_function(),
                                              Model::immunity_clearance_update_function());

  clinical_caused_parasite_->set_update_function(Model::clinical_update_function());

  // Statistic collect cumulative clinical episodes
  Model::get_mdc()->collect_1_clinical_episode(person->get_location(), person->get_age(),
                                               person->get_age_class());

  if (should_receive_treatment(person)) {
    // if ((Model::get_scheduler()->current_time()
    //      - person->get_latest_time_received_public_treatment())
    //     < 30) {
    //   const auto [therapy, is_public_sector] = determine_therapy(person, true);
    //   // record 1 treatement for recrudescence
    //   Model::get_mdc()->record_1_recrudescence_treatment(person->get_location(),
    //   person->get_age(),
    //                                                      person->get_age_class(), 0);
    //
    //   apply_therapy(person, therapy, is_public_sector);
    //     } else {
    //       // this is normal routine for clinical cases
    //       const auto [therapy, is_public_sector] = determine_therapy(person, false);
    //
    //       Model::get_mdc()->record_1_treatment(person->get_location(), person->get_age(),
    //                                            person->get_age_class(), therapy->get_id());
    //
    //       person->schedule_test_treatment_failure_event(
    //           clinical_caused_parasite_,
    //           Model::get_config()->get_therapy_parameters().get_tf_testing_day(),
    //           therapy->get_id());
    //       apply_therapy(person, therapy, is_public_sector);
    // }
    // this is normal routine for clinical cases
    const auto [therapy, is_public_sector] = determine_therapy(person, is_recurrence_);

    Model::get_mdc()->record_1_treatment(person->get_location(), person->get_age(),
                                         person->get_age_class(), therapy->get_id());

    person->schedule_test_treatment_failure_event(
        clinical_caused_parasite_,
        Model::get_config()->get_therapy_parameters().get_tf_testing_day(), therapy->get_id());
    apply_therapy(person, therapy, is_public_sector);
  } else {
    // not recieve treatment
    // Model::get_mdc()->record_1_non_treated_case(person->get_location(), person->get_age(),
    // person->get_age_class());

    handle_no_treatment(person);
  }
  // schedule end clinical event for both treatment and non-treatment cases
  person->schedule_end_clinical_event(clinical_caused_parasite_);
}

// TODO: remove this code
//   const auto p = Model::get_random()->random_flat(0.0, 1.0);

//   const auto p_treatment = Model::get_treatment_coverage()->get_probability_to_be_treated(
//       person->get_location(), person->get_age());

// //   std::cout << p_treatment << std::endl;
//   if (p <= p_treatment) {
//     auto *therapy = Model::get_treatment_strategy()->get_therapy(person);

//     person->receive_therapy(therapy, clinical_caused_parasite_);
//     //Statistic increase today treatments
//     Model::get_mdc()->record_1_treatment(person->get_location(), person->get_age(),
//     person->get_age_class(), therapy->get_id());

//     clinical_caused_parasite_->set_update_function(Model::get_instance()->having_drug_update_function());

//     // calculate EAMU
//     Model::get_mdc()->record_AMU_AFU(person, therapy, clinical_caused_parasite_);
//     //        calculateEAMU(therapy);
//     //

//     // death is 90% lower than no treatment
//     if (person->will_progress_to_death_when_recieve_treatment()) {

//       //for each test treatment failure event inside individual
//       // record treatment failure (not tf)
//       //            person->record_treatment_failure_for_test_treatment_failure_events();

//       //no treatment routine
//       receive_no_treatment_routine(person);

//       person->cancel_all_events_except(nullptr);
//       person->set_host_state(Person::DEAD);
//       Model::get_mdc()->record_1_malaria_death(person->get_location(), person->get_age(),true);
//       Model::get_mdc()->record_1_TF(person->get_location(), true);
//       Model::get_mdc()->record_1_treatment_failure_by_therapy(person->get_location(),
//       person->get_age(),
//                                                                    therapy->get_id());
//       return;
//     }

//     person->schedule_update_by_drug_event(clinical_caused_parasite_);

//     person->schedule_end_clinical_event(clinical_caused_parasite_);
//     person->schedule_test_treatment_failure_event(clinical_caused_parasite_,
//     Model::get_config()->get_therapy_parameters().get_tf_testing_day(),
//                                                   therapy->get_id());

//   } else {
//     //not recieve treatment
//     //Statistic store NTF
//     Model::get_mdc()->record_1_TF(person->get_location(), false);
//     Model::get_mdc()->record_1_non_treated_case(person->get_location(), person->get_age(),
//     person->get_age_class());

//     receive_no_treatment_routine(person);
//     if (person->get_host_state()==Person::DEAD) {
//       Model::get_mdc()->record_1_malaria_death(person->get_location(), person->get_age(),false);
//       return;
//     }
//     //
//     //        //schedule for end of clinical event
//     //        std::cout << "progress clinical event" << std::endl;

//     person->schedule_end_clinical_by_no_treatment_event(clinical_caused_parasite_);
//   }

// void ProgressToClinicalEvent::receive_no_treatment_routine(Person *p) {
//   // if (p->will_progress_to_death_when_receive_no_treatment()) {
//   //   p->cancel_all_events_except(nullptr);
//   //   p->set_host_state(Person::DEAD);
//   // }
// }
