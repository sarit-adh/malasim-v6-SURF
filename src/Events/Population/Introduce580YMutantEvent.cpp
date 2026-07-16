
#include "Introduce580YMutantEvent.h"

#include "Configuration//Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Parasites/Genotype.h"
#include "Population/Population.h"
#include "Population/SingleHostClonalParasitePopulations.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

Introduce580YMutantEvent::Introduce580YMutantEvent(
    const int &location,
    const int &execute_at,
    const double &fraction,
    const std::vector<std::tuple<int, int, char>> &alleles)
    : location_(location), fraction_(fraction), alleles_(alleles) {
  set_time(execute_at);
}

Introduce580YMutantEvent::~Introduce580YMutantEvent() = default;

void Introduce580YMutantEvent::do_execute() {
  // TODO: rework on this

  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();

  // get the approximate current frequency of 580Y in the population
  // and only fill up the different between input fraction and the current frequency

  double current_580_y_fraction = 0.0;
  double total_population_count = 0;
  for (int j = 0; j < Model::get_config()->number_of_age_classes(); ++j) {
    for (Person* person : pi->vPerson()[0][Person::ASYMPTOMATIC][j]) {
      total_population_count +=
          static_cast<double>(person->get_all_clonal_parasite_populations()->size());
      for (auto &pp : *person->get_all_clonal_parasite_populations()) {
        //        if (pp->genotype()->aa_structure()[2] == 1) {
        //          current_580Y_fraction++;
        //        }
      }
    }
  }

  current_580_y_fraction =
      total_population_count == 0 ? 0 : current_580_y_fraction / total_population_count;
  double target_fraction = fraction_ - current_580_y_fraction;
  if (target_fraction <= 0) {
    spdlog::info("{}: Introduce 580Y Copy event with 0 cases",
                 Model::get_scheduler()->get_current_date_string());
    return;
  }
  //  std::cout << target_fraction << std::endl;

  for (int j = 0; j < Model::get_config()->number_of_age_classes(); ++j) {
    const auto number_infected_individual_in_ac = pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()
                                                  + pi->vPerson()[0][Person::CLINICAL][j].size();
    const auto number_of_importation_cases = Model::get_random()->random_poisson(
        static_cast<double>(number_infected_individual_in_ac) * target_fraction);
    if (number_of_importation_cases == 0) continue;
    for (auto i = 0; i < number_of_importation_cases; i++) {
      const size_t index = Model::get_random()->random_uniform(number_infected_individual_in_ac);

      Person* person = nullptr;
      if (index < pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()) {
        person = pi->vPerson()[0][Person::ASYMPTOMATIC][j][index];
      } else {
        person = pi->vPerson()[0][Person::CLINICAL][j]
                              [index - pi->vPerson()[0][Person::ASYMPTOMATIC][j].size()];
      }

      // mutate all clonal populations
      for (auto &pp : *person->get_all_clonal_parasite_populations()) {
        auto* old_genotype = pp->genotype();
        auto* new_genotype = old_genotype->modify_genotype_allele(alleles_, Model::get_config());
        pp->set_genotype(new_genotype);
      }
    }
  }

  spdlog::info("{}: Introduce 580Y mutant event with fraction {}",
               Model::get_scheduler()->get_current_date_string(), target_fraction);
}
