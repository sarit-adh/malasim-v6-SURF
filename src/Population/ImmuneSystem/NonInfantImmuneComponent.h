#ifndef NONINFANTIMMUNECOMPONENT
#define NONINFANTIMMUNECOMPONENT

#include "ImmuneComponent.h"

class NonInfantImmuneComponent : public ImmuneComponent {
public:
  explicit NonInfantImmuneComponent(ImmuneSystem* immune_system = nullptr);

  // disallow copy and assign
  NonInfantImmuneComponent(const NonInfantImmuneComponent &) = delete;
  void operator=(const NonInfantImmuneComponent &) = delete;
  NonInfantImmuneComponent(NonInfantImmuneComponent &&) = delete;
  NonInfantImmuneComponent &operator=(NonInfantImmuneComponent &&) = delete;
  // NonInfantImmuneComponent(const NonInfantImmuneComponent& orig);
  ~NonInfantImmuneComponent() override;

  [[nodiscard]] double get_decay_rate(core::Age age) const override;

  [[nodiscard]] double get_acquire_rate(core::Age age) const override;

  [[nodiscard]] double get_one_day_decay_factor(core::Age age) const override;

  [[nodiscard]] double get_one_day_acquire_factor(core::Age age) const override;

private:
};

#endif /* NONINFANTIMMUNECOMPONENT */
