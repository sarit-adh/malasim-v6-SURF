#include "InfantImmuneComponent.h"

#include <cmath>

#include "Core/Scheduler/Scheduler.h"
#include "Core/types.h"
#include "ImmuneSystem.h"
#include "ImmuneSystemConstants.h"
#include "Population/Person/Person.h"
#include "Simulation/Model.h"

InfantImmuneComponent::InfantImmuneComponent(ImmuneSystem* immune_system)
    : ImmuneComponent(immune_system) {}

InfantImmuneComponent::~InfantImmuneComponent() = default;

double InfantImmuneComponent::get_acquire_rate(core::Age age) const { return 0; }

double InfantImmuneComponent::get_decay_rate(core::Age age) const {
  return immune::K_INFANT_IMMUNE_DECAY_RATE;
}

double InfantImmuneComponent::get_one_day_decay_factor(core::Age) const {
  return immune::K_ONE_DAY_INFANT_DECAY_FACTOR;
}

double InfantImmuneComponent::get_one_day_acquire_factor(core::Age) const { return 1.0; }

double InfantImmuneComponent::get_current_value() {
  const auto* immune_system_ptr = immune_system();
  if (immune_system_ptr == nullptr) { return 0.0; }

  const auto* person = immune_system_ptr->person();
  if (person == nullptr) { return 0.0; }

  const auto current_time = Model::get_scheduler()->current_time();
  const auto duration = current_time - person->get_latest_update_time();
  if (duration == 0.0) { return latest_value(); }
  if (duration == 1) { return latest_value() * immune::K_ONE_DAY_INFANT_DECAY_FACTOR; }
  // decrease I(t) = I0 * e ^ (-b2*t);
  return latest_value() * std::exp(-immune::K_INFANT_IMMUNE_DECAY_RATE * duration);
}
