#ifndef POMS_CHANGEWITHINHOSTINDUCEDFREERECOMBINATIONEVENT_H
#define POMS_CHANGEWITHINHOSTINDUCEDFREERECOMBINATIONEVENT_H

#include "Events/Event.h"

class ChangeWithinHostInducedFreeRecombinationEvent : public WorldEvent {
public:
  // Disallow copy
  ChangeWithinHostInducedFreeRecombinationEvent(
      const ChangeWithinHostInducedFreeRecombinationEvent &) = delete;
  ChangeWithinHostInducedFreeRecombinationEvent &operator=(
      const ChangeWithinHostInducedFreeRecombinationEvent &) = delete;

  // Disallow move
  ChangeWithinHostInducedFreeRecombinationEvent(ChangeWithinHostInducedFreeRecombinationEvent &&) =
      delete;
  ChangeWithinHostInducedFreeRecombinationEvent &operator=(
      ChangeWithinHostInducedFreeRecombinationEvent &&) = delete;

  explicit ChangeWithinHostInducedFreeRecombinationEvent(const bool &value = false,
                                                         const int &at_time = -1);
  ~ChangeWithinHostInducedFreeRecombinationEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"change_within_host_induced_free_recombination"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

  bool value{true};

private:
  void do_execute() override;
};

#endif  // POMS_CHANGEWITHINHOSTINDUCEDFREERECOMBINATIONEVENT_H
