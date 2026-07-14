#include "PublicPrivateMultiLocationStrategy.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "Core/Scheduler/Scheduler.h"
#include "Population/Person/Person.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Random.h"

PublicPrivateMultiLocationStrategy::PublicPrivateMultiLocationStrategy()
    : IStrategy("PublicPrivateMultiLocationStrategy", StrategyType::PublicPrivateMultiLocation) {}

void PublicPrivateMultiLocationStrategy::set_public_strategy(IStrategy* strategy) {
  if (strategy == nullptr) { throw std::invalid_argument("Public strategy must not be null"); }
  public_strategy_ = strategy;
}

void PublicPrivateMultiLocationStrategy::set_private_strategy(IStrategy* strategy) {
  if (strategy == nullptr) { throw std::invalid_argument("Private strategy must not be null"); }
  private_strategy_ = strategy;
}

void PublicPrivateMultiLocationStrategy::add_therapy(Therapy* therapy) {}

Therapy* PublicPrivateMultiLocationStrategy::get_therapy(Person* person) {
  return select_treatment(person).therapy;
}

TreatmentSelection PublicPrivateMultiLocationStrategy::select_treatment(Person* person) {
  if (person == nullptr) { throw std::invalid_argument("Person must not be null"); }
  if (public_strategy_ == nullptr || private_strategy_ == nullptr) {
    throw std::runtime_error("Public/private child strategies are not configured");
  }

  const auto location = person->get_location();
  if (location < 0 || static_cast<std::size_t>(location) >= public_share_by_location.size()) {
    throw std::out_of_range("Person location is outside public/private share range");
  }

  const auto probability = Model::get_random()->random_flat(0.0, 1.0);
  const auto sector = probability < public_share_by_location[location] ? TreatmentSector::Public
                                                                       : TreatmentSector::Private;
  auto* strategy = sector == TreatmentSector::Public ? public_strategy_ : private_strategy_;
  return {strategy->get_therapy(person), sector};
}

std::string PublicPrivateMultiLocationStrategy::to_string() const {
  std::stringstream stream;
  stream << id << "-" << name << "-public:";
  stream << (public_strategy_ == nullptr ? -1 : public_strategy_->id) << "-private:";
  stream << (private_strategy_ == nullptr ? -1 : private_strategy_->id);
  return stream.str();
}

void PublicPrivateMultiLocationStrategy::adjust_started_time_point(const int &current_time) {
  starting_time = current_time;
  public_share_by_location = start_public_share_by_location;
  if (public_strategy_ != nullptr) { public_strategy_->adjust_started_time_point(current_time); }
  if (private_strategy_ != nullptr) { private_strategy_->adjust_started_time_point(current_time); }
}

void PublicPrivateMultiLocationStrategy::update_end_of_time_step() {
  if (public_strategy_ != nullptr) { public_strategy_->update_end_of_time_step(); }
  if (private_strategy_ != nullptr) { private_strategy_->update_end_of_time_step(); }
}

void PublicPrivateMultiLocationStrategy::monthly_update() {
  adjust_public_shares(Model::get_scheduler()->current_time());
  if (public_strategy_ != nullptr) { public_strategy_->monthly_update(); }
  if (private_strategy_ != nullptr) { private_strategy_->monthly_update(); }
}

void PublicPrivateMultiLocationStrategy::adjust_public_shares(const int time) {
  if (peak_after == 0 || time >= starting_time + peak_after) {
    public_share_by_location = peak_public_share_by_location;
    return;
  }

  const auto elapsed = std::clamp(time - starting_time, 0, peak_after);
  const auto progress = static_cast<double>(elapsed) / peak_after;
  for (std::size_t location = 0; location < public_share_by_location.size(); ++location) {
    public_share_by_location[location] =
        start_public_share_by_location[location]
        + ((peak_public_share_by_location[location] - start_public_share_by_location[location])
           * progress);
  }
}
