
#include <gtest/gtest.h>

#include "RandomTestBase.h"
#include "helpers/test_helpers.h"

// Test that random_gamma throws when shape is non-positive
TEST_F(RandomTest, random_gamma_ThrowsWhenShapeNonPositive) {
  double shape = -1.0;  // Invalid shape
  double scale = 2.0;
  EXPECT_THROW(rng.random_gamma(shape, scale), std::invalid_argument);
}

// Test that random_gamma throws when scale is negative
TEST_F(RandomTest, random_gamma_ThrowsWhenScaleNegative) {
  double shape = 2.0;
  double scale = -3.0;  // Invalid scale
  EXPECT_THROW(rng.random_gamma(shape, scale), std::invalid_argument);
}

// Range Validation Tests

// Test that random_gamma returns shape when scale is zero
TEST_F(RandomTest, random_gamma_ReturnsShapeWhenScaleZero) {
  double shape = 4.0;
  double scale = 0.0;
  double result = rng.random_gamma(shape, scale);
  EXPECT_DOUBLE_EQ(result, shape);
}

// Test that random_gamma returns values within (0, ∞) when scale is not zero
TEST_F(RandomTest, random_gamma_ReturnsValuesWithinRange) {
  double shape = 2.0;
  double scale = 3.0;
  int sample_size = 10000;

  for (int i = 0; i < sample_size; ++i) {
    double val = rng.random_gamma(shape, scale);
    EXPECT_GT(val, 0.0);
  }
}

// Statistical Mean and Variance Tests

// Test that the sample mean is close to the theoretical mean of the Gamma
// distribution
TEST_F(RandomTest, random_gamma_SampleMeanCloseToTheoretical) {
  double shape = 3.0;
  double scale = 2.0;
  int sample_size = 100000;
  double epsilon = 0.05;  // Acceptable error margin

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double theoretical_mean = shape * scale;

  EXPECT_NEAR(sample_mean, theoretical_mean, epsilon);
}

// Test that the sample variance is close to the theoretical variance of the
// Gamma distribution
TEST_F(RandomTest, random_gamma_SampleVarianceCloseToTheoretical) {
  double shape = 3.0;
  double scale = 2.0;
  int sample_size = 100000;
  double epsilon = 1;  // theoretical variance is about 12

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double sample_variance = calculate_variance(samples, sample_mean);
  double theoretical_variance = shape * scale * scale;

  EXPECT_NEAR(sample_variance, theoretical_variance, epsilon);
}

// Edge Case Tests

// Test with alpha and scale equal to 1 (Exponential distribution)
TEST_F(RandomTest, random_gamma_GammaAlpha1Scale1_ExponentialDistribution) {
  double shape = 1.0;
  double scale = 1.0;
  int sample_size = 100000;
  double epsilon_mean = 0.05;
  double epsilon_variance = 0.1;

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double sample_variance = calculate_variance(samples, sample_mean);
  double theoretical_mean = shape * scale;
  double theoretical_variance = shape * scale * scale;

  EXPECT_NEAR(sample_mean, theoretical_mean, epsilon_mean);
  EXPECT_NEAR(sample_variance, theoretical_variance, epsilon_variance);
}

// Test with large alpha and scale values
TEST_F(RandomTest, random_gamma_LargeAlphaScaleValues) {
  double shape = 50.0;
  double scale = 2.0;
  int sample_size = 100000;
  double epsilon_mean = 0.5;
  double epsilon_variance = 10.0;  // variance is about 200

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double sample_variance = calculate_variance(samples, sample_mean);
  double theoretical_mean = shape * scale;
  double theoretical_variance = shape * scale * scale;

  EXPECT_NEAR(sample_mean, theoretical_mean, epsilon_mean);
  EXPECT_NEAR(sample_variance, theoretical_variance, epsilon_variance);
}

// Test with shape much larger than scale
TEST_F(RandomTest, random_gamma_ShapeMuchLargerThanScale) {
  double shape = 10.0;
  double scale = 1.0;
  int sample_size = 100000;
  double epsilon_mean = 0.1;
  double epsilon_variance = 0.5;  // var is about 10

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double sample_variance = calculate_variance(samples, sample_mean);
  double theoretical_mean = shape * scale;
  double theoretical_variance = shape * scale * scale;

  EXPECT_NEAR(sample_mean, theoretical_mean, epsilon_mean);
  EXPECT_NEAR(sample_variance, theoretical_variance, epsilon_variance);
}

// Test with scale much larger than shape
TEST_F(RandomTest, random_gamma_ScaleMuchLargerThanShape) {
  constexpr double shape = 2.0;
  constexpr double scale = 10.0;
  constexpr int sample_size = 100000;

  std::vector<double> samples(sample_size);
  for (int i = 0; i < sample_size; ++i) { samples[i] = rng.random_gamma(shape, scale); }

  double sample_mean = calculate_mean(samples);
  double sample_variance = calculate_variance(samples, sample_mean);
  double theoretical_mean = shape * scale;
  double theoretical_variance = shape * scale * scale;

  double epsilon_mean = 4.0 * std::sqrt(theoretical_variance / sample_size);
  double epsilon_variance = 5.0 * std::sqrt(2.0);  // rough for this specific case

  EXPECT_NEAR(sample_mean, theoretical_mean, epsilon_mean);
  EXPECT_NEAR(sample_variance, theoretical_variance, epsilon_variance);
}
