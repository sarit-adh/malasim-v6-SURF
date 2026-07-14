//
// Created by Nguyen Tran on 3/16/2018.
//

#include "MFTMultiLocationStrategy.h"

#include <sstream>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Population/Person/Person.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Random.h"

MFTMultiLocationStrategy::MFTMultiLocationStrategy()
    : IStrategy("MFTMultiLocationStrategy", StrategyType::MFTMultiLocation) {}

MFTMultiLocationStrategy::~MFTMultiLocationStrategy() = default;

void MFTMultiLocationStrategy::add_therapy(Therapy* therapy) { therapy_list.push_back(therapy); }

Therapy* MFTMultiLocationStrategy::get_therapy(Person* person) {
  const auto prob = Model::get_random()->random_flat(0.0, 1.0);
  const auto loc = person->get_location();

  double sum = 0;
  for (auto i = 0; i < distribution[loc].size(); i++) {
    sum += distribution[loc][i];
    if (prob <= sum) { return therapy_list[i]; }
  }

  return therapy_list[therapy_list.size() - 1];
}

std::string MFTMultiLocationStrategy::to_string() const {
  std::stringstream sstm;
  sstm << IStrategy::id << "-" << IStrategy::name << "-";

  for (auto i = 0; i < therapy_list.size() - 1; i++) { sstm << therapy_list[i]->get_id() << "::"; }
  sstm << therapy_list[therapy_list.size() - 1]->get_id();
  // for (auto loc = 0; loc < distribution.size(); loc++) {
  //   sstm << "[";
  //   for (auto i = 0; i < distribution[loc].size() - 1; i++) {
  //     sstm << distribution[loc][i] << ",";
  //   }
  //   sstm << distribution[loc][therapy_list.size() - 1] << "]" << std::endl;
  // }
  return sstm.str();
}

void MFTMultiLocationStrategy::update_end_of_time_step() {
  // do nothing here
}

void MFTMultiLocationStrategy::adjust_started_time_point(const int &current_time) {
  starting_time = current_time;
}

void MFTMultiLocationStrategy::monthly_update() {
  if (peak_after == -1) {
    // inflation every year
    for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
      const auto d_act =
          distribution[loc][0]
          * (1
             + (Model::get_config()->get_epidemiological_parameters().get_inflation_factor() / 12));
      distribution[loc][0] = d_act;
      const auto other_d = (1 - d_act) / (distribution[loc].size() - 1);
      for (auto i = 1; i < distribution[loc].size(); i++) { distribution[loc][i] = other_d; }
    }
  } else {
    // increasing linearly
    if (Model::get_scheduler()->current_time() <= starting_time + peak_after) {
      if (distribution[0][0] < 1) {
        for (auto loc = 0; loc < Model::get_config()->number_of_locations(); loc++) {
          for (auto i = 0; i < distribution[loc].size(); i++) {
            const auto dist =
                ((peak_distribution[loc][i] - start_distribution[loc][i])
                 * (Model::get_scheduler()->current_time() - starting_time) / peak_after)
                + start_distribution[loc][i];
            distribution[loc][i] = dist;
          }
        }
      }
    }
  }
}
