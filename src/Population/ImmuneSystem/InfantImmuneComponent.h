#ifndef INFANTIMMUNECOMPONENT_H
#define INFANTIMMUNECOMPONENT_H

#include "ImmuneComponent.h"

class InfantImmuneComponent : public ImmuneComponent {
  // disallow copy and assign
public:
  explicit InfantImmuneComponent(ImmuneSystem* immune_system = nullptr);

  InfantImmuneComponent(InfantImmuneComponent &&) = delete;
  InfantImmuneComponent(const InfantImmuneComponent &) = delete;
  void operator=(const InfantImmuneComponent &) = delete;
  InfantImmuneComponent &operator=(InfantImmuneComponent &&) = delete;
  // InfantImmuneComponent(const InfantImmuneComponent& orig);
  ~InfantImmuneComponent() override;

  [[nodiscard]] double get_decay_rate(const int &age) const override;

  [[nodiscard]] double get_acquire_rate(const int &age) const override;

  [[nodiscard]] double get_current_value() override;
};

#endif /* INFANTIMMUNECOMPONENT_H */
