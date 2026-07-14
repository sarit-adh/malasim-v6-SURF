#include "PublicPrivateStrategy.h"

#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "Core/Scheduler/Scheduler.h"
#include "Simulation/Model.h"
#include "Treatment/Therapies/Therapy.h"
#include "Utils/Random.h"

PublicPrivateStrategy::PublicPrivateStrategy()
    : IStrategy("PublicPrivateStrategy", StrategyType::PublicPrivate) {}

void PublicPrivateStrategy::set_public_strategy(IStrategy* strategy) {
  if (strategy == nullptr) { throw std::invalid_argument("Public strategy must not be null"); }
  public_strategy_ = strategy;
}

void PublicPrivateStrategy::set_private_strategy(IStrategy* strategy) {
  if (strategy == nullptr) { throw std::invalid_argument("Private strategy must not be null"); }
  private_strategy_ = strategy;
}

void PublicPrivateStrategy::add_therapy(Therapy* therapy) {}

Therapy* PublicPrivateStrategy::get_therapy(Person* person) {
  return select_treatment(person).therapy;
}

TreatmentSelection PublicPrivateStrategy::select_treatment(Person* person) {
  if (public_strategy_ == nullptr || private_strategy_ == nullptr) {
    throw std::runtime_error("Public/private child strategies are not configured");
  }

  const auto probability = Model::get_random()->random_flat(0.0, 1.0);
  const auto sector =
      probability < public_share ? TreatmentSector::Public : TreatmentSector::Private;
  auto* strategy = sector == TreatmentSector::Public ? public_strategy_ : private_strategy_;
  return {strategy->get_therapy(person), sector};
}

std::string PublicPrivateStrategy::to_string() const {
  std::stringstream stream;
  stream << id << "-" << name << "-public:";
  stream << (public_strategy_ == nullptr ? -1 : public_strategy_->id) << "-private:";
  stream << (private_strategy_ == nullptr ? -1 : private_strategy_->id);
  return stream.str();
}

void PublicPrivateStrategy::adjust_started_time_point(const int &current_time) {
  starting_time = current_time;
  public_share = start_public_share;
  if (public_strategy_ != nullptr) { public_strategy_->adjust_started_time_point(current_time); }
  if (private_strategy_ != nullptr) { private_strategy_->adjust_started_time_point(current_time); }
}

void PublicPrivateStrategy::update_end_of_time_step() {
  if (public_strategy_ != nullptr) { public_strategy_->update_end_of_time_step(); }
  if (private_strategy_ != nullptr) { private_strategy_->update_end_of_time_step(); }
}

void PublicPrivateStrategy::monthly_update() {
  adjust_public_share(Model::get_scheduler()->current_time());
  if (public_strategy_ != nullptr) { public_strategy_->monthly_update(); }
  if (private_strategy_ != nullptr) { private_strategy_->monthly_update(); }
}

void PublicPrivateStrategy::adjust_public_share(const int time) {
  if (peak_after == 0 || time >= starting_time + peak_after) {
    public_share = peak_public_share;
    return;
  }

  const auto elapsed = std::clamp(time - starting_time, 0, peak_after);
  const auto progress = static_cast<double>(elapsed) / peak_after;
  public_share = start_public_share + ((peak_public_share - start_public_share) * progress);
}
