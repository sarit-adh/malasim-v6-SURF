#ifndef SFTSTRATEGY_H
#define SFTSTRATEGY_H

#include <vector>

#include "IStrategy.h"

class SFTStrategy : public IStrategy {
  //disallow copy and assign and move
  SFTStrategy(const SFTStrategy&) = delete;
  void operator=(const SFTStrategy&) = delete;
  SFTStrategy(SFTStrategy&&) = delete;
  SFTStrategy& operator=(SFTStrategy&&) = delete;

private:
  std::vector<Therapy *> therapy_list_;
public:
  std::vector<Therapy *> get_therapy_list() const {
    return therapy_list_;
  }
  void set_therapy_list(const std::vector<Therapy *> &therapy_list) {
    therapy_list_ = therapy_list;
  }


 public:
  SFTStrategy();

  //    SFTStrategy(const SFTStrategy& orig);
  virtual ~SFTStrategy();

  virtual std::vector<Therapy *> &get_therapy_list();

  void add_therapy(Therapy *therapy) override;

  Therapy *get_therapy(Person *person) override;

  std::string to_string() const override;

  void update_end_of_time_step() override;

  void adjust_started_time_point(const int &current_time) override;

  void monthly_update() override;

};

#endif /* SFTSTRATEGY_H */
