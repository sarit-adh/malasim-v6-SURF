#ifndef POMS_ChangeMutationProbabilityPerLocusEVENT_H
#define POMS_ChangeMutationProbabilityPerLocusEVENT_H

#include "Events/Event.h"

class ChangeMutationProbabilityPerLocusEvent : public WorldEvent {
public:
  // Disallow copy
  ChangeMutationProbabilityPerLocusEvent(const ChangeMutationProbabilityPerLocusEvent &) = delete;
  ChangeMutationProbabilityPerLocusEvent &operator=(
      const ChangeMutationProbabilityPerLocusEvent &) = delete;

  // Disallow move
  ChangeMutationProbabilityPerLocusEvent(ChangeMutationProbabilityPerLocusEvent &&) = delete;
  ChangeMutationProbabilityPerLocusEvent &operator=(ChangeMutationProbabilityPerLocusEvent &&) =
      delete;

  explicit ChangeMutationProbabilityPerLocusEvent(const double &value = 0.0,
                                                  const int &at_time = -1);
  ~ChangeMutationProbabilityPerLocusEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_mutation_probability_per_locus"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  double value{0.001};

private:
  void do_execute() override;
};

#endif  // POMS_ChangeMutationProbabilityPerLocusEVENT_H
