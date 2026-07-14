/*
 * File:   CyclingStrategy.cpp
 * Author: nguyentran
 *
 * Created on June 4, 2013, 11:10 AM
 */

#include "CyclingStrategy.h"

#include <sstream>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "IStrategy.h"
#include "MDC/ModelDataCollector.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Helpers/StringHelpers.h"

CyclingStrategy::CyclingStrategy() : IStrategy("CyclingStrategy", StrategyType::Cycling) {}

CyclingStrategy::~CyclingStrategy() = default;

void CyclingStrategy::add_therapy(Therapy* therapy) { therapy_list.push_back(therapy); }

void CyclingStrategy::switch_therapy() {
  //    std::cout << "Switch from: " << index_ << "\t - to: " << index_ + 1;
  index++;
  index %= therapy_list.size();
  Model::get_mdc()->update_utl_vector();

  // TODO: cycling_time should be match with calendar day
  next_switching_day = Model::get_scheduler()->current_time() + cycling_time;
  spdlog::info("{}: Cycling Strategy switch Therapy to: {}",
               Model::get_scheduler()->get_current_date_string(), therapy_list[index]->get_id());
}

Therapy* CyclingStrategy::get_therapy(Person* person) {
  // int index = ((Global::scheduler->currentTime - Global::startTreatmentDay) / circleTime) %
  // therapyList.size();
  //     std::cout << therapy_list()[index_]->id() << std::endl;
  return therapy_list[index];
}

std::string CyclingStrategy::to_string() const {
  std::stringstream sstm;
  sstm << id << "-" << name << "-";
  std::string sep;
  for (auto* therapy : therapy_list) {
    sstm << sep << therapy->get_id();
    sep = ",";
  }
  return sstm.str();
}

void CyclingStrategy::update_end_of_time_step() {
  if (Model::get_scheduler()->current_time() == next_switching_day) {
    switch_therapy();
    //            std::cout << to_string() << std::endl;
  }
}

void CyclingStrategy::adjust_started_time_point(const int &current_time) {
  next_switching_day = Model::get_scheduler()->current_time() + cycling_time;
  index = 0;
}

void CyclingStrategy::monthly_update() {}
