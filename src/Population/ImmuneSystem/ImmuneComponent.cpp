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

namespace {

std::size_t age_index(core::Age age) {
  return static_cast<std::size_t>(
      age > immune::K_MAX_IMMUNE_AGE_INDEX ? immune::K_MAX_IMMUNE_AGE_INDEX : age);
}
}  // namespace

ImmuneComponent::ImmuneComponent(ImmuneSystem* immune_system, ImmuneComponentType type)
    : immune_system_(immune_system), type_(type) {}

double ImmuneComponent::get_current_value() const {
  // Early exit if no immune system or person - avoid all pointer dereferences
  if (immune_system_ == nullptr) { return 0.0; }

  auto* person = immune_system_->person();
  if (person == nullptr) { return 0.0; }

  const auto current_time = Model::get_scheduler()->current_time();
  const auto duration = current_time - person->get_latest_update_time();

  // Early exit if no time has passed - avoid expensive exp() calculation
  if (duration == 0.0) { return latest_value_; }

  if (type_ == ImmuneComponentType::Infant) {
    const auto factor = duration == 1 ? immune::K_ONE_DAY_INFANT_DECAY_FACTOR
                                      : std::exp(-immune::K_INFANT_IMMUNE_DECAY_RATE * duration);
    return latest_value_ * factor;
  }

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
  if (type_ == ImmuneComponentType::Infant) { return immune::K_ONE_DAY_INFANT_DECAY_FACTOR; }
  const auto &parameters = Model::get_config()->get_immune_system_parameters();
  return parameters.decay_rate_one_day_factor;
}

double ImmuneComponent::get_one_day_acquire_factor(core::Age age) const {
  if (type_ == ImmuneComponentType::Infant) { return 1.0; }
  const auto &parameters = Model::get_config()->get_immune_system_parameters();
  return parameters.acquire_rate_by_age_one_day_factor[age_index(age)];
}

double ImmuneComponent::get_decay_rate(core::Age) const {
  if (type_ == ImmuneComponentType::Infant) { return immune::K_INFANT_IMMUNE_DECAY_RATE; }
  return Model::get_config()->get_immune_system_parameters().decay_rate;
}

double ImmuneComponent::get_acquire_rate(core::Age age) const {
  if (type_ == ImmuneComponentType::Infant) { return 0.0; }
  const auto &parameters = Model::get_config()->get_immune_system_parameters();
  return parameters.acquire_rate_by_age[age_index(age)];
}

void ImmuneComponent::draw_random_immune() {
  const auto &ims = Model::get_config()->get_immune_system_parameters();
  latest_value_ = Model::get_random()->random_beta(ims.alpha_immune, ims.beta_immune);
}
