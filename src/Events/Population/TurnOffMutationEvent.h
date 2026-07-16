#ifndef TURNOFFMUTATIONEVENT_H
#define TURNOFFMUTATIONEVENT_H

#include "Events/Event.h"

class TurnOffMutationEvent : public WorldEvent {
public:
  // Disallow copy
  TurnOffMutationEvent(const TurnOffMutationEvent &) = delete;
  TurnOffMutationEvent &operator=(const TurnOffMutationEvent &) = delete;

  // Disallow move
  TurnOffMutationEvent(TurnOffMutationEvent &&) = delete;
  TurnOffMutationEvent &operator=(TurnOffMutationEvent &&) = delete;

  explicit TurnOffMutationEvent(const int &at_time = -1);
  ~TurnOffMutationEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"turn_off_mutation"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // TURNOFFMUTATIONEVENT_H
