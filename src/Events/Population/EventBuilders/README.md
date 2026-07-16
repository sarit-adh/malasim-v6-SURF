# Mutant Event Builders

`IntroduceMutantEventBuilder.cpp` contains the two specialized
`PopulationEventBuilder` implementations that introduce selected alleles by administrative unit or
binary raster. It does not define a separate stateful builder class.

## Administrative-unit event

The `introduce_mutant_event` form requires a top-level `admin_level` and one or more entries with
`day`, `unit_id`, `fraction`, and `alleles`:

```yaml
- name: introduce_mutant_event
  admin_level: district
  info:
    - day: 2005/01/01
      unit_id: 4
      fraction: 0.20
      alleles:
        - chromosome: 13
          locus: 11
          allele: Y
```

The builder resolves the configured administrative level through `SpatialData`, checks the unit
identifier, converts the date to simulation time, and returns an `IntroduceMutantEvent`.

## Raster event

The `introduce_mutant_raster` form selects locations from an ESRI ASCII raster:

```yaml
- name: introduce_mutant_raster
  info:
    - date: 2005/01/01
      raster: mutation_targets.asc
      fraction: 0.20
      alleles:
        - chromosome: 13
          locus: 11
          allele: Y
```

Valid raster cells must contain only `0` or `1`; nodata cells are ignored. The number of mapped
cells must match the configured location count, and the fraction must be greater than zero.

## Ownership and errors

Both functions return `std::vector<std::unique_ptr<WorldEvent>>`. Callers transfer those pointers
to the scheduler. YAML conversion failures and invalid spatial input are logged; invalid unit,
allele, fraction, or raster data are rejected before scheduling.
