#ifndef IMMUNESYSTEMPARAMETEROVERRIDES_H
#define IMMUNESYSTEMPARAMETEROVERRIDES_H

#include <map>
#include <stdexcept>
#include <string>

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

// Full config-path keys used in override maps (colon-separated YAML path segments).
// These match the layout of the config file so users can identify what is being overridden.
namespace ImmuneSystemOverridePaths {
  constexpr const char* K_P_CI_SYMP =
      "epidemiological_parameters:allow_new_coinfection_to_cause_symptoms:probability";
  constexpr const char* K_Z =
      "immune_system_parameters:immune_effect_on_progression_to_clinical";
  constexpr const char* K_KAPPA =
      "immune_system_parameters:factor_effect_age_mature_immunity";
  constexpr const char* K_MIDPOINT =
      "immune_system_parameters:midpoint";
  constexpr const char* K_P_SEEK_BASE =
      "epidemiological_parameters:age_based_probability_of_seeking_treatment:power:base";
  constexpr const char* K_MUTATION_PROB =
      "genotype_parameters:mutation_probability_per_locus";
  constexpr const char* K_DEFAULT_CNV_REVERSION_MULTIPLIER =
      "genotype_parameters:default_cnv_reversion_multiplier";
}  // namespace ImmuneSystemOverridePaths

// A single override set: a map from full config-path key to the override value.
struct ImmuneSystemParameterCandidate {
  std::map<std::string, double> overrides;

  [[nodiscard]] bool has(const std::string &path) const {
    return overrides.count(path) > 0;
  }

  [[nodiscard]] double get(const std::string &path, double default_val) const {
    const auto it = overrides.find(path);
    return (it != overrides.end()) ? it->second : default_val;
  }
};

// Container for all override candidates and the index selecting which is used in simulation.
class ImmuneSystemParameterOverrides {
public:
  [[nodiscard]] int get_used_in_simulation() const { return used_in_simulation_; }
  void set_used_in_simulation(const int value) { used_in_simulation_ = value; }

  [[nodiscard]] bool get_random_selection() const { return random_selection_; }
  void set_random_selection(const bool value) { random_selection_ = value; }

  [[nodiscard]] const std::map<int, ImmuneSystemParameterCandidate> &get_candidates() const {
    return candidates_;
  }
  void set_candidates(const std::map<int, ImmuneSystemParameterCandidate> &value) {
    candidates_ = value;
  }

  [[nodiscard]] bool has_selected_candidate() const {
    return candidates_.count(used_in_simulation_) > 0;
  }

  [[nodiscard]] const ImmuneSystemParameterCandidate &get_selected_candidate() const {
    auto it = candidates_.find(used_in_simulation_);
    if (it == candidates_.end()) {
      throw std::runtime_error(
          fmt::format("immune_system_parameter_overrides: used_in_simulation={} not found in candidates",
                      used_in_simulation_));
    }
    return it->second;
  }

  void log_all() const {
    spdlog::info("ImmuneSystemParameterOverrides: used_in_simulation={}, random_selection={}",
                 used_in_simulation_, random_selection_);
    for (const auto &[idx, c] : candidates_) {
      for (const auto &[path, val] : c.overrides) {
        spdlog::info("  candidate[{}]: {}={}", idx, path, val);
      }
    }
  }

private:
  int used_in_simulation_ = 0;
  bool random_selection_ = false;
  std::map<int, ImmuneSystemParameterCandidate> candidates_;
};

// YAML conversion for ImmuneSystemParameterOverrides
template <>
struct YAML::convert<ImmuneSystemParameterOverrides> {
  static Node encode(const ImmuneSystemParameterOverrides &rhs) {
    Node node;
    node["used_in_simulation"] = rhs.get_used_in_simulation();
    node["random_selection"] = rhs.get_random_selection();
    Node candidates_node;
    for (const auto &[idx, c] : rhs.get_candidates()) {
      Node cnode;
      for (const auto &[path, val] : c.overrides) {
        cnode[path] = val;
      }
      candidates_node[idx] = cnode;
    }
    node["candidates"] = candidates_node;
    return node;
  }

  static bool decode(const Node &node, ImmuneSystemParameterOverrides &rhs) {
    if (!node["used_in_simulation"]) {
      throw std::runtime_error(
          "immune_system_parameter_overrides: missing 'used_in_simulation'");
    }
    if (!node["candidates"]) {
      throw std::runtime_error(
          "immune_system_parameter_overrides: missing 'candidates'");
    }

    rhs.set_used_in_simulation(node["used_in_simulation"].as<int>());
    if (node["random_selection"]) {
      rhs.set_random_selection(node["random_selection"].as<bool>());
    }

    std::map<int, ImmuneSystemParameterCandidate> candidates;
    YAML::Node cmap = node["candidates"];
    for (auto it = cmap.begin(); it != cmap.end(); ++it) {
      const int idx = it->first.as<int>();
      YAML::Node cnode = it->second;

      ImmuneSystemParameterCandidate c;
      for (auto kv = cnode.begin(); kv != cnode.end(); ++kv) {
        c.overrides[kv->first.as<std::string>()] = kv->second.as<double>();
      }
      candidates[idx] = c;
    }
    rhs.set_candidates(candidates);
    return true;
  }
};

#endif  // IMMUNESYSTEMPARAMETEROVERRIDES_H