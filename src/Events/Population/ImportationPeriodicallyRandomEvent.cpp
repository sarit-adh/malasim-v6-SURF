/*
 * ImportationPeriodicallyRandomEvent.cpp
 *
 * Implement the event.
 */
#include "ImportationPeriodicallyRandomEvent.h"

#include <cstddef>
#include <cstdint>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Population/ClinicalUpdateFunction.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

Person* ImportationPeriodicallyRandomEvent::get_random_susceptible() {
  auto* random = Model::get_random();
  auto* population = Model::get_population();

  auto* person_index = population->get_person_index<PersonIndexByLocationStateAgeClass>();

  const auto &persons = person_index->vPerson();
  const auto susceptible_state = Person::HostStates::SUSCEPTIBLE;

  std::size_t susceptible_count = 0;

  for (const auto &location : persons) {
    for (const auto &age_group : location[susceptible_state]) {
      susceptible_count += age_group.size();
    }
  }

  if (susceptible_count == 0) { return nullptr; }

  std::size_t target = random->random_uniform(susceptible_count);

  for (const auto &location : persons) {
    for (const auto &age_group : location[susceptible_state]) {
      if (target < age_group.size()) { return age_group[target]; }

      target -= age_group.size();
    }
  }

  throw std::logic_error("Failed to select susceptible person despite non-zero population");
}

void ImportationPeriodicallyRandomEvent::do_execute() {
  auto* scheduler = Model::get_scheduler();
  auto* random = Model::get_random();

  const int days_in_month = static_cast<int>(scheduler->get_days_in_current_month());

  if (days_in_month <= 0) { throw std::logic_error("Current month has no valid days"); }

  const double daily_mean = static_cast<double>(count_) / static_cast<double>(days_in_month);

  const int infections = random->random_poisson(daily_mean);

  for (int infection = 0; infection < infections; ++infection) {
    Person* person = nullptr;
    person = get_random_susceptible();

    if (person == nullptr) {
      spdlog::warn(
          "Unable to introduce all requested infections: "
          "no susceptible individuals remain");
      break;
    }

    const auto location = person->get_location();

    infect(person, genotype_id_);

    spdlog::debug("{} - Introduced infection at {}", scheduler->get_current_date_string(),
                  location);
  }

  int next_event_time = scheduler->current_time() + 1;

  if (scheduler->is_today_last_day_of_month()) {
    const int year = scheduler->get_current_year();
    const auto month = scheduler->get_current_month_in_year();

    const auto next_run_date =
        date::year_month_day{date::year{year + 1}, date::month{month}, date::day{1}};

    next_event_time = static_cast<int>(
        (date::sys_days{next_run_date}
         - date::sys_days{Model::get_config()->get_simulation_timeframe().get_starting_date()})
            .count());
  }

  auto event = std::make_unique<ImportationPeriodicallyRandomEvent>(genotype_id_, next_event_time,
                                                                    count_, log_parasite_density_);

  scheduler->schedule_population_event(std::move(event));
}

void ImportationPeriodicallyRandomEvent::infect(Person* person, int genotype_id) const {
  // Prepare the immune system
  person->get_immune_system()->set_increase(true);
  person->set_host_state(Person::ASYMPTOMATIC);

  // Prepare the genotype
  Genotype* genotype = Model::get_genotype_db()->at(genotype_id);

  // Inflict the infection
  auto* blood_parasite = person->add_new_parasite_to_blood(genotype);
  blood_parasite->set_gametocyte_level(
      Model::get_config()->get_epidemiological_parameters().get_gametocyte_level_full());
  blood_parasite->set_last_update_log10_parasite_density(log_parasite_density_);
  blood_parasite->set_update_function(Model::immunity_clearance_update_function());

  // Check if the configured log density is equal to or greater than the
  // standard for clinical
  if (log_parasite_density_ >= Model::get_config()
                                   ->get_parasite_parameters()
                                   .get_parasite_density_levels()
                                   .get_log_parasite_density_clinical()) {
    blood_parasite->set_update_function(Model::progress_to_clinical_update_function());
    person->schedule_progress_to_clinical_event(blood_parasite);
  }
}
