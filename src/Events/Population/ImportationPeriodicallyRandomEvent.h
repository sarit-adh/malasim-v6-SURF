/*
 * ImportationPeriodicallyRandomEvent.h
 *
 * Import the indicated genotype on a populated weighted basis at the time
 * step indicated, reschedule the event to occur periodically afterwards.
 */
#ifndef IMPORTATIONPERIODICALLYRANDOMEVENT_HXX
#define IMPORTATIONPERIODICALLYRANDOMEVENT_HXX

#include <date/date.h>

#include "Events/Event.h"
#include "Population/Person/Person.h"

/**
 * @brief ImportationPeriodicallyRandomEvent is a class that imports infections
 * on a population weighted basis at the time step indicated, rescheduling the
 * event to occur periodically afterwards.
 * Import for a particular month of the year and reschedule for the same month
 * of the following year.
 */
class ImportationPeriodicallyRandomEvent : public WorldEvent {
public:
  ImportationPeriodicallyRandomEvent(const ImportationPeriodicallyRandomEvent &) = delete;
  ImportationPeriodicallyRandomEvent(ImportationPeriodicallyRandomEvent &&) = delete;
  ImportationPeriodicallyRandomEvent &operator=(const ImportationPeriodicallyRandomEvent &) =
      delete;
  ImportationPeriodicallyRandomEvent &operator=(ImportationPeriodicallyRandomEvent &&) = delete;
  ImportationPeriodicallyRandomEvent(int genotype_id,
                                     int start,
                                     int count,
                                     double log_parasite_density)
      : genotype_id_(genotype_id), log_parasite_density_(log_parasite_density), count_(count) {
    set_time(start);
  }
  ~ImportationPeriodicallyRandomEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"importation_periodically_random"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  int count_ = 0;                      // Number of infections to inflict per month
  int genotype_id_ = 0;                // Genotype id to inflict
  double log_parasite_density_ = 0.0;  // Log parasite density to inflict

  // Execute the import event
  void do_execute() override;

  // Inflict the act infection upon the individual
  void infect(Person* person, int genotype_id) const;
  static Person* get_random_susceptible();
};

#endif
