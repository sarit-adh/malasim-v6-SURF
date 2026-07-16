#ifndef CHANGESTRATEGYEVENT_H
#define CHANGESTRATEGYEVENT_H

#include "Events/Event.h"

class ChangeTreatmentStrategyEvent : public WorldEvent {
public:
  // Disallow copy
  ChangeTreatmentStrategyEvent(const ChangeTreatmentStrategyEvent &) = delete;
  ChangeTreatmentStrategyEvent &operator=(const ChangeTreatmentStrategyEvent &) = delete;

  // Disallow move
  ChangeTreatmentStrategyEvent(ChangeTreatmentStrategyEvent &&) = delete;
  ChangeTreatmentStrategyEvent &operator=(ChangeTreatmentStrategyEvent &&) = delete;

  explicit ChangeTreatmentStrategyEvent(const int &strategy_id = 0, const int &at_time = -1);
  ~ChangeTreatmentStrategyEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_treatment_strategy"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  int strategy_id_;
  void do_execute() override;
};

#endif  // CHANGESTRATEGYEVENT_H
