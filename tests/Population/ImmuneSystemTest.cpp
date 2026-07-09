#include <memory>

#include "Population/ImmuneSystem/ImmuneComponent.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Person/Person.h"
#include "gtest/gtest.h"

// Dummy ImmuneComponent for full control
class DummyImmuneComponent : public ImmuneComponent {
public:
  double value = 0.0;
  DummyImmuneComponent(ImmuneSystem* immune_system = nullptr) : ImmuneComponent(immune_system) {}
  double get_current_value() override { return value; }
  double get_decay_rate(core::Age age) const override { return 0.1; }
  double get_acquire_rate(core::Age age) const override { return 0.2; }
  void update() override {};
  void draw_random_immune() override { /* can be overridden in derived */ }
};

TEST(ImmuneSystemTest, ConstructionAndSetters) {
  ImmuneSystem immune;
  EXPECT_EQ(immune.person(), nullptr);
  Person person;
  immune.set_person(&person);
  EXPECT_EQ(immune.person(), &person);
}

TEST(ImmuneSystemTest, SetAndGetImmuneComponent) {
  ImmuneSystem immune;
  auto component = std::make_unique<DummyImmuneComponent>(&immune);
  auto* ptr = component.get();
  immune.set_immune_component(std::move(component));
  EXPECT_EQ(immune.immune_component(), ptr);
}

TEST(ImmuneSystemTest, SetLatestImmuneValueAndGetCurrent) {
  ImmuneSystem immune;
  auto component = std::make_unique<DummyImmuneComponent>(&immune);
  auto* ptr = component.get();
  immune.set_immune_component(std::move(component));
  immune.set_latest_immune_value(0.5);
  ptr->value = 0.5;
  EXPECT_DOUBLE_EQ(immune.get_current_value(), 0.5);
}

TEST(ImmuneSystemTest, IncreaseFlag) {
  ImmuneSystem immune;
  EXPECT_FALSE(immune.increase());
}

TEST(ImmuneSystemTest, DrawRandomImmuneDelegates) {
  ImmuneSystem immune;
  struct DrawImmuneComponent : public DummyImmuneComponent {
    bool called = false;
    DrawImmuneComponent(ImmuneSystem* immune_system = nullptr)
        : DummyImmuneComponent(immune_system) {}
    void draw_random_immune() override { called = true; }
  };
  auto component = std::make_unique<DrawImmuneComponent>(&immune);
  auto* ptr = component.get();
  immune.set_immune_component(std::move(component));
  immune.draw_random_immune();
  EXPECT_TRUE(ptr->called);
}

TEST(ImmuneSystemTest, UpdateCallsComponentUpdate) {
  ImmuneSystem immune;
  struct FlagImmuneComponent : public DummyImmuneComponent {
    bool updated = false;
    FlagImmuneComponent(ImmuneSystem* immune_system = nullptr)
        : DummyImmuneComponent(immune_system) {}
    void update() override { updated = true; }
  };
  auto component = std::make_unique<FlagImmuneComponent>(&immune);
  auto* ptr = component.get();
  immune.set_immune_component(std::move(component));
  immune.update();
  EXPECT_TRUE(ptr->updated);
}

// get_parasite_size_after_t_days and get_clinical_progression_probability require more mocks or
// integration
