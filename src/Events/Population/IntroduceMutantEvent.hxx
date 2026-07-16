/**
 * IntroduceMutantEvent.hxx
 *
 * Introduce mutant genotypes into the simulation by randomly selecting infected
 * individuals and switching the relevant genotype from the wild type to the
 * specified mutant based upon the district id supplied.
 */
#ifndef INTRODUCEMUTANTEVENT_HXX
#define INTRODUCEMUTANTEVENT_HXX

#include <string>

// #include "Core/ObjectPool.h"
#include "Core/Scheduler/Scheduler.h"
#include "IntroduceMutantEventBase.h"
#include "Simulation/Model.h"
#include "Spatial/GIS/SpatialData.h"

class IntroduceMutantEvent : public IntroduceMutantEventBase {
public:
  IntroduceMutantEvent &operator=(const IntroduceMutantEvent &) = delete;
  IntroduceMutantEvent &operator=(IntroduceMutantEvent &&) = delete;
  // disallow copy and move
  IntroduceMutantEvent(const IntroduceMutantEvent &) = delete;
  IntroduceMutantEvent(IntroduceMutantEvent &&) = delete;

private:
  int admin_level_id_;
  int unit_id_;

  void do_execute() override {
    // Calculate the target fraction of the district infections and perform them
    // as needed
    auto locations = Model::get_spatial_data()->get_locations_in_unit(admin_level_id_, unit_id_);
    double target_fraction = calculate(locations);
    auto count = (target_fraction > 0) ? mutate(locations, target_fraction) : 0;

    // Log the event's operation
    spdlog::info(
        "Introduce mutant event: {} : Introduce mutant event, target fraction: "
        "{}, mutations: {}",
        Model::get_scheduler()->get_current_date_string(), target_fraction, count);
  }

public:
  inline static const std::string EVENT_NAME = "introduce_mutant_event";

  explicit IntroduceMutantEvent(const int &time,
                                const int &unit_id,
                                const int &admin_level_id,
                                const double &fraction,
                                const std::vector<std::tuple<int, int, char>> &alleles)
      : IntroduceMutantEventBase(fraction, alleles),
        unit_id_(unit_id),
        admin_level_id_(admin_level_id) {
    this->set_time(time);
  }

  ~IntroduceMutantEvent() override = default;

  // Return the name of this event
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
};

#endif
