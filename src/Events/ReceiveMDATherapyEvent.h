#ifndef RECEIVEMDADRUGEVENT_H
#define RECEIVEMDADRUGEVENT_H

// #include "Core/PropertyMacro.h"
#include "Event.h"

class Scheduler;

class Person;

class Therapy;

class ReceiveMDATherapyEvent : public PersonEvent {
public:
  ReceiveMDATherapyEvent &operator=(const ReceiveMDATherapyEvent &) = delete;
  ReceiveMDATherapyEvent &operator=(ReceiveMDATherapyEvent &&) = delete;
  // disable copy and move
  ReceiveMDATherapyEvent(const ReceiveMDATherapyEvent &) = delete;
  ReceiveMDATherapyEvent(ReceiveMDATherapyEvent &&) = delete;
  //  DELETE_COPY_AND_MOVE(ReceiveMDATherapyEvent)

  //  POINTER_PROPERTY(Therapy, received_therapy)
  Therapy* received_therapy() { return received_therapy_; }
  void set_received_therapy(Therapy* value) { received_therapy_ = value; }

  explicit ReceiveMDATherapyEvent(Person* person) : PersonEvent(person) {}

  //    ReceiveMDADrugEvent(const ReceiveMDADrugEvent& orig);
  ~ReceiveMDATherapyEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"ReceiveMDADrugEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  Therapy* received_therapy_{nullptr};
  void do_execute() override;
};

#endif /* RECEIVEMDADRUGEVENT_H */
