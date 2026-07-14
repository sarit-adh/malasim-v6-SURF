#ifndef PUBLICPRIVATEMULTILOCATIONSTRATEGY_H
#define PUBLICPRIVATEMULTILOCATIONSTRATEGY_H

#include "IStrategy.h"
#include "TreatmentSelection.h"
#include "Utils/TypeDef.h"

class PublicPrivateMultiLocationStrategy : public IStrategy {
public:
  PublicPrivateMultiLocationStrategy();
  ~PublicPrivateMultiLocationStrategy() override = default;

  PublicPrivateMultiLocationStrategy(const PublicPrivateMultiLocationStrategy &) = delete;
  PublicPrivateMultiLocationStrategy &operator=(const PublicPrivateMultiLocationStrategy &) =
      delete;
  PublicPrivateMultiLocationStrategy(PublicPrivateMultiLocationStrategy &&) = delete;
  PublicPrivateMultiLocationStrategy &operator=(PublicPrivateMultiLocationStrategy &&) = delete;

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

  void adjust_public_shares(int time);

  DoubleVector public_share_by_location;
  DoubleVector start_public_share_by_location;
  DoubleVector peak_public_share_by_location;
  int starting_time{0};
  int peak_after{0};

private:
  IStrategy* public_strategy_{nullptr};
  IStrategy* private_strategy_{nullptr};
};

#endif  // PUBLICPRIVATEMULTILOCATIONSTRATEGY_H
