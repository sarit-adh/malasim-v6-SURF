#ifndef SWITCH_IMMUNE_SYSTEM_MODE_EVENT_H
#define SWITCH_IMMUNE_SYSTEM_MODE_EVENT_H

#include "Event.h"

class Person;

class SwitchImmuneSystemModeEvent : public PersonEvent {
public:
  SwitchImmuneSystemModeEvent(const SwitchImmuneSystemModeEvent &) = delete;
  SwitchImmuneSystemModeEvent(SwitchImmuneSystemModeEvent &&) = delete;
  SwitchImmuneSystemModeEvent &operator=(const SwitchImmuneSystemModeEvent &) = delete;
  SwitchImmuneSystemModeEvent &operator=(SwitchImmuneSystemModeEvent &&) = delete;

  explicit SwitchImmuneSystemModeEvent(Person* person);
  ~SwitchImmuneSystemModeEvent() override;

  static constexpr std::string_view EVENT_NAME{"SwitchImmuneSystemModeEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

protected:
  void do_execute() override;
};

#endif  // SWITCH_IMMUNE_SYSTEM_MODE_EVENT_H
