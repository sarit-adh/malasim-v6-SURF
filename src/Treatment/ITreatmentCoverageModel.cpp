#include "ITreatmentCoverageModel.h"

#include <yaml-cpp/yaml.h>

#include "Configuration/Config.h"
#include "Core/types.h"
#include "InflatedTCM.h"
#include "LinearTCM.h"
#include "SteadyTCM.h"
#include "spdlog/spdlog.h"

double ITreatmentCoverageModel::get_probability_to_be_treated(core::LocationId location,
                                                              core::Age age) {
  if (location < 0 || location >= p_treatment_under_5.size()
      || location >= p_treatment_over_5.size()) {
    spdlog::error("wrong location value: {}", location);
    throw std::invalid_argument("wrong location value: " + std::to_string(location));
  }
  return age <= 5 ? p_treatment_under_5[location] : p_treatment_over_5[location];
}

std::unique_ptr<ITreatmentCoverageModel> ITreatmentCoverageModel::build_steady_tcm(
    const YAML::Node &node, Config* config) {
  auto result = std::make_unique<SteadyTCM>();

  const auto starting_date = node["date"].as<date::year_month_day>();
  result->starting_time = (date::sys_days{starting_date}
                           - date::sys_days{config->get_simulation_timeframe().get_starting_date()})
                              .count();

  read_p_treatment(node["p_treatment_under_5_by_location"], result->p_treatment_under_5,
                   config->number_of_locations());
  read_p_treatment(node["p_treatment_over_5_by_location"], result->p_treatment_over_5,
                   config->number_of_locations());
  result->type = node["type"].as<std::string>();

  return result;
}

void ITreatmentCoverageModel::read_p_treatment(const YAML::Node &node,
                                               std::vector<double> &p_treatments,
                                               const int number_of_locations) {
  if (node) {
    for (std::size_t loc = 0; loc < number_of_locations; loc++) {
      auto input_loc = node.size() < number_of_locations ? 0 : loc;
      p_treatments.push_back(node[input_loc].as<float>());
    }
  }
}

std::unique_ptr<ITreatmentCoverageModel> ITreatmentCoverageModel::build_inflated_tcm(
    const YAML::Node &node, Config* config) {
  auto result = std::make_unique<InflatedTCM>();
  const auto starting_date = node["date"].as<date::year_month_day>();
  result->starting_time = (date::sys_days{starting_date}
                           - date::sys_days{config->get_simulation_timeframe().get_starting_date()})
                              .count();

  const auto annual_inflation_rate = node["annual_inflation_rate"].as<double>();
  result->monthly_inflation_rate = annual_inflation_rate / 12;

  read_p_treatment(node["p_treatment_under_5_by_location"], result->p_treatment_under_5,
                   config->number_of_locations());
  read_p_treatment(node["p_treatment_over_5_by_location"], result->p_treatment_over_5,
                   config->number_of_locations());
  result->type = node["type"].as<std::string>();

  return result;
}

std::unique_ptr<ITreatmentCoverageModel> ITreatmentCoverageModel::build_linear_tcm(
    const YAML::Node &node, Config* config) {
  auto result = std::make_unique<LinearTCM>();
  if (!result) { throw std::runtime_error("LinearTCM is nullptr"); }

  const auto starting_date = node["from_date"].as<date::year_month_day>();
  const auto to_date = node["to_date"].as<date::year_month_day>();
  result->starting_time = (date::sys_days{starting_date}
                           - date::sys_days{config->get_simulation_timeframe().get_starting_date()})
                              .count();
  result->end_time = (date::sys_days{to_date}
                      - date::sys_days{config->get_simulation_timeframe().get_starting_date()})
                         .count();

  read_p_treatment(node["p_treatment_under_5_by_location_from"], result->p_treatment_under_5,
                   config->number_of_locations());
  read_p_treatment(node["p_treatment_under_5_by_location_to"], result->p_treatment_under_5_to,
                   config->number_of_locations());

  read_p_treatment(node["p_treatment_over_5_by_location_from"], result->p_treatment_over_5,
                   config->number_of_locations());
  read_p_treatment(node["p_treatment_over_5_by_location_to"], result->p_treatment_over_5_to,
                   config->number_of_locations());
  result->type = node["type"].as<std::string>();

  if (!result) { throw std::runtime_error("LinearTCM is nullptr"); }

  spdlog::info("Treatment coverage model: {}", result->type);
  spdlog::info("Start time: {}", result->starting_time);
  spdlog::info("End time: {}", result->end_time);
  spdlog::info("p_treatment_under_5_by_location_from: {}", result->p_treatment_under_5.front());
  spdlog::info("p_treatment_under_5_by_location_to: {}", result->p_treatment_under_5_to.front());
  spdlog::info("p_treatment_over_5_by_location_from: {}", result->p_treatment_over_5.front());
  spdlog::info("p_treatment_over_5_by_location_to: {}", result->p_treatment_over_5_to.front());

  if (!result) { throw std::runtime_error("LinearTCM is nullptr"); }

  return result;
}

std::unique_ptr<ITreatmentCoverageModel> ITreatmentCoverageModel::build(const YAML::Node &node,
                                                                        Config* config) {
  const auto type = node["type"].as<std::string>();

  if (type == "SteadyTCM") { return build_steady_tcm(node, config); }
  if (type == "InflatedTCM") { return build_inflated_tcm(node, config); }
  if (type == "LinearTCM") {
    spdlog::info("LinearTCM model built for type: {}", type);
    return build_linear_tcm(node, config);
  }

  return nullptr;
}
