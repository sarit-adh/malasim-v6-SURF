#include "IntroduceParasitesPeriodicallyEventV2.h"

#include <cstdint>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "MDC/ModelDataCollector.h"
#include "Population/ClonalParasitePopulation.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Index/PersonIndexByLocationStateAgeClass.h"
#include "Utils/Random.h"

// OBJECTPOOL_IMPL(IntroduceParasitesPeriodicallyEventV2)

IntroduceParasitesPeriodicallyEventV2::IntroduceParasitesPeriodicallyEventV2(
    const std::vector<std::vector<double>> &allele_distributions_in,
    const int &location,
    const int &duration,
    const int &number_of_cases,
    const int &start_day_in,
    const int &end_day_in)
    : allele_distributions(allele_distributions_in),
      location_(location),
      duration_(duration),
      number_of_cases_(number_of_cases),
      start_day(start_day_in),
      end_day(end_day_in) {
  set_time(start_day);

  if (end_day_in == -1) {
    end_day = Model::get_config()->get_simulation_timeframe().get_total_time();
  }
}

IntroduceParasitesPeriodicallyEventV2::~IntroduceParasitesPeriodicallyEventV2() = default;

void IntroduceParasitesPeriodicallyEventV2::do_execute() {
  // TODO: rework this

  // std::cout << date::year_month_day{ Model::get_scheduler()->calendar_date } << ":import
  // periodically event" << std::endl;
  // schedule importation for the next day
  if (Model::get_scheduler()->current_time() < end_day) {
    auto event = std::make_unique<IntroduceParasitesPeriodicallyEventV2>(
        allele_distributions, location_, duration_, number_of_cases_,
        Model::get_scheduler()->current_time() + 1);
    Model::get_scheduler()->schedule_population_event(std::move(event));
    // schedule_event(Model::get_scheduler(), this);
  }
  //  else {
  //    LOG(INFO) << "Hello End importation" ;
  //  }

  const auto number_of_importation_cases =
      Model::get_random()->random_poisson(static_cast<double>(number_of_cases_) / duration_);
  if (Model::get_mdc()->popsize_by_location_hoststate()[location_][0]
      < number_of_importation_cases) {
    return;
  }

  //    std::cout << number_of_cases_ << std::endl;
  auto* pi = Model::get_population()->get_person_index<PersonIndexByLocationStateAgeClass>();
  if (number_of_importation_cases > 0) {
    spdlog::debug("Day {}: Importing {} at location {}", Model::get_scheduler()->current_time(),
                  number_of_importation_cases, location_);
  }

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
    std::vector<int> gene_structure(allele_distributions.size());

    for (int j = 0; j < allele_distributions.size(); ++j) {
      int k_count = 0;
      double sum = allele_distributions[j][k_count];
      double rand_uni = Model::get_random()->random_uniform();

      while (rand_uni > sum) {
        k_count += 1;
        sum += allele_distributions[j][k_count];
      }
      gene_structure[j] = k_count;
    }

    Genotype* imported_genotype =
        Model::get_genotype_db()->get_genotype_from_alleles_structure(gene_structure);

    auto* blood_parasite = person->add_new_parasite_to_blood(imported_genotype);

    auto size = Model::get_config()
                    ->get_parasite_parameters()
                    .get_parasite_density_levels()
                    .get_log_parasite_density_asymptomatic();

    blood_parasite->set_gametocyte_level(
        Model::get_config()->get_epidemiological_parameters().get_gametocyte_level_full());
    blood_parasite->set_last_update_log10_parasite_density(size);
    blood_parasite->set_update_function(Model::immunity_clearance_update_function());
  }
  spdlog::info("Day {}: Importing v2 {} at location {}", Model::get_scheduler()->current_time(),
               number_of_importation_cases, location_);
}
