#include "ImportationPeriodicallyEvent.h"

#include "MDC/ModelDataCollector.h"
#include "Parasites/Genotype.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
// OBJECTPOOL_IMPL(ImportationPeriodicallyEvent)

ImportationPeriodicallyEvent::ImportationPeriodicallyEvent(const int &location,
                                                           const int &duration,
                                                           int genotype_id,
                                                           const int &number_of_cases,
                                                           const int &start_day)
    : location_(location),
      duration_(duration),
      genotype_id_(genotype_id),
      number_of_cases_(number_of_cases) {
  // TODO: remove start_day_
  set_time(start_day);
}

ImportationPeriodicallyEvent::~ImportationPeriodicallyEvent() = default;

void ImportationPeriodicallyEvent::do_execute() {
  // std::cout << date::year_month_day{ Model::get_scheduler()->calendar_date }
  // << ":import periodically event" << std::endl; schedule importation for the
  // next day
  auto event = std::make_unique<ImportationPeriodicallyEvent>(
      location_, duration_, genotype_id_, number_of_cases_,
      Model::get_scheduler()->current_time() + 1);
  Model::get_scheduler()->schedule_population_event(std::move(event));

  const auto number_of_importation_cases =
      Model::get_random()->random_poisson(static_cast<double>(number_of_cases_) / duration_);
  if (Model::get_mdc()->popsize_by_location_hoststate()[location_][0]
      < number_of_importation_cases) {
    return;
  }

  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();
  for (auto i = 0; i < number_of_importation_cases; i++) {
    std::size_t ind_ac = Model::get_random()->random_uniform(
        static_cast<std::uint64_t>(pi->vPerson()[location_][0].size()));
    if (pi->vPerson()[location_][0][ind_ac].empty()) { continue; }

    std::size_t index =
        Model::get_random()->random_uniform(pi->vPerson()[location_][0][ind_ac].size());
    auto* person = pi->vPerson()[location_][0][ind_ac][index];

    person->get_immune_system()->set_increase(true);
    person->set_host_state(Person::ASYMPTOMATIC);

    // check and draw random Genotype
    Genotype* imported_genotype = nullptr;

    // TODO: rework on this function to have a random genotype string
    uint32_t random_id = Model::get_random()->random_uniform<int>(0, 1);

    switch (genotype_id_) {
      case -1:
        //      new genotype will have 50% chance of 580Y and 50% plasmepsin-2
        //      copy, last allele will always be x
        if (random_id % 2 == 1) { random_id -= 1; }
        imported_genotype = Model::get_genotype_db()->at(random_id);
        break;
      case -2:
        // all random even last xX locus new genotype will have
        // 50% chance of 580Y and 50% plasmepsin-2 copy and %50 X ....
        imported_genotype = Model::get_genotype_db()->at(random_id);
        break;
      default:
        imported_genotype = Model::get_genotype_db()->at(genotype_id_);
    }

    auto* blood_parasite = person->add_new_parasite_to_blood(imported_genotype);
    //    std::cout << "hello"<< std::endl;

    auto size = Model::get_config()
                    ->get_parasite_parameters()
                    .get_parasite_density_levels()
                    .get_log_parasite_density_asymptomatic();

    blood_parasite->set_gametocyte_level(
        Model::get_config()->get_epidemiological_parameters().get_gametocyte_level_full());
    blood_parasite->set_last_update_log10_parasite_density(size);
    blood_parasite->set_update_function(Model::immunity_clearance_update_function());

    //        Model::get_population()->initial_infection(pi->vPerson()[0][0][ind_ac][index],
    //        Model::CONFIG->parasite_db()->get(0));
  }
  if (number_of_importation_cases > 0) {
    spdlog::debug("{} - Importing (periodically) {} at location {} with genotype {}",
                  Model::get_scheduler()->get_current_date_string(), number_of_importation_cases,
                  location_, Model::get_genotype_db()->at(genotype_id_)->get_aa_sequence());
  }
}
