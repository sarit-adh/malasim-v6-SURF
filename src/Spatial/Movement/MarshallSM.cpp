#include "Spatial/Movement/MarshallSM.hxx"

#include <utility>

#include "Simulation/Model.h"

Spatial::MarshallSM::MarshallSM(double tau, double alpha, double log_rho,
                               int number_of_locations,
                               std::vector<std::vector<double>> spatial_distance_matrix)
    : tau_(tau),
      alpha_(alpha),
      log_rho_(log_rho),
      number_of_locations_(number_of_locations),
      spatial_distance_matrix_(std::move(spatial_distance_matrix)) {
  if (!spatial_distance_matrix_.empty()) {
    constructor_distance_lut_ = LocationPairTable::make_dense(spatial_distance_matrix_);
  }
}

Spatial::MarshallSM::~MarshallSM() { release_dense_kernel(); }

void Spatial::MarshallSM::release_dense_kernel() {
  if (kernel == nullptr) { return; }
  for (int ndx = 0; ndx < number_of_locations_; ++ndx) { delete[] kernel[ndx]; }
  delete[] kernel;
  kernel = nullptr;
}

void Spatial::MarshallSM::prepare_kernel() {
  kernel_lut_ = LocationPairTable{};
  const LocationPairTable* distances = nullptr;

  if (!constructor_distance_lut_.empty()) {
    distances = &constructor_distance_lut_;
  } else if (Model::get_config() != nullptr) {
    const auto &configured =
        Model::get_config()->get_spatial_settings().get_spatial_distance_table();
    if (!configured.empty()) { distances = &configured; }
  }

  if (distances == nullptr) { return; }

  const double log_rho = log_rho_;
  const double alpha = alpha_;
  kernel_lut_ = distances->map_with_zero_sentinel(
      [log_rho, alpha](double distance) {
        return std::pow(1.0 + (distance / log_rho), -alpha);
      },
      "MarshallSM kernel");
}

void Spatial::MarshallSM::prepare() { prepare_kernel(); }

DoubleVector Spatial::MarshallSM::get_v_relative_out_movement_to_destination(
    const int &from_location, const int &number_of_locations,
    const DoubleVector &relative_distance_vector,
    const IntVector &v_number_of_residents_by_location) const {
  const double source_population_power =
      std::pow(v_number_of_residents_by_location[from_location], tau_);
  DoubleVector results(number_of_locations, 0.0);

  if (kernel_lut_.size() == static_cast<size_t>(number_of_locations)) {
    const auto kernel_row = kernel_lut_.row_view(from_location);
    for (int destination = 0; destination < number_of_locations; ++destination) {
      const double kernel_value = kernel_row[static_cast<size_t>(destination)];
      if (NumberHelpers::is_zero(kernel_value)) { continue; }
      results[destination] = source_population_power * kernel_value;
    }
    return results;
  }

  // Compatibility fallback when a standalone object has no configured LUT.
  if (relative_distance_vector.size() < static_cast<size_t>(number_of_locations)) {
    throw std::runtime_error(fmt::format(
        "MarshallSM called without a prepared distance LUT or compatibility distance row"));
  }
  for (int destination = 0; destination < number_of_locations; ++destination) {
    const double distance = relative_distance_vector[destination];
    if (NumberHelpers::is_zero(distance)) { continue; }
    results[destination] = source_population_power
                           * std::pow(1.0 + (distance / log_rho_), -alpha_);
  }

  return results;
}
