#include "ChangeTreatmentCoverageEvent.h"

#include <memory>

#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

ChangeTreatmentCoverageEvent::ChangeTreatmentCoverageEvent(
    std::unique_ptr<ITreatmentCoverageModel> tcm)
    : treatment_coverage_model{std::move(tcm)} {
  if (!treatment_coverage_model) {
    throw std::runtime_error("ChangeTreatmentCoverageEvent: received null treatment model");
  }

  set_time(treatment_coverage_model->starting_time);
}

ChangeTreatmentCoverageEvent::~ChangeTreatmentCoverageEvent() = default;

void ChangeTreatmentCoverageEvent::do_execute() {
  spdlog::info("{}: change treatment coverage model to {}",
               Model::get_scheduler()->get_current_date_string(), treatment_coverage_model->type);
  Model::get_instance()->set_treatment_coverage(std::move(treatment_coverage_model));
  set_executable(false);
}
