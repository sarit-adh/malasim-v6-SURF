#ifndef ISTRATEGY_H
#define ISTRATEGY_H

#include <map>
#include <string>
#include <utility>
#include <vector>

class Therapy;

class Person;

class IStrategy {
public:
  enum StrategyType {
    SFT = 0,
    Cycling = 1,
    MFT = 2,
    AdaptiveCycling = 3,
    MFTRebalancing = 4,
    MFTMultiLocation = 5,
    NestedMFT = 6,
    NestedMFTMultiLocation = 7,
    NovelDrugIntroduction = 8,
    DistrictMft = 9,
    MFTAgeBased = 10,
    PublicPrivate = 11,
    PublicPrivateMultiLocation = 12,
  };

  static std::map<std::string, StrategyType> strategy_type_map;

  IStrategy(IStrategy &&) = delete;
  IStrategy &operator=(IStrategy &&) = delete;
  // Disallow copy
  IStrategy(const IStrategy &) = delete;
  IStrategy &operator=(const IStrategy &) = delete;

  IStrategy(std::string name, const StrategyType &type) : name{std::move(name)}, type{type} {}

  virtual ~IStrategy() = default;

  virtual bool is_strategy(const std::string &s_name) { return name == s_name; }

  [[nodiscard]] virtual StrategyType get_type() const { return type; };

  virtual void add_therapy(Therapy* therapy) = 0;

  virtual Therapy* get_therapy(Person* person) = 0;

  [[nodiscard]] virtual std::string to_string() const = 0;

  virtual void adjust_started_time_point(const int &current_time) = 0;

  /**
   * This function will be executed at end of time step, to check and switch therapy if needed
   */
  virtual void update_end_of_time_step() = 0;

  virtual void monthly_update() = 0;

  // Properties
  int id{-1};
  std::string name;
  StrategyType type;
};

#endif /* ISTRATEGY_H */
