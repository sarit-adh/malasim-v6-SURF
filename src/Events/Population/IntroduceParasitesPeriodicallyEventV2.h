#ifndef POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H
#define POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H

#include <vector>

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

class Scheduler;

class IntroduceParasitesPeriodicallyEventV2 : public WorldEvent {
public:
  // Disallow copy
  IntroduceParasitesPeriodicallyEventV2(const IntroduceParasitesPeriodicallyEventV2 &) = delete;
  IntroduceParasitesPeriodicallyEventV2 &operator=(const IntroduceParasitesPeriodicallyEventV2 &) =
      delete;

  // Disallow move
  IntroduceParasitesPeriodicallyEventV2(IntroduceParasitesPeriodicallyEventV2 &&) = delete;
  IntroduceParasitesPeriodicallyEventV2 &operator=(IntroduceParasitesPeriodicallyEventV2 &&) =
      delete;

  // OBJECTPOOL(IntroduceParasitesPeriodicallyEventV2)
private:
  int location_;
  int duration_;
  int number_of_cases_;

public:
  [[nodiscard]] int location() const { return location_; }
  void set_location(int location) { location_ = location; }
  [[nodiscard]] int duration() const { return duration_; }
  void set_duration(int duration) { duration_ = duration; }
  [[nodiscard]] int number_of_cases() const { return number_of_cases_; }
  void set_number_of_cases(int number_of_cases) { number_of_cases_ = number_of_cases; }

  std::vector<std::vector<double>> allele_distributions;
  int start_day;
  int end_day;

  explicit IntroduceParasitesPeriodicallyEventV2(
      const std::vector<std::vector<double>> &allele_distributions_in =
          std::vector<std::vector<double>>(),
      const int &location = -1,
      const int &duration = -1,
      const int &number_of_cases = -1,
      const int &start_day_in = -1,
      const int &end_day_in = -1);

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceParasitesPeriodicallyEventV2() override;

  static constexpr std::string_view EVENT_NAME{"introduce_parasites_periodically_v2"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // POMS_SRC_EVENTS_POPULATION_INTRODUCEPARASITESPERIODICALLYEVENTV2_H
