#include "TurnOffMutationEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

TurnOffMutationEvent::TurnOffMutationEvent(const int &at_time) { set_time(at_time); }

void TurnOffMutationEvent::do_execute() {
  Model::get_config()->get_genotype_parameters().set_mutation_probability_per_locus(0.0);
  spdlog::info("{}: turn mutation off", Model::get_scheduler()->get_current_date_string());
}
