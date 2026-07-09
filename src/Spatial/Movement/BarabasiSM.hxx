/*
 * BarabasiSM.hxx
 *
 * Movement model based upon the radius of gyration distribution in
 * https://www.nature.com/articles/nature06958
 *
 * REMINDER Verify the correctness of the equation (2023-05-05)
 */
#ifndef SPATIAL_BARABASISM_HXX
#define SPATIAL_BARABASISM_HXX

#include <cmath>

#include "Spatial/SpatialModel.hxx"
#include "Utils/Helpers/NumberHelpers.h"

namespace Spatial {
class BarabasiSM : public SpatialModel {
public:
  // Disallow copy
  BarabasiSM(const BarabasiSM &) = delete;
  BarabasiSM &operator=(const BarabasiSM &) = delete;

  // Disallow move
  BarabasiSM(BarabasiSM &&) = delete;
  BarabasiSM &operator=(BarabasiSM &&) = delete;

  [[nodiscard]] double get_r_g_0() const { return r_g_0_; }

  void set_r_g_0(const double &value) { r_g_0_ = value; }

  [[nodiscard]] double get_beta_r() const { return beta_r_; }

  void set_beta_r(const double &value) { beta_r_ = value; }

  [[nodiscard]] double get_kappa() const { return kappa_; }

  explicit BarabasiSM(double r_g_0, double beta_r, double kappa)
      : r_g_0_(r_g_0), beta_r_(beta_r), kappa_(kappa) {}

  ~BarabasiSM() override = default;

  [[nodiscard]] std::vector<double> get_v_relative_out_movement_to_destination(
      const int &from_location, const int &number_of_locations,
      const std::vector<double> &relative_distance_vector,
      const std::vector<int> &v_number_of_residents_by_location) const override {
    std::vector<double> v_relative_number_of_circulation_by_location(number_of_locations, 0);
    for (int target_location = 0; target_location < number_of_locations; target_location++) {
      if (NumberHelpers::is_zero(relative_distance_vector[target_location])) {
        v_relative_number_of_circulation_by_location[target_location] = 0;
      } else {
        // P(r_g) = (r_g + r_g^0)^{-\beta_r}exp(\frac{-r_g}{\kappa})
        auto r_g = relative_distance_vector[target_location];
        v_relative_number_of_circulation_by_location[target_location] =
            std::pow((r_g + r_g_0_), -beta_r_) * std::exp(-r_g / kappa_);
      }
    }
    return v_relative_number_of_circulation_by_location;
  }

private:
  double r_g_0_;
  double beta_r_;
  double kappa_;
};
}  // namespace Spatial

#endif
