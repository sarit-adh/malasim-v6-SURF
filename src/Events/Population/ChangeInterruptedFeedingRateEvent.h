#ifndef POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H
#define POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H

#include "Events/Event.h"

class ChangeInterruptedFeedingRateEvent : public WorldEvent {
public:
  // Disallow copy
  ChangeInterruptedFeedingRateEvent(const ChangeInterruptedFeedingRateEvent &) = delete;
  ChangeInterruptedFeedingRateEvent &operator=(const ChangeInterruptedFeedingRateEvent &) = delete;

  // Disallow move
  ChangeInterruptedFeedingRateEvent(ChangeInterruptedFeedingRateEvent &&) = delete;
  ChangeInterruptedFeedingRateEvent &operator=(ChangeInterruptedFeedingRateEvent &&) = delete;

  explicit ChangeInterruptedFeedingRateEvent(const int &location = -1,
                                             const double &ifr = 0.0,
                                             const int &at_time = -1);
  ~ChangeInterruptedFeedingRateEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_interrupted_feeding_rate"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  int location{-1};
  double ifr{0.0};

private:
  void do_execute() override;
};

#endif  // POMS_CHANGEINTERRUPTEDFEEDINGRATEEVENT_H
