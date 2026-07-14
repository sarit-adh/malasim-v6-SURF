#ifndef PUBLICPRIVATESTRATEGY_H
#define PUBLICPRIVATESTRATEGY_H

#include "IStrategy.h"
#include "TreatmentSelection.h"

class PublicPrivateStrategy : public IStrategy {
public:
  PublicPrivateStrategy();
  ~PublicPrivateStrategy() override = default;

  PublicPrivateStrategy(const PublicPrivateStrategy &) = delete;
  PublicPrivateStrategy &operator=(const PublicPrivateStrategy &) = delete;
  PublicPrivateStrategy(PublicPrivateStrategy &&) = delete;
  PublicPrivateStrategy &operator=(PublicPrivateStrategy &&) = delete;

  void set_public_strategy(IStrategy* strategy);
  void set_private_strategy(IStrategy* strategy);
  [[nodiscard]] IStrategy* get_public_strategy() const { return public_strategy_; }
  [[nodiscard]] IStrategy* get_private_strategy() const { return private_strategy_; }

  void add_therapy(Therapy* therapy) override;
  Therapy* get_therapy(Person* person) override;
  [[nodiscard]] TreatmentSelection select_treatment(Person* person);
  [[nodiscard]] std::string to_string() const override;
  void adjust_started_time_point(const int &current_time) override;
  void update_end_of_time_step() override;
  void monthly_update() override;

  void adjust_public_share(int time);

  double public_share{0.0};
  double start_public_share{0.0};
  double peak_public_share{0.0};
  int starting_time{0};
  int peak_after{0};

private:
  IStrategy* public_strategy_{nullptr};
  IStrategy* private_strategy_{nullptr};
};

#endif  // PUBLICPRIVATESTRATEGY_H
