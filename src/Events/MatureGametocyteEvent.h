#ifndef MATUREGAMETOCYTEEVENT_H
#define MATUREGAMETOCYTEEVENT_H

// #include "Core/ObjectPool.h"
// #include "Core/PropertyMacro.h"
#include "Event.h"

class ClonalParasitePopulation;

class Scheduler;

class Person;

class MatureGametocyteEvent : public PersonEvent {
  // OBJECTPOOL(MatureGametocyteEvent)
public:
  // disallow copy, assign and move
  MatureGametocyteEvent(const MatureGametocyteEvent &) = delete;
  MatureGametocyteEvent &operator=(const MatureGametocyteEvent &) = delete;
  MatureGametocyteEvent(MatureGametocyteEvent &&) = delete;
  MatureGametocyteEvent &operator=(MatureGametocyteEvent &&) = delete;
  explicit MatureGametocyteEvent(Person* person) : PersonEvent(person) {}

  ClonalParasitePopulation* blood_parasite() { return blood_parasite_; }
  void set_blood_parasite(ClonalParasitePopulation* value) { blood_parasite_ = value; }

  ~MatureGametocyteEvent() override = default;
  static constexpr std::string_view EVENT_NAME{"MatureGametocyteEvent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  ClonalParasitePopulation* blood_parasite_{nullptr};

  void do_execute() override;
};

#endif /* MATUREGAMETOCYTEEVENT_H */
