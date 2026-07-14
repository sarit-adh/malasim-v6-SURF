#ifndef IMMUNESYSTEM_H
#define IMMUNESYSTEM_H

#include <vector>

#include "Population/ImmuneSystem/ImmuneComponent.h"
#include "Utils/TypeDef.h"

class Model;

class Person;

class Config;

// typedef std::vector<ImmuneComponent*> ImmuneComponentPtrVector;

class ImmuneSystem {
  // OBJECTPOOL(ImmuneSystem)
public:
  // Disallow copy
  ImmuneSystem(const ImmuneSystem &) = delete;
  ImmuneSystem &operator=(const ImmuneSystem &) = delete;

  explicit ImmuneSystem(Person* person = nullptr);

  virtual ~ImmuneSystem();

  [[nodiscard]] Person* person() const { return person_; }
  void set_person(Person* person) { person_ = person; }

  [[nodiscard]] ImmuneComponent* immune_component() { return &immune_component_; }
  [[nodiscard]] const ImmuneComponent* immune_component() const { return &immune_component_; }
  void set_component_type(ImmuneComponentType type) { immune_component_.set_type(type); }
  void switch_to_non_infant() { immune_component_.switch_to_non_infant(); }

  [[nodiscard]] bool increase() const { return increase_; }
  void set_increase(bool increase) { increase_ = increase; }

  virtual void draw_random_immune();

  virtual void update();

  [[nodiscard]] virtual double get_latest_immune_value() const;

  virtual void set_latest_immune_value(double value);

  [[nodiscard]] virtual double get_current_value() const;

  [[nodiscard]] virtual double get_parasite_size_after_t_days(const int &duration,
                                                              const double &original_size,
                                                              const double &fitness) const;

  [[nodiscard]] virtual double get_clinical_progression_probability() const;

private:
  Person* person_{nullptr};
  ImmuneComponent immune_component_;
  bool increase_{false};

  //    virtual void clear();
};

#endif /* IMMUNESYSTEM_H */
