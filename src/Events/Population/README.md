# Population Events

Population events are configuration-built `WorldEvent` instances for importation, mutation,
treatment policy, transmission, movement, and mass drug administration.

## Construction and ownership

Each configuration item has a `name` and an `info` sequence. `PopulationEventBuilder::build()`
compares `name` with each event class's `static constexpr EVENT_NAME`, validates and converts the
entries, and returns `std::vector<std::unique_ptr<WorldEvent>>`.

```cpp
auto events = PopulationEventBuilder::build(event_node);
for (auto &event : events) {
  Model::get_scheduler()->schedule_population_event(std::move(event));
}
```

In normal startup this is handled by `PopulationEvents` and `Model`: configuration owns the built
events until `release_events()`, after which the scheduler takes ownership. Event names are exposed
without allocation:

```cpp
static constexpr std::string_view EVENT_NAME{"change_treatment_strategy"};
[[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
```

## Supported configuration names

### Importation

- `introduce_parasites`
- `introduce_parasites_periodically`
- `introduce_parasites_periodically_v2`
- `importation_periodically_random`
- `district_importation_daily`

### Mutation and parasite variants

- `introduce_plas2_copy_parasite`
- `introduce_amodiaquine_mutant`
- `introduce_lumefantrine_mutant`
- `introduce_580Y_mutant`
- `introduce_triple_mutant_to_dpm`
- `introduce_mutant_event`
- `introduce_mutant_raster`
- `turn_on_mutation`
- `turn_off_mutation`
- `change_mutation_mask`
- `change_mutation_probability_per_locus`
- `change_within_host_induced_free_recombination`

See `EventBuilders/README.md` for the administrative-unit and raster mutant schemas.

### Treatment and intervention

- `change_treatment_coverage`
- `change_treatment_strategy`
- `single_round_MDA`
- `modify_nested_mft_strategy`
- `rotate_treatment_strategy`

### Transmission and movement

- `annual_beta_update`
- `annual_coverage_update`
- `change_circulation_percent`
- `change_interrupted_feeding_rate`
- `update_beta_raster_event`

## Examples

```yaml
population_events:
  - name: change_treatment_strategy
    info:
      - date: 2005/06/02
        strategy_id: 3

  - name: single_round_MDA
    info:
      - date: 2006/01/01
        fraction_population_targeted: [1.0]
        days_to_complete_all_treatments: 14

  - name: introduce_parasites
    info:
      - location: 0
        parasite_info:
          - date: 2006/03/20
            genotype_aa_sequence: "||||YY1||KTHFIMG,x||||||FNCMYRIPRPYRA|1"
            number_of_cases: 5
```

Fields are event-specific. Date fields are converted relative to the simulation start date, and
builders validate constraints such as location, strategy, fraction, duration, and raster shape
before the event is scheduled.

## Implementation notes

- Recurring events schedule a newly allocated event with `std::unique_ptr`; they do not reschedule
  the executing object.
- `IntroduceMutantEventBase` provides shared fraction calculation and mutation logic to the
  administrative-unit and raster events.
- `PopulationEventBuilder.h/.cpp` contains the main dispatch and builders;
  `EventBuilders/IntroduceMutantEventBuilder.cpp` contains spatial mutant builders.
