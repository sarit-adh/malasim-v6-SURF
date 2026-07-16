#ifndef RETURNTORESIDENCEEVENT_H
#define RETURNTORESIDENCEEVENT_H

// #include "Core/ObjectPool.h"
#include "Event.h"

class Person;
class Scheduler;

class ReturnToResidenceEvent : public PersonEvent {
  //  OBJECTPOOL(ReturnToResidenceEvent)
public:
  ReturnToResidenceEvent &operator=(const ReturnToResidenceEvent &) = delete;
  ReturnToResidenceEvent &operator=(ReturnToResidenceEvent &&) = delete;
  // disallow copy and move
  ReturnToResidenceEvent(const ReturnToResidenceEvent &) = delete;
  ReturnToResidenceEvent(ReturnToResidenceEvent &&) = delete;

  explicit ReturnToResidenceEvent(Person* person) : PersonEvent(person) {}
  ~ReturnToResidenceEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"ReturnToResidenceEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif
