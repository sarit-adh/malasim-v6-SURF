#include "SwitchImmuneComponentEvent.h"

#include <cassert>
#include "Population/ImmuneSystem/ImmuneSystem.h"
#include "Population/Person/Person.h"

// OBJECTPOOL_IMPL(SwitchImmuneComponentEvent)

SwitchImmuneComponentEvent::SwitchImmuneComponentEvent(Person* person) : PersonEvent(person) {
  if (person == nullptr) {
    spdlog::error("SwitchImmuneComponentEvent::SwitchImmuneComponentEvent, person is nullptr");
    throw std::invalid_argument(
        "SwitchImmuneComponentEvent::SwitchImmuneComponentEvent, person is nullptr");
  }
}

SwitchImmuneComponentEvent::~SwitchImmuneComponentEvent() = default;

void SwitchImmuneComponentEvent::do_execute() {
  auto* person = get_person();
  if (person == nullptr) {
    spdlog::error("SwitchImmuneComponentEvent::do_execute, person is nullptr");
    throw std::invalid_argument("SwitchImmuneComponentEvent::do_execute, person is nullptr");
  }
  person->get_immune_system()->switch_to_non_infant();
}
