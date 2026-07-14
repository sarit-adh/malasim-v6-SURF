#include "MFTRebalancingStrategy.h"

#include <iostream>
#include <string>

#include "Core/Scheduler/Scheduler.h"
#include "MDC/ModelDataCollector.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"

MFTRebalancingStrategy::MFTRebalancingStrategy() {
  name = "MFTRebalancingStrategy";
  type = StrategyType::MFTRebalancing;
}

MFTRebalancingStrategy::~MFTRebalancingStrategy() = default;

std::string MFTRebalancingStrategy::to_string() const {
  std::stringstream sstm;
  //    sstm << IStrategy::id << "-" << IStrategy::name << "-";
  //
  //    for (int i = 0; i < therapy_list().size() - 1; i++) {
  //        sstm << therapy_list()[i]->id() << ",";
  //    }
  //    sstm << therapy_list()[therapy_list().size() - 1]->id() << "-";
  //
  //    for (int i = 0; i < distribution().size() - 1; i++) {
  //        sstm << distribution()[i] << ",";
  //    }
  //    sstm << distribution()[therapy_list().size() - 1] << "-" << update_distribution_duration_;
  sstm << MFTStrategy::to_string() << "-" << update_duration_after_rebalancing;
  return sstm.str();
}

void MFTRebalancingStrategy::update_end_of_time_step() {
  if (Model::get_scheduler()->current_time() == latest_adjust_distribution_time) {
    // actual trigger adjust distribution
    for (auto i = 0; i < distribution.size(); i++) { distribution[i] = next_distribution[i]; }
    next_update_time = Model::get_scheduler()->current_time() + update_duration_after_rebalancing;
    std::cout << Model::get_scheduler()->get_current_date_string()
              << ": MFT Rebalancing adjust distribution: " << to_string();
    //            std::cout << to_string() << std::endl;
  } else {
    if (Model::get_scheduler()->current_time() == next_update_time) {
      double sum = 0;
      for (auto i = 0; i < distribution.size(); i++) {
        std::cout << "Current treatment failure rate of " << therapy_list[i]->get_id() << " : "
                  << Model::get_mdc()->current_tf_by_therapy()[therapy_list[i]->get_id()];
        if (Model::get_mdc()->current_tf_by_therapy()[therapy_list[i]->get_id()] < 0.05) {
          next_distribution[i] = 1.0 / 0.05;
        } else {
          next_distribution[i] =
              1.0 / Model::get_mdc()->current_tf_by_therapy()[therapy_list[i]->get_id()];
        }
        sum += next_distribution[i];
      }

      for (auto i = 0; i < distribution.size(); i++) {
        next_distribution[i] = next_distribution[i] / sum;
      }
      latest_adjust_distribution_time =
          Model::get_scheduler()->current_time() + delay_until_actual_trigger;
      std::cout << Model::get_scheduler()->get_current_date_string()
                << ": MFT Rebalancing will adjust distribution after " << delay_until_actual_trigger
                << "days";
    }
  }
}

void MFTRebalancingStrategy::adjust_started_time_point(const int &current_time) {
  next_update_time = Model::get_scheduler()->current_time() + update_duration_after_rebalancing;
  latest_adjust_distribution_time = -1;
}
