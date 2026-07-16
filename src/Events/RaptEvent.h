#pragma once

#include "Event.h"

class Person;

class RaptEvent : public PersonEvent {
public:
  RaptEvent(const RaptEvent &) = delete;
  RaptEvent &operator=(const RaptEvent &) = delete;
  RaptEvent(RaptEvent &&) = delete;
  RaptEvent &operator=(RaptEvent &&) = delete;

  explicit RaptEvent(Person* person) : PersonEvent(person) {}
  ~RaptEvent() override = default;
  static constexpr std::string_view EVENT_NAME{"RAPT Event"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};
