#include "Population/ImmuneSystem/ImmuneComponent.h"
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Person/Person.h"
#include "gtest/gtest.h"

TEST(ImmuneSystemTest, ConstructionAndSetters) {
  ImmuneSystem immune;
  EXPECT_EQ(immune.person(), nullptr);
  Person person;
  immune.set_person(&person);
  EXPECT_EQ(immune.person(), &person);
}

TEST(ImmuneSystemTest, OwnsNonInfantComponentByDefault) {
  ImmuneSystem immune;
  ASSERT_NE(immune.immune_component(), nullptr);
  EXPECT_EQ(immune.immune_component()->immune_system(), &immune);
  EXPECT_EQ(immune.immune_component()->type(), ImmuneComponentType::NonInfant);
}

TEST(ImmuneSystemTest, SetLatestImmuneValue) {
  ImmuneSystem immune;
  immune.set_latest_immune_value(0.5);
  EXPECT_DOUBLE_EQ(immune.get_latest_immune_value(), 0.5);
}

TEST(ImmuneSystemTest, IncreaseFlag) {
  ImmuneSystem immune;
  EXPECT_FALSE(immune.increase());
}

TEST(ImmuneSystemTest, SwitchToNonInfantPreservesValue) {
  ImmuneSystem immune;
  immune.set_component_type(ImmuneComponentType::Infant);
  immune.set_latest_immune_value(0.25);

  immune.switch_to_non_infant();

  EXPECT_EQ(immune.immune_component()->type(), ImmuneComponentType::NonInfant);
  EXPECT_DOUBLE_EQ(immune.get_latest_immune_value(), 0.25);
}

// get_parasite_size_after_t_days and get_clinical_progression_probability require more mocks or
// integration
