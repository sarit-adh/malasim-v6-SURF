#ifndef EVENT_H
#define EVENT_H

#include <string_view>

class Event {
public:
  // Disallow copy
  Event(const Event &) = delete;
  Event &operator=(const Event &) = delete;

  // Disallow move
  Event(Event &&) = delete;
  Event &operator=(Event &&) = delete;

  Event() = default;
  virtual ~Event() = default;

  // Public interface
  void execute();  // Non-virtual public interface (Template Method)
  [[nodiscard]] virtual std::string_view name() const noexcept = 0;

  // Public state management
  [[nodiscard]] bool is_executable() const { return executable_; }
  void set_executable(bool value) { executable_ = value; }

  [[nodiscard]] int get_time() const { return time_; }
  void set_time(int value) { time_ = value; }

protected:
  // Protected interface for derived classes
  virtual void do_execute() = 0;  // Hook method for derived classes

private:
  bool executable_{false};
  int time_{-1};
};

class Person;

class PersonEvent : public Event {
public:
  explicit PersonEvent(Person* person) : person_(person) {}

  [[nodiscard]] Person* get_person() const { return person_; }
  void set_person(Person* person) { person_ = person; }

private:
  Person* person_;
};

class WorldEvent : public Event {};

#endif  // EVENT_H

