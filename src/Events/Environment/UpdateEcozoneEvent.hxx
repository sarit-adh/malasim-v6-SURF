/*
 * UpdateEcozoneEvent.hxx
 *
 * Update the ecozone from the previous type to the new type.
 */
#ifndef UPDATEECOZONEEVENT_H
#define UPDATEECOZONEEVENT_H

#include <spdlog/spdlog.h>

#include "Configuration/Config.h"
#include "Configuration/SeasonalitySettings.h"
#include "Events/Event.h"
#include "Simulation/Model.h"

class UpdateEcozoneEvent : public WorldEvent {
private:
  int from_;
  int to_;

  // Execute the import event
  void do_execute() override {
    spdlog::info("Updating ecozone from {} to {}", from_, to_);

    auto* seasons = Model::get_config()->get_seasonality_settings().get_seasonal_equation();
    seasons->update_seasonality(from_, to_);
  }

public:
  UpdateEcozoneEvent(const UpdateEcozoneEvent &) = delete;
  UpdateEcozoneEvent(UpdateEcozoneEvent &&) = delete;
  UpdateEcozoneEvent &operator=(const UpdateEcozoneEvent &) = delete;
  UpdateEcozoneEvent &operator=(UpdateEcozoneEvent &&) = delete;
  UpdateEcozoneEvent(int from, int to, int start) : from_(from), to_(to) { set_time(start); }

  ~UpdateEcozoneEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"update_ecozone"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
};

#endif
