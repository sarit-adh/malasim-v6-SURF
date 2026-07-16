#pragma once

#include <vector>

#include "Events/Event.h"

/**
 * @brief Event that represents a single round of Mass Drug Administration (MDA)
 *
 * This event handles the administration of drugs to a targeted fraction of the population
 * over a specified number of days.
 */
class SingleRoundMDAEvent : public WorldEvent {
public:
  // Disallow copy
  SingleRoundMDAEvent(const SingleRoundMDAEvent &) = delete;
  SingleRoundMDAEvent &operator=(const SingleRoundMDAEvent &) = delete;

  // Disallow move
  SingleRoundMDAEvent(SingleRoundMDAEvent &&) = delete;
  SingleRoundMDAEvent &operator=(SingleRoundMDAEvent &&) = delete;
  ~SingleRoundMDAEvent() override = default;

  /**
   * @brief Constructs a new Single Round MDA Event
   * @param at_time The time at which the event should execute (-1 for immediate execution)
   */
  explicit SingleRoundMDAEvent(const int &at_time = -1);

  explicit SingleRoundMDAEvent(std::vector<double> fraction_population_targeted)
      : fraction_population_targeted_(std::move(fraction_population_targeted)) {}
  /**
   * @brief Gets the targeted population fractions
   * @return The vector of population fractions targeted for treatment
   */
  [[nodiscard]] const std::vector<double> &get_fraction_population_targeted() const {
    return fraction_population_targeted_;
  }

  /**
   * @brief Sets the targeted population fractions
   * @param fractions The vector of population fractions to target
   */
  void set_fraction_population_targeted(const std::vector<double> &fractions) {
    fraction_population_targeted_ = fractions;
  }

  /**
   * @brief Adds a single population fraction to the target vector
   * @param fraction The fraction to add
   */
  void add_fraction_population_targeted(double fraction) {
    fraction_population_targeted_.push_back(fraction);
  }

  /**
   * @brief Gets the number of days to complete all treatments
   * @return The number of days
   */
  [[nodiscard]] int get_days_to_complete() const { return days_to_complete_all_treatments_; }

  /**
   * @brief Sets the number of days to complete all treatments
   * @param days The number of days
   */
  void set_days_to_complete(const int days) { days_to_complete_all_treatments_ = days; }

  /**
   * @brief Gets the name of the event
   * @return The event name
   */
  static constexpr std::string_view EVENT_NAME{"single_round_MDA"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  std::vector<double> fraction_population_targeted_;
  int days_to_complete_all_treatments_{14};

  void do_execute() override;
};
