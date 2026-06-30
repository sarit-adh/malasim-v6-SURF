#ifndef IMMUNESYSTEMPARAMETERCANDIDATES_H
#define IMMUNESYSTEMPARAMETERCANDIDATES_H

#include <map>
#include <stdexcept>
#include <string>

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

// A single candidate set of immune/clinical parameters for parameter sweeps.
struct ImmuneSystemParameterCandidate {
  double p_ci_symp  = 1.0;  // probability of clinical symptoms given co-infection
  double z          = 5.4;  // immune_effect_on_progression_to_clinical (slope)
  double kappa      = 0.3;  // factor_effect_age_mature_immunity (inflection adj.)
  double midpoint   = 0.4;  // midpoint of sigmoidal immunity curve
  double p_seek_base = 1.0; // base for age-based probability of seeking treatment
};

// Container for all candidates and the index selecting which is used in simulation.
class ImmuneSystemParameterCandidates {
public:
  [[nodiscard]] int get_used_in_simulation() const { return used_in_simulation_; }
  void set_used_in_simulation(const int value) { used_in_simulation_ = value; }

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
          fmt::format("immune_system_paprameter_candidates: used_in_simulation={} not found in candidates",
                      used_in_simulation_));
    }
    return it->second;
  }

  void log_all() const {
    spdlog::info("ImmuneSystemParameterCandidates: used_in_simulation={}", used_in_simulation_);
    for (const auto &[idx, c] : candidates_) {
      spdlog::info(
          "  candidate[{}]: p_ci_symp={}, z={}, kappa={}, midpoint={}, p_seek_base={}",
          idx, c.p_ci_symp, c.z, c.kappa, c.midpoint, c.p_seek_base);
    }
  }

private:
  int used_in_simulation_ = 0;
  std::map<int, ImmuneSystemParameterCandidate> candidates_;
};

// YAML conversion for ImmuneSystemParameterCandidates
template <>
struct YAML::convert<ImmuneSystemParameterCandidates> {
  static Node encode(const ImmuneSystemParameterCandidates &rhs) {
    Node node;
    node["used_in_simulation"] = rhs.get_used_in_simulation();
    Node candidates_node;
    for (const auto &[idx, c] : rhs.get_candidates()) {
      Node cnode;
      cnode["p_ci_symp"]   = c.p_ci_symp;
      cnode["z"]           = c.z;
      cnode["kappa"]       = c.kappa;
      cnode["midpoint"]    = c.midpoint;
      cnode["p_seek_base"] = c.p_seek_base;
      candidates_node[idx] = cnode;
    }
    node["candidates"] = candidates_node;
    return node;
  }

  static bool decode(const Node &node, ImmuneSystemParameterCandidates &rhs) {
    if (!node["used_in_simulation"]) {
      throw std::runtime_error(
          "immune_system_paprameter_candidates: missing 'used_in_simulation'");
    }
    if (!node["candidates"]) {
      throw std::runtime_error(
          "immune_system_paprameter_candidates: missing 'candidates'");
    }

    rhs.set_used_in_simulation(node["used_in_simulation"].as<int>());

    std::map<int, ImmuneSystemParameterCandidate> candidates;
    const auto &cmap = node["candidates"];
    for (auto it = cmap.begin(); it != cmap.end(); ++it) {
      const int idx = it->first.as<int>();
      const auto &cnode = it->second;

      auto require_field = [&](const std::string &field) {
        if (!cnode[field]) {
          throw std::runtime_error(
              fmt::format("immune_system_paprameter_candidates.candidates[{}]: missing '{}'",
                          idx, field));
        }
      };
      require_field("p_ci_symp");
      require_field("z");
      require_field("kappa");
      require_field("midpoint");
      require_field("p_seek_base");

      ImmuneSystemParameterCandidate c;
      c.p_ci_symp   = cnode["p_ci_symp"].as<double>();
      c.z           = cnode["z"].as<double>();
      c.kappa       = cnode["kappa"].as<double>();
      c.midpoint    = cnode["midpoint"].as<double>();
      c.p_seek_base = cnode["p_seek_base"].as<double>();
      candidates[idx] = c;
    }
    rhs.set_candidates(candidates);
    return true;
  }
};

#endif  // IMMUNESYSTEMPARAMETERCANDIDATES_H
