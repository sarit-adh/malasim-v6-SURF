#pragma once

#include <tuple>
#include <vector>

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

class IntroduceLumefantrineMutantEvent : public WorldEvent {
public:
  // Disallow copy
  IntroduceLumefantrineMutantEvent(const IntroduceLumefantrineMutantEvent &) = delete;
  IntroduceLumefantrineMutantEvent &operator=(const IntroduceLumefantrineMutantEvent &) = delete;

  // Disallow move
  IntroduceLumefantrineMutantEvent(IntroduceLumefantrineMutantEvent &&) = delete;
  IntroduceLumefantrineMutantEvent &operator=(IntroduceLumefantrineMutantEvent &&) = delete;

  // OBJECTPOOL(IntroduceLumefantrineMutantEvent)

private:
  int location_;
  double fraction_;
  std::vector<std::tuple<int, int, char>> alleles_;

public:
  explicit IntroduceLumefantrineMutantEvent(
      const int &location = -1,
      const int &execute_at = -1,
      const double &fraction = 0,
      const std::vector<std::tuple<int, int, char>> &alleles = {});

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceLumefantrineMutantEvent() override;

  static constexpr std::string_view EVENT_NAME{"introduce_lumefantrine_mutant"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};
