#include "ImmuneComponent.h"

#include <cmath>

#include "Configuration/Config.h"
#include "Core/Scheduler/Scheduler.h"
#include "ImmuneSystem.h"
#include "Population/ImmuneSystem/ImmuneSystemConstants.h"
#include "Population/Person/Person.h"
#include "Population/Population.h"
#include "Simulation/Model.h"
#include "Utils/Random.h"

ImmuneComponent::ImmuneComponent(ImmuneSystem* immune_system) : immune_system_(immune_system) {}

ImmuneComponent::~ImmuneComponent() = default;

double ImmuneComponent::get_current_value() {
  // Early exit if no immune system or person - avoid all pointer dereferences
  if (immune_system_ == nullptr) { return 0.0; }

  auto* person = immune_system_->person();
  if (person == nullptr) { return 0.0; }

  const auto current_time = Model::get_scheduler()->current_time();
  const auto duration = current_time - person->get_latest_update_time();

  // Early exit if no time has passed - avoid expensive exp() calculation
  if (duration == 0.0) { return latest_value_; }

  const auto age = person->get_age();

  if (immune_system_->increase()) {
    // increase I(t) = 1 - (1-I0)e^(-b1*t)
    const auto factor = duration == 1 ? get_one_day_acquire_factor(age)
                                      : std::exp(-get_acquire_rate(age) * duration);
    return 1 - ((1 - latest_value_) * factor);
  }

  // decrease I(t) = I0 * e ^ (-b2*t);
  const auto factor =
      duration == 1 ? get_one_day_decay_factor(age) : std::exp(-get_decay_rate(age) * duration);
  const auto value = latest_value_ * factor;
  return (value < immune::K_IMMUNE_VALUE_CUTOFF) ? 0.0 : value;
}

void ImmuneComponent::update() { latest_value_ = get_current_value(); }

double ImmuneComponent::get_one_day_decay_factor(core::Age age) const {
  return std::exp(-get_decay_rate(age));
}

double ImmuneComponent::get_one_day_acquire_factor(core::Age age) const {
  return std::exp(-get_acquire_rate(age));
}

void ImmuneComponent::draw_random_immune() {
  const auto &ims = Model::get_config()->get_immune_system_parameters();
  latest_value_ = Model::get_random()->random_beta(ims.alpha_immune, ims.beta_immune);
}
