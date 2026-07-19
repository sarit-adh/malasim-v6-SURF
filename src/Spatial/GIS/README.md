# Geographic Information System (GIS)

This module implements the geographic information system components for spatial data management and analysis in the malaria simulation.

## Overview

The GIS module provides:

- Administrative level management
- Spatial data handling
- Geographic data processing
- ASC file operations
- Location mapping
- Interchangeable raster-grid distance storage

## Components

### Core Systems

- `AdminLevelManager`: Administrative hierarchy
  - Level definitions
  - Boundary management
  - Region relationships
  - Location mapping
  - Hierarchy traversal

### Spatial Data

- `SpatialData`: Geographic data management
  - Coordinate systems
  - Region definitions
  - Distance calculations
  - Area computations
  - Spatial relationships
- `DistanceProvider`: Common raster-grid distance interface
  - `DenseGridDistanceProvider` stores the legacy full distance matrix
  - `GridLutDistanceProvider` stores distances by absolute row/column delta
  - Both implementations expose `distance(from_location, to_location)` and `size()`
- `LocationPairTable`: Compact pair-value storage used by the LUT and derived movement kernels

### File Operations

- `AscFile`: ASC file handling
  - Grid data reading
  - Raster processing
  - Data validation
  - Format conversion
  - File I/O

## Implementation

### Admin Level Management

```cpp
class AdminLevelManager {
    void add_level(int level, string name);
    void map_locations(int level);
    vector<int> get_locations(int level);
    bool is_ancestor_of(int location1, int location2);
};
```

### Raster-Grid Distance Providers

```cpp
class DistanceProvider {
public:
  virtual ~DistanceProvider() = default;
  virtual double distance(std::size_t from_location,
                          std::size_t to_location) const noexcept = 0;
  virtual std::size_t size() const noexcept = 0;
};

enum class GridDistanceBackend { Dense, Lut };
```

`SpatialData::generate_distances()` creates the selected provider after raster locations have been
generated. Backend selection is deliberately source-controlled while equivalence is being verified:

```cpp
inline constexpr GridDistanceBackend GRID_DISTANCE_BACKEND = GridDistanceBackend::Lut;
```

There is no CMake option or YAML setting for this selection.

## Usage

### Administrative Management

```cpp
// Initialize admin levels
auto admin = new AdminLevelManager();
admin->add_level(0, "Country");
admin->add_level(1, "Province");

// Map locations
admin->map_locations(1);
auto locations = admin->get_locations(1);
```

### Comparing Raster Distances

```cpp
DenseGridDistanceProvider dense(locations, cell_size);
GridLutDistanceProvider lut(locations, cell_size);

for (std::size_t from = 0; from < locations.size(); ++from) {
  for (std::size_t to = 0; to < locations.size(); ++to) {
    assert(dense.distance(from, to) == lut.distance(from, to));
  }
}
```

The unit tests use `EXPECT_DOUBLE_EQ` for every location pair. Exact equality is required because the
LUT is intended to replace the dense raster matrix without changing movement probabilities or random
number streams.

## Distance Behavior by Spatial Mode

### Grid-Based Mode

Raster coordinates store integer row and column indices. Distance is Euclidean and depends only on
`(|delta_row|, |delta_column|)`, so an `N × N` dense matrix can be represented by a much smaller lookup
table. The current hard-coded production selection is `GridDistanceBackend::Lut`; changing the constant
to `Dense` selects the reference implementation on the next build. Both provider classes coexist in
each build, so tests can instantiate and compare them directly.

### Location-Based Mode

The grid LUT does not apply to location-based configurations. These locations contain arbitrary
latitude/longitude coordinates and continue to use the existing dense matrix populated with
`Spatial::Coordinate::calculate_distance_in_km`. A location-based configuration never constructs a
grid distance provider.

## Key Features

### Administrative Management

- Level hierarchy
- Location mapping
- Region relationships
- Boundary management
- Location queries

### Spatial Analysis

- Distance calculations
- Bit-identical dense/LUT raster distance implementations
- Neighbor finding
- Region containment
- Area calculations
- Coordinate transformations

### Data Processing

- Raster handling
- Grid operations
- Data validation
- Format conversion
- File management

## Dependencies

### Core Components

- `Location`
- `Coordinate`
- `Config`
- `Model`

### Support Systems

- File I/O
- Math utilities
- Data structures
- Error handling

## Notes

### Implementation Guidelines

- Validate spatial data
- Handle edge cases
- Check boundaries
- Verify relationships
- Document assumptions
- Test thoroughly
- Handle errors
- Log operations

### Performance Considerations

- Cache calculations
- Prefer the raster LUT for large grids to avoid quadratic distance storage
- Keep the dense raster provider as the numerical reference until replacement verification is complete
- Optimize queries
- Manage memory
- Handle large datasets
- Profile operations
- Validate inputs
- Clean up resources
- Document limitations
