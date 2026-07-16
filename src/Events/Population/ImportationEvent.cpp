#include "ImportationEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Parasites/Genotype.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

// OBJECTPOOL_IMPL(ImportationEvent)

ImportationEvent::ImportationEvent(const int &location,
                                   const int &execute_at,
                                   const int &genotype_id,
                                   const int &number_of_cases,
                                   const std::vector<std::vector<double>> &allele_distributions)
    : location_(location),
      genotype_id_(genotype_id),
      number_of_cases_(number_of_cases),
      allele_distributions_(allele_distributions) {
  set_time(execute_at);
}

void ImportationEvent::do_execute() {
  const auto number_of_importation_cases = Model::get_random()->random_poisson(number_of_cases_);
  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();

  for (auto i = 0; i < number_of_importation_cases; i++) {
    const std::size_t ind_ac =
        Model::get_random()->random_uniform(static_cast<std::uint64_t>(pi->vPerson()[0][0].size()));
    if (pi->vPerson()[0][0][ind_ac].empty()) { continue; }
    const std::size_t index =
        Model::get_random()->random_uniform(pi->vPerson()[0][0][ind_ac].size());
    auto* person = pi->vPerson()[0][0][ind_ac][index];

    person->get_immune_system()->set_increase(true);
    person->set_host_state(Person::ASYMPTOMATIC);

    auto* blood_parasite = person->add_new_parasite_to_blood(
        Model::get_genotype_db()->at(static_cast<const std::uint64_t &>(genotype_id_)));

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
  spdlog::info("Day {}: Importation event: {} at location {} with genotype {}",
               Model::get_scheduler()->current_time(), number_of_cases_, location_,
               Model::get_genotype_db()->at(genotype_id_)->get_aa_sequence());
}
