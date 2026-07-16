#include "ChangeInterruptedFeedingRateEvent.h"

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

ChangeInterruptedFeedingRateEvent::ChangeInterruptedFeedingRateEvent(const int &location,
                                                                     const double &ifr,
                                                                     const int &at_time)
    : location{location}, ifr{ifr} {
  set_time(at_time);
}

void ChangeInterruptedFeedingRateEvent::do_execute() {
  Model::get_config()->location_db()[location].mosquito_ifr = ifr;
  spdlog::info("{}: Change interrupted feeding rate at location {} to {}",
               Model::get_scheduler()->get_current_date_string(), location, ifr);
}
