#include "NonInfantImmuneComponent.h"

#include "Configuration/Config.h"
#include "Simulation/Model.h"

// OBJECTPOOL_IMPL(NonInfantImmuneComponent)

NonInfantImmuneComponent::NonInfantImmuneComponent(ImmuneSystem* immune_system)
    : ImmuneComponent(immune_system) {}

NonInfantImmuneComponent::~NonInfantImmuneComponent() = default;

double NonInfantImmuneComponent::get_acquire_rate(const int &age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.acquire_rate_by_age[(age > 80) ? 80 : age];
}

double NonInfantImmuneComponent::get_decay_rate(const int &age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.decay_rate;
}
