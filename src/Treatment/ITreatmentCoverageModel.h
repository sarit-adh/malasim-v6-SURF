#ifndef TREATMENTCOVERAGEMODEL_H
#define TREATMENTCOVERAGEMODEL_H

#include <yaml-cpp/yaml.h>

#include <vector>

#include "Configuration/Config.h"
#include "Core/types.h"

class ITreatmentCoverageModel {
public:
  // disallow copy and move constructors and assign operators
  ITreatmentCoverageModel(const ITreatmentCoverageModel &) = delete;
  void operator=(const ITreatmentCoverageModel &) = delete;
  ITreatmentCoverageModel(ITreatmentCoverageModel &&) = delete;
  ITreatmentCoverageModel &operator=(ITreatmentCoverageModel &&) = delete;

  ITreatmentCoverageModel() = default;

  virtual ~ITreatmentCoverageModel() = default;

  std::string type;
  int starting_time{0};
  std::vector<double> p_treatment_under_5;
  std::vector<double> p_treatment_over_5;

  virtual double get_probability_to_be_treated(core::LocationId location, core::Age age);

  virtual void monthly_update() = 0;

  static std::unique_ptr<ITreatmentCoverageModel> build_steady_tcm(const YAML::Node &node,
                                                                   Config* config);

  static void read_p_treatment(const YAML::Node &node, std::vector<double> &p_treatments,
                               int number_of_locations);

  static std::unique_ptr<ITreatmentCoverageModel> build_inflated_tcm(const YAML::Node &node,
                                                                     Config* config);

  static std::unique_ptr<ITreatmentCoverageModel> build_linear_tcm(const YAML::Node &node,
                                                                   Config* config);

  static std::unique_ptr<ITreatmentCoverageModel> build(const YAML::Node &node, Config* config);
};

#endif  // TREATMENTCOVERAGEMODEL_H
