#define NOMINMAX

#include "SingleRoundMDAEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Events/ReceiveMDATherapyEvent.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

SingleRoundMDAEvent::SingleRoundMDAEvent(const int &at_time) {
  set_time(at_time);
  fraction_population_targeted_ = std::vector<double>();
}

void SingleRoundMDAEvent::do_execute() {
  spdlog::info("{}: executing Single Round MDA", Model::get_scheduler()->get_current_date_string());

  // for all location
  for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
    // step 1: get number of individuals for MDA
    auto* pi_lsa = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();
    std::vector<Person*> all_persons_in_location;
    for (auto hs = 0; hs < Person::DEAD; hs++) {
      for (auto ac = 0; ac < Model::get_config()->number_of_age_classes(); ac++) {
        for (auto* person : pi_lsa->vPerson()[loc][hs][ac]) {
          all_persons_in_location.push_back(person);
        }
      }
    }

    const auto number_indidividuals_in_location = all_persons_in_location.size();
    auto number_of_individuals_will_receive_mda = Model::get_random()->random_poisson(
        fraction_population_targeted_[loc] * static_cast<double>(number_indidividuals_in_location));

    number_of_individuals_will_receive_mda =
        number_of_individuals_will_receive_mda > number_indidividuals_in_location
            ? static_cast<int>(number_indidividuals_in_location)
            : number_of_individuals_will_receive_mda;
    // shuffle app_persons_in_location index for sampling without replacement
    Model::get_random()->shuffle(all_persons_in_location);

    for (auto p_i = 0; p_i < number_of_individuals_will_receive_mda; p_i++) {
      auto* person = all_persons_in_location[p_i];
      // step 2: determine whether person will receive treatment
      const auto prob = Model::get_random()->random_flat(0.0, 1.0);
      if (prob < person->prob_present_at_mda()) {
        // receive MDA
        auto* therapy =
            Model::get_therapy_db()
                [Model::get_config()->get_strategy_parameters().get_mda().get_mda_therapy_id()]
                    .get();
        // schedule received therapy in within days_to_complete_all_treatments
        int days_to_receive_mda_therapy =
            static_cast<int>(Model::get_random()->random_uniform(days_to_complete_all_treatments_))
            + 1;

        person->schedule_receive_mda_therapy_event(therapy, days_to_receive_mda_therapy);
      }
    }
  }
}
