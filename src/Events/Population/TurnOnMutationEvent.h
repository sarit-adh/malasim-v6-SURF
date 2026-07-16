#ifndef TURNONMUTATIONEVENT_H
#define TURNONMUTATIONEVENT_H

#include "Events/Event.h"

class TurnOnMutationEvent : public WorldEvent {
public:
  // Disallow copy
  TurnOnMutationEvent(const TurnOnMutationEvent &) = delete;
  TurnOnMutationEvent &operator=(const TurnOnMutationEvent &) = delete;

  // Disallow move
  TurnOnMutationEvent(TurnOnMutationEvent &&) = delete;
  TurnOnMutationEvent &operator=(TurnOnMutationEvent &&) = delete;

  explicit TurnOnMutationEvent(const int &at_time, const double &mutation_probability);
  ~TurnOnMutationEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"turn_on_mutation"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  double mutation_probability{0.0};
  int drug_id{-1};

private:
  void do_execute() override;
};

#endif  // TURNONMUTATIONEVENT_H
