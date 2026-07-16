#ifndef UPDATEWHENDRUGISPRESENTEVENT_H
#define UPDATEWHENDRUGISPRESENTEVENT_H

#include "Event.h"
// #include "Core/ObjectPool.h"
// #include "Core/PropertyMacro.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class UpdateWhenDrugIsPresentEvent : public PersonEvent {
  // OBJECTPOOL(UpdateWhenDrugIsPresentEvent)
public:
  // Disallow copy
  UpdateWhenDrugIsPresentEvent(const UpdateWhenDrugIsPresentEvent &) = delete;
  UpdateWhenDrugIsPresentEvent &operator=(const UpdateWhenDrugIsPresentEvent &) = delete;

  // Disallow move
  UpdateWhenDrugIsPresentEvent(UpdateWhenDrugIsPresentEvent &&) = delete;
  UpdateWhenDrugIsPresentEvent &operator=(UpdateWhenDrugIsPresentEvent &&) = delete;

  explicit UpdateWhenDrugIsPresentEvent(Person* person) : PersonEvent(person) {}

  ~UpdateWhenDrugIsPresentEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"UpdateByHavingDrugEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  ClonalParasitePopulation* clinical_caused_parasite() { return clinical_caused_parasite_; }
  void set_clinical_caused_parasite(ClonalParasitePopulation* value) {
    clinical_caused_parasite_ = value;
  }

private:
  ClonalParasitePopulation* clinical_caused_parasite_{nullptr};

  void do_execute() override;
};

#endif /* UPDATEWHENDRUGISPRESENTEVENT_H */
