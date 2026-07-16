#ifndef MODIFYNESTEDMFTEVENT_H
#define MODIFYNESTEDMFTEVENT_H

#include "Events/Event.h"

class ModifyNestedMFTEvent : public WorldEvent {
public:
  // Disallow copy
  ModifyNestedMFTEvent(const ModifyNestedMFTEvent &) = delete;
  ModifyNestedMFTEvent &operator=(const ModifyNestedMFTEvent &) = delete;

  // Disallow move
  ModifyNestedMFTEvent(ModifyNestedMFTEvent &&) = delete;
  ModifyNestedMFTEvent &operator=(ModifyNestedMFTEvent &&) = delete;

  int strategy_id{-1};

  ModifyNestedMFTEvent(const int &at_time, const int &strategy_id);

  ~ModifyNestedMFTEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"modify_nested_mft_strategy"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // MODIFYNESTEDMFTEVENT_H
