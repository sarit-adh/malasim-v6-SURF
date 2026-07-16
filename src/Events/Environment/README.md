# Environment Events

Environment events are configuration-built `WorldEvent` instances that modify environmental
state while the simulation is running.

## Supported event

`UpdateEcozoneEvent` changes all seasonality entries using one ecozone identifier to another. Its
canonical event name is `update_ecozone`; `EnvironmentEventBuilder::build()` dispatches directly
against `UpdateEcozoneEvent::EVENT_NAME`.

```yaml
- name: update_ecozone
  info:
    - day: 2005/01/01
      from: 1
      to: 2
```

`day` is converted to a simulation-day offset from the configured starting date. Both `from` and
`to` must be non-negative.

## Builder API

```cpp
std::vector<std::unique_ptr<WorldEvent>> events =
    EnvironmentEventBuilder::build(event_node);

for (auto &event : events) {
  Model::get_scheduler()->schedule_population_event(std::move(event));
}
```

The builder returns an empty vector for an unsupported name. During normal configuration loading,
`PopulationEvents` first tries `PopulationEventBuilder` and then this builder. Scheduled events are
owned by the scheduler.

`UpdateEcozoneEvent` follows the shared event-name contract:

```cpp
static constexpr std::string_view EVENT_NAME{"update_ecozone"};
[[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
```

## Files

- `EnvironmentEventBuilder.h/.cpp`: YAML dispatch, date conversion, and validation.
- `UpdateEcozoneEvent.hxx`: execution logic and seasonality update.
