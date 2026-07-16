#ifndef EVENT_MANAGER_TEST_COMMON_H
#define EVENT_MANAGER_TEST_COMMON_H

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Core/Scheduler/EventManager.h"
#include "Events/Event.h"

using ::testing::_;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::StrictMock;

// Mock Event class using GMock
class MockEvent : public PersonEvent {
public:
  explicit MockEvent(const int time) : PersonEvent(nullptr) { set_time(time); }

  MOCK_METHOD(std::string_view, name, (), (const, noexcept, override));
  MOCK_METHOD(void, do_execute, (), (override));
  MOCK_METHOD(void, die, ());  // Helper method to track destruction

  ~MockEvent() override {
    die();  // Call mock method in destructor
  }

protected:
  friend class Event;  // Allow base class to access protected members
};

// Base test fixture class
class EventManagerTestBase : public ::testing::Test {
protected:
  EventManager<PersonEvent> event_manager;

  void SetUp() override { event_manager.initialize(); }

  void TearDown() override {}
};

#endif  // EVENT_MANAGER_TEST_COMMON_H

