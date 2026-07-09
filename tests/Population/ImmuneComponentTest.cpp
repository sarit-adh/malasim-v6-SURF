#include <cmath>

#include "Population/ImmuneSystem/ImmuneComponent.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/ImmuneSystem/ImmunityClearanceUpdateFunction.h"
#include "Population/ImmuneSystem/InfantImmuneComponent.h"
#include "Population/ImmuneSystem/NonInfantImmuneComponent.h"
#include "Simulation/Model.h"
#include "Utils/Cli.h"
#include "fixtures/TestFileGenerators.h"
#include "gtest/gtest.h"

namespace {
struct ModelTestEnvironment {
  ModelTestEnvironment() {
    test_fixtures::setup_test_environment("test_input.yml");
    Model::get_instance()->release();
    utils::Cli::MaSimAppInput cli_input;
    cli_input.input_path = "test_input.yml";
    Model::set_cli_input(cli_input);
    Model::get_instance()->initialize();
  }

  ~ModelTestEnvironment() {
    Model::get_instance()->release();
    test_fixtures::cleanup_test_files();
  }
};
}  // namespace

// Dummy classes for isolation
template <typename Base>
class DummyComponent : public Base {
public:
  double value = 1.23;
  DummyComponent(ImmuneSystem* immune_system = nullptr) : Base(immune_system) {}
  double get_current_value() override { return value; }
  double get_decay_rate(core::Age age) const override { return 0.1 + age; }
  double get_acquire_rate(core::Age age) const override { return 0.2 + age; }
  void update() override { value += 1.0; }
  void draw_random_immune() override { value = 42.0; }
};

TEST(ImmuneComponentTest, ConstructionAndDefaults) {
  DummyComponent<ImmuneComponent> comp;
  EXPECT_EQ(comp.latest_value(), 0);
  comp.set_latest_value(3.14);
  EXPECT_DOUBLE_EQ(comp.latest_value(), 3.14);
}

TEST(ImmuneComponentTest, VirtualMethods) {
  DummyComponent<ImmuneComponent> comp;
  EXPECT_DOUBLE_EQ(comp.get_current_value(), 1.23);
  EXPECT_DOUBLE_EQ(comp.get_decay_rate(2), 2.1);
  EXPECT_DOUBLE_EQ(comp.get_acquire_rate(2), 2.2);
  comp.update();
  EXPECT_DOUBLE_EQ(comp.value, 2.23);
  comp.draw_random_immune();
  EXPECT_DOUBLE_EQ(comp.value, 42.0);
}

TEST(InfantImmuneComponentTest, ConstructionAndMethods) {
  ModelTestEnvironment model_environment;
  InfantImmuneComponent comp;
  EXPECT_NO_THROW({ auto decay_rate = comp.get_decay_rate(0); });
  EXPECT_NO_THROW({ auto acquire_rate = comp.get_acquire_rate(0); });
  EXPECT_NO_THROW({ auto value = comp.get_current_value(); });
}

TEST(NonInfantImmuneComponentTest, ConstructionAndMethods) {
  ModelTestEnvironment model_environment;
  NonInfantImmuneComponent comp;
  EXPECT_NO_THROW({ auto decay_rate = comp.get_decay_rate(10); });
  EXPECT_NO_THROW({ auto acquire_rate = comp.get_acquire_rate(10); });
}

TEST(NonInfantImmuneComponentTest, OneDayFactorsUseProcessedConfigCache) {
  ModelTestEnvironment model_environment;
  NonInfantImmuneComponent comp;

  EXPECT_DOUBLE_EQ(comp.get_one_day_acquire_factor(10), std::exp(-comp.get_acquire_rate(10)));
  EXPECT_DOUBLE_EQ(comp.get_one_day_decay_factor(10), std::exp(-comp.get_decay_rate(10)));
}

// ImmunityClearanceUpdateFunction: requires Model and ClonalParasitePopulation mocks
class DummyModel {};
class DummyParasite {};

class DummyImmunityClearanceUpdateFunction : public ImmunityClearanceUpdateFunction {
public:
  DummyImmunityClearanceUpdateFunction() : ImmunityClearanceUpdateFunction(nullptr) {}
  double get_current_parasite_density(ClonalParasitePopulation* parasite, int duration) override {
    return 123.45 + duration;
  }
};

TEST(ImmunityClearanceUpdateFunctionTest, ConstructionAndMethod) {
  DummyImmunityClearanceUpdateFunction func;
  EXPECT_DOUBLE_EQ(func.get_current_parasite_density(nullptr, 5), 128.45);
}
