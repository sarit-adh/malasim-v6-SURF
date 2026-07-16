/*
 * TestTreatmentFailureEvent.cpp
 *
 * Defines the event that tests to see if the treatment given to a patient
 * failed or not.
 */
#ifndef TESTTREATMENTFAILUREEVENT_H
#define TESTTREATMENTFAILUREEVENT_H

// #include "Core/ObjectPool.h"
// #include "Core/PropertyMacro.h"
#include <cstddef>

#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class TestTreatmentFailureEvent : public PersonEvent {
  //  OBJECTPOOL(TestTreatmentFailureEvent)
public:
  // disallow copy, assign and move
  TestTreatmentFailureEvent(const TestTreatmentFailureEvent &) = delete;
  TestTreatmentFailureEvent &operator=(const TestTreatmentFailureEvent &) = delete;
  TestTreatmentFailureEvent(TestTreatmentFailureEvent &&) = delete;
  TestTreatmentFailureEvent &operator=(TestTreatmentFailureEvent &&) = delete;

  explicit TestTreatmentFailureEvent(Person* person) : PersonEvent(person) {}
  ~TestTreatmentFailureEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"TestTreatmentFailureEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  ClonalParasitePopulation* clinical_caused_parasite() { return clinical_caused_parasite_; }
  void set_clinical_caused_parasite(ClonalParasitePopulation* value) {
    clinical_caused_parasite_ = value;
  }
  [[nodiscard]] int therapy_id() const { return therapy_id_; }
  void set_therapy_id(int value) { therapy_id_ = value; }

private:
  int therapy_id_{0};
  ClonalParasitePopulation* clinical_caused_parasite_{nullptr};
  void do_execute() override;
};

#endif
