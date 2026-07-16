#include "EventManagerTestCommon.h"

class CancellationTest : public EventManagerTestBase {};

TEST_F(CancellationTest, CancelSingleEvent) {
  auto event_ptr = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto* raw_event_ptr = event_ptr.get();  // Get the raw pointer BEFORE moving

  // Event should be destroyed but not executed
  EXPECT_CALL(*event_ptr, do_execute()).Times(0);
  EXPECT_CALL(*event_ptr, die()).Times(1);

  event_manager.schedule_event(std::move(event_ptr));  // Move ownership
  EXPECT_FALSE(event_manager.get_events().empty());

  // cancel the event simply marks it as non-executable
  event_manager.cancel_event(raw_event_ptr);  // Use the saved raw pointer
  EXPECT_FALSE(event_manager.get_events().empty());

  // execute the event
  event_manager.execute_events(10);
  EXPECT_TRUE(event_manager.get_events().empty());
}

TEST_F(CancellationTest, CancelEventsByType) {
  auto event1 = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto event2 = std::make_unique<testing::StrictMock<MockEvent>>(15);
  auto event3 = std::make_unique<testing::StrictMock<MockEvent>>(20);

  // Events should be destroyed but not executed
  EXPECT_CALL(*event1, do_execute()).Times(0);
  EXPECT_CALL(*event1, die()).Times(1);
  EXPECT_CALL(*event2, do_execute()).Times(0);
  EXPECT_CALL(*event2, die()).Times(1);
  EXPECT_CALL(*event3, do_execute()).Times(0);
  EXPECT_CALL(*event3, die()).Times(1);

  event_manager.schedule_event(std::move(event1));
  event_manager.schedule_event(std::move(event2));
  event_manager.schedule_event(std::move(event3));

  EXPECT_EQ(event_manager.get_events().size(), 3);
  event_manager.cancel_all_events<MockEvent>();
  EXPECT_FALSE(event_manager.get_events().empty());

  // execute the event
  event_manager.execute_events(20);
  EXPECT_TRUE(event_manager.get_events().empty());
}

TEST_F(CancellationTest, CancelEventBeforeExecution) {
  auto event1_ptr = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto event2_ptr = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto* raw_event1_ptr = event1_ptr.get();  // Get raw pointer for event1

  // event1 should be destroyed but not executed
  EXPECT_CALL(*event1_ptr, do_execute()).Times(0);
  EXPECT_CALL(*event1_ptr, die()).Times(1);

  // event2 should be executed and destroyed normally
  EXPECT_CALL(*event2_ptr, do_execute()).Times(1);
  EXPECT_CALL(*event2_ptr, die()).Times(1);

  event_manager.schedule_event(std::move(event1_ptr));  // Move event1
  event_manager.schedule_event(std::move(event2_ptr));  // Move event2

  event_manager.cancel_event(raw_event1_ptr);  // Use saved raw pointer for event1
  event_manager.execute_events(10);
  EXPECT_TRUE(event_manager.get_events().empty());
}

TEST_F(CancellationTest, CancelEventAfterScheduling) {
  auto event_ptr = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto* raw_event_ptr = event_ptr.get();  // Get the raw pointer BEFORE moving

  // Event should be marked as non-executable but not executed
  EXPECT_CALL(*event_ptr, do_execute()).Times(0);
  EXPECT_CALL(*event_ptr, die()).Times(1);

  event_manager.schedule_event(std::move(event_ptr));  // Move ownership
  EXPECT_FALSE(event_manager.get_events().empty());

  // cancel the event simply marks it as non-executable
  event_manager.cancel_event(raw_event_ptr);  // Use the saved raw pointer
  EXPECT_FALSE(event_manager.get_events().empty());

  // execute the event
  event_manager.execute_events(10);
  EXPECT_TRUE(event_manager.get_events().empty());
}

TEST_F(CancellationTest, CancelNonexistentEvent) {
  auto event = std::make_unique<testing::StrictMock<MockEvent>>(10);

  // Event should be destroyed but not executed since it was never scheduled
  EXPECT_CALL(*event, do_execute()).Times(0);
  EXPECT_CALL(*event, die()).Times(1);

  // Cancel event that was never scheduled
  event_manager.cancel_event(event.get());
  EXPECT_TRUE(event_manager.get_events().empty());

  // Execute events to ensure nothing unexpected happens
  event_manager.execute_events(10);
  EXPECT_TRUE(event_manager.get_events().empty());
}

TEST_F(CancellationTest, CancelEventsByTypeWithMultipleTypes) {
  class OtherEvent : public PersonEvent {
  public:
    explicit OtherEvent(int time) : PersonEvent(nullptr) { set_time(time); }

    MOCK_METHOD(std::string_view, name, (), (const, noexcept, override));
    MOCK_METHOD(void, do_execute, (), (override));
    MOCK_METHOD(void, die, ());  // Helper method to track destruction

    ~OtherEvent() override {
      die();  // Call mock method in destructor
    }
  };

  auto mock_event = std::make_unique<testing::StrictMock<MockEvent>>(10);
  auto other_event = std::make_unique<testing::StrictMock<OtherEvent>>(15);

  // MockEvent should be destroyed but not executed
  EXPECT_CALL(*mock_event, do_execute()).Times(0);
  EXPECT_CALL(*mock_event, die()).Times(1);

  // OtherEvent should be executed and destroyed normally
  EXPECT_CALL(*other_event, do_execute()).Times(1);
  EXPECT_CALL(*other_event, die()).Times(1);

  event_manager.schedule_event(std::move(mock_event));
  event_manager.schedule_event(std::move(other_event));

  // cancel just marks the event as non-executable but does not remove it
  event_manager.cancel_all_events<MockEvent>();
  EXPECT_FALSE(event_manager.get_events().empty());

  // execute the event
  event_manager.execute_events(15);
  EXPECT_TRUE(event_manager.get_events().empty());
}

