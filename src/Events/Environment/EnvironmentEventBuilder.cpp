/*
 * EnvironmentEventBuilder.cpp
 *
 * Implement the environmental event builders.
 */
#include "EnvironmentEventBuilder.h"

#include <vector>

#include "Configuration/Config.h"
#include "UpdateEcozoneEvent.hxx"
#include "Utils/Helpers/StringHelpers.h"

std::vector<std::unique_ptr<WorldEvent>> EnvironmentEventBuilder::build(const YAML::Node &node) {
  std::vector<std::unique_ptr<WorldEvent>> events;
  const auto name = node["name"].as<std::string>();

  if (name == UpdateEcozoneEvent::EVENT_NAME) {
    events = build_update_ecozone_event(node["info"], Model::get_config());
  }

  return events;
}

std::vector<std::unique_ptr<WorldEvent>> EnvironmentEventBuilder::build_update_ecozone_event(
    const YAML::Node &node, Config* config) {
  try {
    std::vector<std::unique_ptr<WorldEvent>> events;
    for (const auto &ndx : node) {
      // Load the values
      auto start_date = ndx["day"].as<date::year_month_day>();
      auto time = (date::sys_days{start_date}
                   - date::sys_days{config->get_simulation_timeframe().get_starting_date()})
                      .count();
      auto from = ndx["from"].as<int>();
      auto to = ndx["to"].as<int>();

      // Verify inputs
      if (from < 0) {
        spdlog::error(
            "Invalid from value supplied from for {} "
            "value supplied cannot be less than zero",
            UpdateEcozoneEvent::EVENT_NAME);
        throw std::invalid_argument("From value cannot be less than zero");
      }
      if (to < 0) {
        spdlog::error(
            "Invalid from value supplied to for {} "
            "value supplied cannot be less than zero",
            UpdateEcozoneEvent::EVENT_NAME);
        throw std::invalid_argument("To value cannot be less than zero");
      }

      // Log and add the event to the queue
      auto event = std::make_unique<UpdateEcozoneEvent>(from, to, time);
      spdlog::debug("Adding event {} start date: {} from: {} to: {}", event->name(),
                    StringHelpers::date_as_string(start_date), from, to);
      events.push_back(std::move(event));
    }
    return events;

  } catch (YAML::BadConversion &error) {
    spdlog::error(
        "Unrecoverable error parsing YAML value in "
        "UpdateEcozoneEvent node: {}",
        error.msg);
    exit(1);
  }
}
