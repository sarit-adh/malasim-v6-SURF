/*
 * RotateStrategyEvent.h
 *
 * Define the event class to rotate treatment strategies on a regular basis.
 */
#ifndef ROTATE_STRATEGY_EVENT_H
#define ROTATE_STRATEGY_EVENT_H

// #include "Core/ObjectPool.h"
// #include "Core/PropertyMacro.h"
#include "Events/Event.h"

class RotateStrategyEvent : public WorldEvent {
public:
  RotateStrategyEvent(const RotateStrategyEvent &) = delete;
  RotateStrategyEvent(RotateStrategyEvent &&) = delete;
  RotateStrategyEvent &operator=(const RotateStrategyEvent &) = delete;
  RotateStrategyEvent &operator=(RotateStrategyEvent &&) = delete;
  RotateStrategyEvent(int at_time, int years, int current_strategy_id, int next_strategy_id);
  ~RotateStrategyEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"rotate_treatment_strategy"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  int new_strategy_id_;
  int next_strategy_id_;
  int years_;

  void do_execute() override;
};

#endif
