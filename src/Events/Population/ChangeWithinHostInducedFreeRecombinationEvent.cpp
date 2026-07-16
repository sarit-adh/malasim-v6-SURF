#include "ChangeWithinHostInducedFreeRecombinationEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

ChangeWithinHostInducedFreeRecombinationEvent::ChangeWithinHostInducedFreeRecombinationEvent(
    const bool &value, const int &at_time)
    : value{value} {
  set_time(at_time);
}
void ChangeWithinHostInducedFreeRecombinationEvent::do_execute() {
  Model::get_config()->get_mosquito_parameters().set_within_host_induced_free_recombination(value);
  spdlog::info("{}: Change within host induced free recombination to {}",
               Model::get_scheduler()->get_current_date_string(), value);
}
