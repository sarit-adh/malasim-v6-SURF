#ifndef IMMUNECOMPONENT_H
#define IMMUNECOMPONENT_H

// #include "ObjectPool.h"
#include <cstddef>
#include <cstdint>

#include "Core/types.h"
class ImmuneSystem;

class Model;

enum class ImmuneComponentType : std::uint8_t {
  Infant,
  NonInfant,
};

class ImmuneComponent final {
public:
  explicit ImmuneComponent(ImmuneSystem* immune_system = nullptr,
                           ImmuneComponentType type = ImmuneComponentType::NonInfant);

  ImmuneComponent(ImmuneComponent &&) = delete;
  ImmuneComponent &operator=(ImmuneComponent &&) = delete;
  // disallow copy and assign
  ImmuneComponent(const ImmuneComponent &) = delete;
  void operator=(const ImmuneComponent &) = delete;
  //    ImmuneComponent(const ImmuneComponent& orig);
  [[nodiscard]] ImmuneSystem* immune_system() const { return immune_system_; }

  void set_immune_system(ImmuneSystem* immune_system) { immune_system_ = immune_system; }

  [[nodiscard]] double latest_value() const { return latest_value_; }
  void set_latest_value(double latest_value) { latest_value_ = latest_value; }

  [[nodiscard]] ImmuneComponentType type() const { return type_; }
  void set_type(ImmuneComponentType type) { type_ = type; }
  void switch_to_non_infant() { type_ = ImmuneComponentType::NonInfant; }

  void update();

  void draw_random_immune();

  [[nodiscard]] double get_current_value() const;

  [[nodiscard]] double get_decay_rate(core::Age age) const;

  [[nodiscard]] double get_acquire_rate(core::Age age) const;

  [[nodiscard]] double get_one_day_decay_factor(core::Age age) const;

  [[nodiscard]] double get_one_day_acquire_factor(core::Age age) const;

private:
  ImmuneSystem* immune_system_{nullptr};
  double latest_value_{0};
  ImmuneComponentType type_{ImmuneComponentType::NonInfant};
};

#endif /* IMMUNECOMPONENT_H */
