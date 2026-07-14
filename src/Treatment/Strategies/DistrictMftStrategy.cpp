/*
 * DistrictMftStrategy.cpp
 *
 * Implement the district MFT strategy.
 *
 * NOTE that there may be a memory leak present in the district_strategies
 * object, but since the object is going to be in scope for the entire expected
 * lifecycle of the simulation it's not a pressing concern.
 */
#include "DistrictMftStrategy.h"

#include "Population/Person/Person.h"
#include "Simulation/Model.h"
#include "Spatial/GIS/SpatialData.h"
#include "Utils/Random.h"

// TODO : fix for 0-1 based indexing
DistrictMftStrategy::DistrictMftStrategy() : IStrategy("DistrictMFT", StrategyType::DistrictMft) {
  // Size the map to accommodate either 0-based or 1-based district IDs
  // Pre-populate map with nullptr entries for all possible district IDs
  auto vector_size = Model::get_spatial_data()->get_boundary("district")->max_unit_id + 1;
  for (int i = 0; i < vector_size; i++) { district_strategies_[i] = nullptr; }
}

void DistrictMftStrategy::add_therapy(Therapy* therapy) {
  throw std::runtime_error("Invalid function called to add therapy to the District MFT Strategy.");
}

void DistrictMftStrategy::set_district_strategy(int district,
                                                std::unique_ptr<MftStrategy> strategy) {
  // Validate district ID is within bounds
  if (district < 0 || district >= district_strategies_.size()) {
    throw std::out_of_range(fmt::format("District ID {} is out of valid range [0, {}]", district,
                                        district_strategies_.size() - 1));
  }

  // Check if district already has a strategy assigned
  if (district_strategies_[district] != nullptr) {
    throw std::runtime_error(
        fmt::format("District {} already has an MFT strategy assigned", district));
  }

  // Move the unique_ptr to our map
  district_strategies_[district] = std::move(strategy);
}

Therapy* DistrictMftStrategy::get_therapy(Person* person) {
  // Resolve the MFT for this district
  auto district = Model::get_spatial_data()->get_admin_unit("district", person->get_location());
  auto* mft = district_strategies_[district].get();

  // Select the therapy to give the individual
  auto pr = Model::get_random()->random_flat(0.0, 1.0);
  auto sum = 0.0;
  for (auto ndx = 0; ndx < mft->percentages.size(); ndx++) {
    sum += mft->percentages[ndx];
    if (pr <= sum) { return Model::get_therapy_db()[mft->therapies[ndx]].get(); }
  }

  // Since we should ways return above, throw an error if we get here
  throw std::runtime_error("Scanned for therapy without finding a match: " + this->name
                           + ", district: " + std::to_string(district)
                           + ", pr: " + std::to_string(pr));
}
