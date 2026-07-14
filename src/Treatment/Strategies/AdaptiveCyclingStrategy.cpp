/*
 * File:   AdaptiveCyclingStrategy.cpp
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:10 AM
 */

#include "AdaptiveCyclingStrategy.h"

#include <sstream>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "IStrategy.h"
#include "MDC/ModelDataCollector.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Helpers/StringHelpers.h"

AdaptiveCyclingStrategy::AdaptiveCyclingStrategy()
    : IStrategy("AdaptiveCyclingStrategy", StrategyType::AdaptiveCycling) {}

AdaptiveCyclingStrategy::~AdaptiveCyclingStrategy() = default;

void AdaptiveCyclingStrategy::add_therapy(Therapy* therapy) { therapy_list.push_back(therapy); }

void AdaptiveCyclingStrategy::switch_therapy() {
  //    std::cout << "Switch from: " << index_ << "\t - to: " << index_ + 1;
  index++;
  index %= therapy_list.size();

  Model::get_mdc()->update_utl_vector();
  spdlog::info("{}: Adaptive Cycling Strategy switch Therapy to: {}",
               Model::get_scheduler()->get_current_date_string(), therapy_list[index]->get_id());
}

Therapy* AdaptiveCyclingStrategy::get_therapy(Person* person) { return therapy_list[index]; }

std::string AdaptiveCyclingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto* therapy : therapy_list) {
    sstm << sep << therapy->get_id();
    sep = ",";
  }
  return sstm.str();
}

void AdaptiveCyclingStrategy::update_end_of_time_step() {
  if (Model::get_scheduler()->current_time() == latest_switch_time) {
    switch_therapy();
    //            std::cout << to_string() << std::endl;
  } else {
    if (Model::get_mdc()->current_tf_by_therapy()[get_therapy(nullptr)->get_id()] > trigger_value) {
      // TODO:: turn_off_days and delay_until_actual_trigger should be match with calendar day
      if (Model::get_scheduler()->current_time() > latest_switch_time + turn_off_days) {
        latest_switch_time = Model::get_scheduler()->current_time() + delay_until_actual_trigger;
        spdlog::info("{}: Adaptive Cycling will switch therapy next year",
                     Model::get_scheduler()->get_current_date_string());
      }
    }
  }
}

void AdaptiveCyclingStrategy::adjust_started_time_point(const int &current_time) {
  latest_switch_time = -1;
  index = 0;
}

void AdaptiveCyclingStrategy::monthly_update() {}
