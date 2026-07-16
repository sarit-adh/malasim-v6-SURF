# Events

This directory contains the event hierarchy and the person- and world-level events used by the
simulation.

## Event model

`Event` implements the non-virtual public interface: the scheduler calls `execute()`, which checks
`is_executable()` and then invokes the derived class's `do_execute()` hook. Every event also stores
its simulation day in `time_` through `get_time()` and `set_time()`.

Derived events use one of two marker base classes:

- `PersonEvent` stores a non-owning `Person*` and is scheduled on that person's event manager.
- `WorldEvent` is scheduled by the global `Scheduler` and can modify population or environment
  state.

Events are non-copyable and non-movable because ownership is transferred with
`std::unique_ptr`. A concrete event provides a stable, allocation-free name:

```cpp
static constexpr std::string_view EVENT_NAME{"my_event"};
[[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
```

Configuration builders also use `EVENT_NAME` as the YAML dispatch key, so a configurable event's
constant must match its `name:` value.

## Scheduling

World events are owned by the scheduler:

```cpp
auto event = std::make_unique<AnnualBetaUpdateEvent>(0.05F, start_day);
Model::get_scheduler()->schedule_population_event(std::move(event));
```

Person events are owned by the person's event manager:

```cpp
auto event = std::make_unique<ProgressToClinicalEvent>(person);
event->set_time(execute_at);
person->schedule_basic_event(std::move(event));
```

Do not allocate scheduled events with raw `new` or retain an owning pointer after scheduling.
Events that reschedule themselves must create a new `std::unique_ptr` instance.

## Contents

- Disease progression: `ProgressToClinicalEvent`, `MatureGametocyteEvent`,
  `MoveParasiteToBloodEvent`, and `EndClinicalEvent`.
- Treatment: `ReceiveTherapyEvent`, `ReceiveMDATherapyEvent`,
  `TestTreatmentFailureEvent`, `ReportTreatmentFailureDeathEvent`, and
  `UpdateWhenDrugIsPresentEvent`.
- Movement and lifecycle: `CirculateToTargetLocationNextDayEvent`,
  `ReturnToResidenceEvent`, `BirthdayEvent`, and `SwitchImmuneSystemModeEvent`.
- `Population/`: configurable population-wide interventions and importation events.
- `Environment/`: configurable environment events.
- `Trials/`: the clinical-study world event.

## Implementation checklist

- Override private `do_execute()` with the event action.
- Delete copy and move construction/assignment.
- Declare `EVENT_NAME` and return it from `name() const noexcept` as `std::string_view`.
- Use `[[nodiscard]]` on value-returning accessors that should not be ignored.
- Treat `Person*` and other model pointers as non-owning; use RAII for owned resources.
- Validate configuration in the builder before returning the event.
