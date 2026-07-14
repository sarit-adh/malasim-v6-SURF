#include "NestedMFTMultiLocationStrategy.h"

#include <algorithm>  // For std::min and std::max
#include <sstream>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Population/Person/Person.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Random.h"

NestedMFTMultiLocationStrategy::NestedMFTMultiLocationStrategy()
    : IStrategy("NestedMFTMultiLocationStrategy", StrategyType::NestedMFTMultiLocation) {}

NestedMFTMultiLocationStrategy::~NestedMFTMultiLocationStrategy() = default;

void NestedMFTMultiLocationStrategy::add_strategy(IStrategy* strategy) {
  strategy_list.push_back(strategy);
}

void NestedMFTMultiLocationStrategy::add_therapy(Therapy* therapy) {}

Therapy* NestedMFTMultiLocationStrategy::get_therapy(Person* person) {
  const auto loc = person->get_location();
  const auto prob = Model::get_random()->random_flat(0.0, 1.0);

  double sum = 0;
  for (auto i = 0; i < distribution[loc].size(); i++) {
    sum += distribution[loc][i];
    if (prob <= sum) { return strategy_list[i]->get_therapy(person); }
  }
  return strategy_list[strategy_list.size() - 1]->get_therapy(person);
}

std::string NestedMFTMultiLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name;
  // for (auto i : distribution[0]) {
  //   sstm << i << ",";
  // }
  // sstm << std::endl;
  //
  // for (auto i : start_distribution[0]) {
  //   sstm << i << ",";
  // }
  // sstm << std::endl;
  return sstm.str();
}

void NestedMFTMultiLocationStrategy::update_end_of_time_step() {
  // update each strategy in the nest
  for (auto &strategy : strategy_list) { strategy->update_end_of_time_step(); }
}

void NestedMFTMultiLocationStrategy::adjust_distribution(const int &time) {
  if (peak_after == -1) {
    // inflation every year
    for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
      const auto d_act =
          distribution[loc][0]
          * ((Model::get_config()->get_epidemiological_parameters().get_inflation_factor() / 12)
             + 1);
      distribution[loc][0] = d_act;
      const auto other_d = (1 - d_act) / (distribution[loc].size() - 1);
      for (auto i = 1; i < distribution[loc].size(); i++) { distribution[loc][i] = other_d; }
    }
  } else {
    // increasing linearly
    if (time <= starting_time + peak_after) {
      // Calculate fraction of progress towards peak (0.0 to 1.0)
      double progress_fraction = (time - starting_time) / static_cast<double>(peak_after);

      for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
        for (auto i = 0; i < distribution[loc].size(); i++) {
          // Linear interpolation between start and peak distribution
          double dist =
              start_distribution[loc][i]
              + ((peak_distribution[loc][i] - start_distribution[loc][i]) * progress_fraction);

          // Ensure we don't exceed peak values
          dist = peak_distribution[loc][i] > start_distribution[loc][i]
                     ? std::min(dist, peak_distribution[loc][i])
                     : std::max(dist, peak_distribution[loc][i]);

          distribution[loc][i] = dist;
        }
      }
    } else {
      // After peak, use peak distribution
      for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
        for (auto i = 0; i < distribution[loc].size(); i++) {
          distribution[loc][i] = peak_distribution[loc][i];
        }
      }
    }
  }
}

void NestedMFTMultiLocationStrategy::adjust_started_time_point(const int &current_time) {
  starting_time = current_time;
  // update each strategy in the nest
  for (auto* strategy : strategy_list) { strategy->adjust_started_time_point(current_time); }
}

void NestedMFTMultiLocationStrategy::monthly_update() {
  adjust_distribution(Model::get_scheduler()->current_time());

  for (auto* strategy : strategy_list) { strategy->monthly_update(); }

  // for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
  //   std::cout << distribution[loc] << std::endl;
  // }
}
