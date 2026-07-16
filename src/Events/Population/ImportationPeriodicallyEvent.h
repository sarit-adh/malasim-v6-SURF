#ifndef IMPORTATIONPERIODICALLYEVENT_H
#define IMPORTATIONPERIODICALLYEVENT_H

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

class Scheduler;

class ImportationPeriodicallyEvent : public WorldEvent {
public:
  // disallow copy and assign
  ImportationPeriodicallyEvent(const ImportationPeriodicallyEvent &) = delete;
  void operator=(const ImportationPeriodicallyEvent &) = delete;
  ImportationPeriodicallyEvent(ImportationPeriodicallyEvent &&) = delete;
  void operator=(ImportationPeriodicallyEvent &&) = delete;

  // OBJECTPOOL(ImportationPeriodicallyEvent)
private:
  int location_;
  int duration_;
  int genotype_id_;
  int number_of_cases_;

public:
  [[nodiscard]] int get_location() const { return location_; }
  void set_location(int location) { location_ = location; }
  [[nodiscard]] int get_duration() const { return duration_; }
  void set_duration(int duration) { duration_ = duration; }
  [[nodiscard]] int get_genotype_id() const { return genotype_id_; }
  void set_genotype_id(int genotype_id) { genotype_id_ = genotype_id; }
  [[nodiscard]] int get_number_of_cases() const { return number_of_cases_; }
  void set_number_of_cases(int number_of_cases) { number_of_cases_ = number_of_cases; }

  explicit ImportationPeriodicallyEvent(const int &location = -1,
                                        const int &duration = -1,
                                        int genotype_id = -1,
                                        const int &number_of_cases = -1,
                                        const int &start_day = -1);

  //    ImportationEvent(const ImportationEvent& orig);
  ~ImportationPeriodicallyEvent() override;

  static constexpr std::string_view EVENT_NAME{"introduce_parasites_periodically"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif /* IMPORTATIONPERIODICALLYEVENT_H */
