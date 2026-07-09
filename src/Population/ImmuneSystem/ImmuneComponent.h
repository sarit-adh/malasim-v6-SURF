#ifndef IMMUNECOMPONENT_H
#define IMMUNECOMPONENT_H

// #include "ObjectPool.h"
#include <cstddef>

#include "Core/types.h"
class ImmuneSystem;

class Model;

class ImmuneComponent {
public:
  explicit ImmuneComponent(ImmuneSystem* immune_system = nullptr);

  ImmuneComponent(ImmuneComponent &&) = delete;
  ImmuneComponent &operator=(ImmuneComponent &&) = delete;
  // disallow copy and assign
  ImmuneComponent(const ImmuneComponent &) = delete;
  void operator=(const ImmuneComponent &) = delete;
  //    ImmuneComponent(const ImmuneComponent& orig);
  virtual ~ImmuneComponent();

  [[nodiscard]] ImmuneSystem* immune_system() const { return immune_system_; }

  void set_immune_system(ImmuneSystem* immune_system) { immune_system_ = immune_system; }

  [[nodiscard]] double latest_value() const { return latest_value_; }
  void set_latest_value(double latest_value) { latest_value_ = latest_value; }

  virtual void update();

  virtual void draw_random_immune();

  [[nodiscard]] virtual double get_current_value();

  [[nodiscard]] virtual double get_decay_rate(core::Age age) const = 0;

  [[nodiscard]] virtual double get_acquire_rate(core::Age age) const = 0;

  [[nodiscard]] virtual double get_one_day_decay_factor(core::Age age) const;

  [[nodiscard]] virtual double get_one_day_acquire_factor(core::Age age) const;

private:
  ImmuneSystem* immune_system_{nullptr};
  double latest_value_{0};
};

#endif /* IMMUNECOMPONENT_H */
