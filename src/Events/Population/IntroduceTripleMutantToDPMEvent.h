#ifndef POMS_SRC_EVENTS_POPULATION_INTRODUCETRIPLEMUTANTTODPMEVENT_H
#define POMS_SRC_EVENTS_POPULATION_INTRODUCETRIPLEMUTANTTODPMEVENT_H

#include <tuple>
#include <vector>

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

class IntroduceTripleMutantToDPMEvent : public WorldEvent {
public:
  // disallow copy, assign and move
  IntroduceTripleMutantToDPMEvent(const IntroduceTripleMutantToDPMEvent &) = delete;
  void operator=(const IntroduceTripleMutantToDPMEvent &) = delete;
  IntroduceTripleMutantToDPMEvent(IntroduceTripleMutantToDPMEvent &&) = delete;
  void operator=(IntroduceTripleMutantToDPMEvent &&) = delete;

  // OBJECTPOOL(IntroduceTripleMutantToDPMEvent)

private:
  int location_;
  double fraction_;
  std::vector<std::tuple<int, int, char>> alleles_;

public:
  explicit IntroduceTripleMutantToDPMEvent(
      const int &location = -1,
      const int &execute_at = -1,
      const double &fraction = 0,
      const std::vector<std::tuple<int, int, char>> &alleles = {});

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceTripleMutantToDPMEvent() override;

  static constexpr std::string_view EVENT_NAME{"introduce_triple_mutant_to_dpm"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // POMS_SRC_EVENTS_POPULATION_INTRODUCETRIPLEMUTANTTODPMEVENT_H
