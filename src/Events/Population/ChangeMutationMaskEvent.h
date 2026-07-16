#ifndef CHANGEMUTATIONMASKEVENT_H
#define CHANGEMUTATIONMASKEVENT_H

#include <vector>

#include "Events/Event.h"

class ChangeMutationMaskEvent : public WorldEvent {
public:
  // disallow copy, assign and move
  ChangeMutationMaskEvent(const ChangeMutationMaskEvent &) = delete;
  void operator=(const ChangeMutationMaskEvent &) = delete;
  ChangeMutationMaskEvent(ChangeMutationMaskEvent &&) = delete;
  void operator=(ChangeMutationMaskEvent &&) = delete;

  std::vector<bool> mask;

  explicit ChangeMutationMaskEvent(const std::vector<bool> &mask, const int &at_time = -1);

  ~ChangeMutationMaskEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_mutation_mask"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // CHANGEMUTATIONMASKEVENT_H
