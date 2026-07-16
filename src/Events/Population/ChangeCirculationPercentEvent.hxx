/*
 * ChangeCirculationPercentEvent.hxx
 *
 * Define and implement an event to change the daily percentage of the
 * population circulating.
 */
#ifndef CHANGE_CIRCULATION_PERCENT_EVENT_HXX
#define CHANGE_CIRCULATION_PERCENT_EVENT_HXX

#include "Configuration/Config.h"
#include "Events/Event.h"
#include "Simulation/Model.h"

class ChangeCirculationPercentEvent : public WorldEvent {
public:
  ChangeCirculationPercentEvent(const ChangeCirculationPercentEvent &) = delete;
  ChangeCirculationPercentEvent(ChangeCirculationPercentEvent &&) = delete;
  ChangeCirculationPercentEvent &operator=(const ChangeCirculationPercentEvent &) = delete;
  ChangeCirculationPercentEvent &operator=(ChangeCirculationPercentEvent &&) = delete;
  ChangeCirculationPercentEvent(float rate, int start) : rate_(rate) { set_time(start); }
  ~ChangeCirculationPercentEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_circulation_percent"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  float rate_ = 0.0;

  // Execute the event to change the circulation percentage
  void do_execute() override {
    MovementSettings::CirculationInfo circulation_info =
        Model::get_config()->get_movement_settings().get_circulation_info();
    circulation_info.set_circulation_percent(rate_);
    Model::get_config()->get_movement_settings().set_circulation_info(circulation_info);

    // Log on demand
    spdlog::debug("Change circulation percent event: {} - {}",
                  Model::get_scheduler()->get_current_date_string(), rate_);
  }
};

#endif
