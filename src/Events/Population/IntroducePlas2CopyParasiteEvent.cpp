#include "IntroducePlas2CopyParasiteEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Parasites/Genotype.h"
#include "Population/Population.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

IntroducePlas2CopyParasiteEvent::IntroducePlas2CopyParasiteEvent(
    const int &location,
    const int &execute_at,
    const double &fraction,
    const std::vector<std::tuple<int, int, char>> &allele_map)
    : location_(location), fraction_(fraction), alleles_(allele_map) {
  set_time(execute_at);
}

void IntroducePlas2CopyParasiteEvent::do_execute() {
  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();

  for (std::size_t j = 0; j < Model::get_config()->number_of_age_classes(); ++j) {
    const auto number_infected_individual_in_ac = pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()
                                                  + pi->vPerson()[0][Person::CLINICAL][j].size();
    const auto number_of_importation_cases = Model::get_random()->random_poisson(
        static_cast<double>(number_infected_individual_in_ac) * fraction_);
    if (number_of_importation_cases == 0) continue;
    for (auto i = 0; i < number_of_importation_cases; i++) {
      const std::size_t index =
          Model::get_random()->random_uniform(number_infected_individual_in_ac);

      Person* person = nullptr;
      if (index < pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()) {
        person = pi->vPerson()[0][Person::ASYMPTOMATIC][j][index];
      } else {
        person = pi->vPerson()[0][Person::CLINICAL][j]
                              [index - pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()];
      }
      // mutate all
      for (auto &pp : *person->get_all_clonal_parasite_populations()) {
        auto* old_genotype = pp->genotype();
        auto* new_genotype =
            old_genotype->modify_genotype_allele({std::tuple(14, 1, '2')}, Model::get_config());
        pp->set_genotype(new_genotype);
      }
    }
  }

  spdlog::info("{}: Introduce Plas2 Copy Parasite with fraction {}",
               Model::get_scheduler()->get_current_date_string(), fraction_);
}
