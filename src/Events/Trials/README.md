# Clinical Study Event

`ClinicalStudy` is a `WorldEvent` scaffold for simulating a clinical study. Its canonical event
name is `clinical_study`, returned as a `std::string_view` from a `noexcept` `name()` override.

## Current behavior

The event maintains:

- `enrollments_`: non-owning `Person*` entries and their elapsed study days.
- `observations_`: completed 28-day observations.
- `recrudescence_`: reserved count for recrudescence outcomes.

On execution, `do_execute()` calls `check_enrollees()` and `check_population()`.
`check_enrollees()` advances enrollment duration and counts observations at the 28-day boundary;
the target is 10,000 observations. Population enrollment and treatment-success/recrudescence
assessment are still TODOs, so this class does not yet implement protocol assignment, outcome
analysis, persistence, or automatic rescheduling.

## Event contract

`ClinicalStudy` is non-copyable and non-movable and is intended to be owned by the world-event
scheduler:

```cpp
auto study = std::make_unique<ClinicalStudy>();
study->set_time(start_day);
Model::get_scheduler()->schedule_population_event(std::move(study));
```

The `Person*` values held by enrollments are observers; `ClinicalStudy` does not own or delete
participants.
