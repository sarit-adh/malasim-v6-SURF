/**
 * IntroduceMutantEventBase.cpp
 *
 * Implement the functions for the base class introduce mutant event.
 */
#include "Events/Population/IntroduceMutantEventBase.h"

#include "Configuration/Config.h"
#include "Parasites/Genotype.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Helpers/StringHelpers.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

double IntroduceMutantEventBase::calculate(std::vector<int> &locations) const {
  double mutant_fraction = 0.0;
  double parasite_population_count = 0;
  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();

  // Calculate the frequency of the mutant type across the whole district
  for (auto location : locations) {
    for (auto hs : {Person::ASYMPTOMATIC, Person::CLINICAL}) {
      for (auto ac = 0; ac < Model::get_config()->number_of_age_classes(); ac++) {
        for (auto &person : pi->vPerson()[location][hs][ac]) {
          parasite_population_count +=
              static_cast<double>(person->get_all_clonal_parasite_populations()->size());
          for (auto &pp : *person->get_all_clonal_parasite_populations()) {
            auto chromosome_strings =
                StringHelpers::split<char>(pp->genotype()->get_aa_sequence(), '|', false);
            // spdlog::info(StringHelpers::join(chromosome_strings,"#"));
            for (const auto &allele_info : alleles) {
              if (chromosome_strings[std::get<0>(allele_info)][std::get<1>(allele_info)]
                  == std::get<2>(allele_info)) {
                mutant_fraction++;
              }
            }
          }
        }
      }
    }
  }

  // Calculate and return the target fraction across the whole district
  mutant_fraction =
      (parasite_population_count == 0) ? 0 : mutant_fraction / parasite_population_count;
  return (fraction - mutant_fraction);
}

// Induce the mutations in individuals across the district, return the total
// number of mutations inflicted
int IntroduceMutantEventBase::mutate(std::vector<int> &locations, double target_fraction) const {
  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();

  auto mutations_count = 0;
  for (auto location : locations) {
    for (auto ac = 0; ac < Model::get_config()->number_of_age_classes(); ac++) {
      // Note the infected individuals in the location
      auto infections = pi->vPerson()[location][Person::ASYMPTOMATIC][ac].size()
                        + pi->vPerson()[location][Person::CLINICAL][ac].size();

      if (infections > 0) {
        spdlog::trace("mutate location: {}, age class: {}, infections: {}", location, ac,
                      infections);
      }
      // Use a Poisson distribution to determine the number of mutations in this
      // location
      auto mutations =
          Model::get_random()->random_poisson(static_cast<double>(infections) * target_fraction);
      // spdlog::debug("mutate location: {}, age class: {}, mutations: {}",
      //               location, ac, mutations);
      if (mutations == 0) { continue; }
      mutations_count += mutations;

      // Note the number of asymptomatic cases for indexing operations
      auto asymptomatic = pi->vPerson()[location][Person::ASYMPTOMATIC][ac].size();

      // Perform the target number of mutations, operating on all infected
      // individuals in the location and age class
      for (auto count = 0; count < mutations; count++) {
        // Use a uniform draw to get the index of an infected individual
        auto index = Model::get_random()->random_uniform(infections);

        // Select the person based upon the index
        Person* person = nullptr;
        if (index < asymptomatic) {
          person = pi->vPerson()[location][Person::ASYMPTOMATIC][ac][index];
        } else {
          person = pi->vPerson()[location][Person::CLINICAL][ac][index - asymptomatic];
        }

        // Mutate all the clonal populations the individual is carrying
        for (auto &pp : *person->get_all_clonal_parasite_populations()) {
          auto* old_genotype = pp->genotype();
          auto* new_genotype = old_genotype->modify_genotype_allele(alleles, Model::get_config());
          spdlog::trace("location {} Introduce mutant new genotype: {}", location,
                        new_genotype->get_aa_sequence());
          pp->set_genotype(new_genotype);
        }
      }
    }
  }

  // Return the total mutations count
  return mutations_count;
}
