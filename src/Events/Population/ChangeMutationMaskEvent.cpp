#include "ChangeMutationMaskEvent.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"

ChangeMutationMaskEvent::ChangeMutationMaskEvent(const std::vector<bool> &mask, const int &at_time)
    : mask{mask} {
  set_time(at_time);
}

void ChangeMutationMaskEvent::do_execute() {
  Model::get_config()->get_genotype_parameters().set_mutation_mask(mask);
  spdlog::info("{}: change mutation mask to {} enabled positions across {} entries",
               Model::get_scheduler()->get_current_date_string(),
               std::count(mask.begin(), mask.end(), true), mask.size());
}
