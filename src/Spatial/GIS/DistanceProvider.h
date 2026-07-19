/*
 * DistanceProvider.h
 *
 * Interchangeable distance storage for raster-grid locations. Location-based
 * configurations use their existing haversine distance matrix and do not use
 * these providers.
 */
#ifndef SPATIAL_GIS_DISTANCEPROVIDER_H
#define SPATIAL_GIS_DISTANCEPROVIDER_H

#include <cmath>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Spatial/GIS/LocationPairTable.h"
#include "Spatial/Location/Location.h"

class DistanceProvider {
public:
  virtual ~DistanceProvider() = default;

  [[nodiscard]] virtual double distance(std::size_t from_location,
                                        std::size_t to_location) const noexcept = 0;
  [[nodiscard]] virtual std::size_t size() const noexcept = 0;
};

class DenseGridDistanceProvider final : public DistanceProvider {
public:
  DenseGridDistanceProvider(const std::vector<Spatial::Location> &locations, float cell_size)
      : distances_(locations.size(), std::vector<double>(locations.size())) {
    for (std::size_t from = 0; from < locations.size(); ++from) {
      for (std::size_t to = 0; to < locations.size(); ++to) {
        // Keep this expression identical to the original raster-grid distance
        // calculation. Its operand types and evaluation order are part of the
        // equivalence contract with GridLutDistanceProvider.
        distances_[from][to] = std::sqrt(std::pow(cell_size
                                                      * (locations[from].coordinate.latitude
                                                         - locations[to].coordinate.latitude),
                                                  2)
                                         + std::pow(cell_size
                                                        * (locations[from].coordinate.longitude
                                                           - locations[to].coordinate.longitude),
                                                    2));
      }
    }
  }

  [[nodiscard]] double distance(std::size_t from_location,
                                std::size_t to_location) const noexcept override {
    return distances_[from_location][to_location];
  }

  [[nodiscard]] std::size_t size() const noexcept override { return distances_.size(); }

  [[nodiscard]] const std::vector<std::vector<double>> &matrix() const noexcept {
    return distances_;
  }

private:
  std::vector<std::vector<double>> distances_;
};

class GridLutDistanceProvider final : public DistanceProvider {
public:
  GridLutDistanceProvider(const std::vector<Spatial::Location> &locations, float cell_size)
      : distances_(LocationPairTable::make_grid_distances(locations, cell_size)) {}

  [[nodiscard]] double distance(std::size_t from_location,
                                std::size_t to_location) const noexcept override {
    return distances_.at(static_cast<int>(from_location), static_cast<int>(to_location));
  }

  [[nodiscard]] std::size_t size() const noexcept override { return distances_.size(); }

  [[nodiscard]] std::size_t memory_bytes() const noexcept { return distances_.memory_bytes(); }

  [[nodiscard]] const LocationPairTable &table() const noexcept { return distances_; }

private:
  LocationPairTable distances_;
};

enum class GridDistanceBackend { Dense, Lut };

// Raster distance selection is intentionally a source-level decision while the
// two implementations are being verified. It is not a build or YAML option.
inline constexpr GridDistanceBackend GRID_DISTANCE_BACKEND = GridDistanceBackend::Lut;

[[nodiscard]] inline std::unique_ptr<DistanceProvider> make_grid_distance_provider(
    const std::vector<Spatial::Location> &locations,
    float cell_size,
    GridDistanceBackend backend = GRID_DISTANCE_BACKEND) {
  switch (backend) {
    case GridDistanceBackend::Dense:
      return std::make_unique<DenseGridDistanceProvider>(locations, cell_size);
    case GridDistanceBackend::Lut:
      return std::make_unique<GridLutDistanceProvider>(locations, cell_size);
  }
  throw std::logic_error("Unknown raster-grid distance backend");
}

#endif  // SPATIAL_GIS_DISTANCEPROVIDER_H
