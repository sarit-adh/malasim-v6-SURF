#include "NonInfantImmuneComponent.h"

#include "Configuration/Config.h"
#include "Core/types.h"
#include "Population/ImmuneSystem/ImmuneSystemConstants.h"
#include "Simulation/Model.h"

// OBJECTPOOL_IMPL(NonInfantImmuneComponent)
namespace {

std::size_t age_index(core::Age age) {
  return static_cast<std::size_t>(
      age > immune::K_MAX_IMMUNE_AGE_INDEX ? immune::K_MAX_IMMUNE_AGE_INDEX : age);
}
}  // namespace

NonInfantImmuneComponent::NonInfantImmuneComponent(ImmuneSystem* immune_system)
    : ImmuneComponent(immune_system) {}

NonInfantImmuneComponent::~NonInfantImmuneComponent() = default;

double NonInfantImmuneComponent::get_acquire_rate(core::Age age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.acquire_rate_by_age[age_index(age)];
}

double NonInfantImmuneComponent::get_decay_rate(core::Age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.decay_rate;
}

double NonInfantImmuneComponent::get_one_day_acquire_factor(core::Age age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.acquire_rate_by_age_one_day_factor[age_index(age)];
}

double NonInfantImmuneComponent::get_one_day_decay_factor(core::Age) const {
  const auto &immune_parameters = Model::get_config()->get_immune_system_parameters();
  return immune_parameters.decay_rate_one_day_factor;
}
