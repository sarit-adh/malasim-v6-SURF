/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   StrategyBuilder.h
 * Author: Merlin
 *
 * Created on August 23, 2017, 11:03 AM
 */

#ifndef STRATEGYBUILDER_H
#define STRATEGYBUILDER_H

#include <yaml-cpp/yaml.h>

#include "Utils/TypeDef.h"

class IStrategy;

class Config;

class StrategyBuilder {
public:
  StrategyBuilder();

  // disallow copy and assign
  StrategyBuilder(const StrategyBuilder &) = delete;
  void operator=(const StrategyBuilder &) = delete;
  StrategyBuilder(StrategyBuilder &&) = delete;
  StrategyBuilder &operator=(StrategyBuilder &&) = delete;
  virtual ~StrategyBuilder();

  static std::unique_ptr<IStrategy> build(const YAML::Node &ns, const int &strategy_id);

  static void add_therapies(const YAML::Node &ns, IStrategy* result);

  static void add_distributions(const YAML::Node &ns, DoubleVector &distribution);

  static std::unique_ptr<IStrategy> build_sft_strategy(const YAML::Node &ns,
                                                       const int &strategy_id);

  static std::unique_ptr<IStrategy> build_cycling_strategy(const YAML::Node &ns,
                                                           const int &strategy_id);

  static std::unique_ptr<IStrategy> build_adaptive_cycling_strategy(const YAML::Node &ns,
                                                                    const int &strategy_id);

  static std::unique_ptr<IStrategy> build_mft_strategy(const YAML::Node &ns,
                                                       const int &strategy_id);

  static std::unique_ptr<IStrategy> build_mft_rebalancing_strategy(const YAML::Node &ns,
                                                                   const int &strategy_id);

  static std::unique_ptr<IStrategy> build_nested_switching_strategy(const YAML::Node &ns,
                                                                    const int &strategy_id);

  static std::unique_ptr<IStrategy> build_mft_multi_location_strategy(const YAML::Node &ns,
                                                                      const int &strategy_id);

  static std::unique_ptr<IStrategy> build_nested_mft_different_distribution_by_location_strategy(
      const YAML::Node &ns, const int &strategy_id);

  static std::unique_ptr<IStrategy> build_novel_drug_introduction_strategy(const YAML::Node &ns,
                                                                           int strategy_id);

  static std::unique_ptr<IStrategy> build_district_mft_strategy(const YAML::Node &node,
                                                                const int &strategy_id);

  static std::unique_ptr<IStrategy> build_mft_age_based_strategy(const YAML::Node &node,
                                                                 const int &strategy_id);

  static std::unique_ptr<IStrategy> build_public_private_strategy(const YAML::Node &node,
                                                                  const int &strategy_id);

  static std::unique_ptr<IStrategy> build_public_private_multi_location_strategy(
      const YAML::Node &node, const int &strategy_id);
};

#endif /* STRATEGYBUILDER_H */
