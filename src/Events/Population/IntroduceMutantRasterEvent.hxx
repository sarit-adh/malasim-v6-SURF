/**
 * IntroduceMutantRasterEvent.hxx
 *
 * Introduce mutant genotypes into the simulation by randomly selecting infected
 * individuals and switching the relevant genotype from the wild type to the
 * specified mutant based upon the raster supplied.
 */
#ifndef INTRODUCEMUTANTRASTEREVENT_HXX
#define INTRODUCEMUTANTRASTEREVENT_HXX

#include <utility>

#include "Core/Scheduler/Scheduler.h"
#include "IntroduceMutantEventBase.h"
#include "Simulation/Model.h"

class IntroduceMutantRasterEvent : public IntroduceMutantEventBase {
public:
  IntroduceMutantRasterEvent &operator=(const IntroduceMutantRasterEvent &) = delete;
  IntroduceMutantRasterEvent &operator=(IntroduceMutantRasterEvent &&) = delete;
  // disallow copy and move
  IntroduceMutantRasterEvent(const IntroduceMutantRasterEvent &) = delete;
  IntroduceMutantRasterEvent(IntroduceMutantRasterEvent &&) = delete;

  IntroduceMutantRasterEvent(const int &time,
                             std::vector<int> locations,
                             const double &fraction,
                             const std::vector<std::tuple<int, int, char>> &alleles)
      : IntroduceMutantEventBase(fraction, alleles), locations_(std::move(locations)) {
    this->set_time(time);
  }

  ~IntroduceMutantRasterEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"introduce_mutant_raster"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  std::vector<int> locations_;

  void do_execute() override {
    // Use the locations to calculate the target fraction of mutations and
    // perform them
    auto target_fraction = calculate(locations_);
    auto count = (target_fraction > 0) ? mutate(locations_, target_fraction) : 0;

    // Log the event's operation
    for (auto allele : alleles) {
      spdlog::info(
          "Time: {} - Introduce mutant raster event chromosome {} locus {} "
          "allele {} fraction: {} count: {}",
          Model::get_scheduler()->current_time(), std::get<0>(allele), std::get<1>(allele),
          std::get<2>(allele), target_fraction, count);
    }
  }
};

#endif
