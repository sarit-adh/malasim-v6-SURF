#include <cmath>

#include "Population/ImmuneSystem/ImmuneComponent.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/ImmuneSystem/ImmuneSystemConstants.h"
#include "Population/ImmuneSystem/ImmunityClearanceUpdateFunction.h"
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

TEST(ImmuneComponentTest, ConstructionAndDefaults) {
  ImmuneComponent comp;
  EXPECT_EQ(comp.latest_value(), 0);
  EXPECT_EQ(comp.type(), ImmuneComponentType::NonInfant);
  comp.set_latest_value(3.14);
  EXPECT_DOUBLE_EQ(comp.latest_value(), 3.14);
}

TEST(ImmuneComponentTest, InfantModeUsesInfantRates) {
  ModelTestEnvironment model_environment;
  ImmuneComponent comp(nullptr, ImmuneComponentType::Infant);
  EXPECT_DOUBLE_EQ(comp.get_decay_rate(0), immune::K_INFANT_IMMUNE_DECAY_RATE);
  EXPECT_DOUBLE_EQ(comp.get_acquire_rate(0), 0.0);
  EXPECT_DOUBLE_EQ(comp.get_one_day_decay_factor(0), immune::K_ONE_DAY_INFANT_DECAY_FACTOR);
  EXPECT_DOUBLE_EQ(comp.get_one_day_acquire_factor(0), 1.0);
}

TEST(ImmuneComponentTest, SwitchToNonInfantPreservesValue) {
  ModelTestEnvironment model_environment;
  ImmuneComponent comp(nullptr, ImmuneComponentType::Infant);
  comp.set_latest_value(0.75);

  comp.switch_to_non_infant();

  EXPECT_EQ(comp.type(), ImmuneComponentType::NonInfant);
  EXPECT_DOUBLE_EQ(comp.latest_value(), 0.75);
}

TEST(ImmuneComponentTest, NonInfantOneDayFactorsUseProcessedConfigCache) {
  ModelTestEnvironment model_environment;
  ImmuneComponent comp;

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
