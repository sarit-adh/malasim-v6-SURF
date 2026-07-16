#include "ChangeTreatmentStrategyEvent.h"

#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

ChangeTreatmentStrategyEvent::ChangeTreatmentStrategyEvent(const int &strategy_id,
                                                           const int &at_time)
    : strategy_id_(strategy_id) {
  set_time(at_time);
}

void ChangeTreatmentStrategyEvent::do_execute() {
  Model::get_instance()->set_treatment_strategy(strategy_id_);
  spdlog::info("Day {}: Change treatment strategy to {}", Model::get_scheduler()->current_time(),
               strategy_id_);
}
