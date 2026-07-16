#ifndef DISTRICTIMPORTATIONDAILYEVENT_H
#define DISTRICTIMPORTATIONDAILYEVENT_H

#include <tuple>
#include <vector>

#include "Events/Event.h"

class Scheduler;
class DistrictImportationDailyEvent : public WorldEvent {
private:
  int district_;
  double daily_rate_;
  std::vector<std::tuple<int, int, char>> alleles_;

public:
  explicit DistrictImportationDailyEvent(
      int district = -1,
      double daily_rate = -1,
      int start_day = -1,
      const std::vector<std::tuple<int, int, char>> &alleles = {});

  DistrictImportationDailyEvent(const DistrictImportationDailyEvent &) = delete;
  DistrictImportationDailyEvent(DistrictImportationDailyEvent &&) = delete;
  DistrictImportationDailyEvent &operator=(const DistrictImportationDailyEvent &) = delete;
  DistrictImportationDailyEvent &operator=(DistrictImportationDailyEvent &&) = delete;
  ~DistrictImportationDailyEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"district_importation_daily"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // DISTRICTIMPORTATIONDAILYEVENT_H
