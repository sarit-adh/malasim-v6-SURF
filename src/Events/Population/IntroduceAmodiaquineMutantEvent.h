#pragma once

// #include "Core/ObjectPool.h"
#include <tuple>
#include <vector>

#include "Events/Event.h"

/**
 * @brief Event that introduces amodiaquine-resistant mutant parasites
 */
class IntroduceAmodiaquineMutantEvent : public WorldEvent {
public:
  // Disallow copy
  IntroduceAmodiaquineMutantEvent(const IntroduceAmodiaquineMutantEvent &) = delete;
  IntroduceAmodiaquineMutantEvent &operator=(const IntroduceAmodiaquineMutantEvent &) = delete;

  // Disallow move
  IntroduceAmodiaquineMutantEvent(IntroduceAmodiaquineMutantEvent &&) = delete;
  IntroduceAmodiaquineMutantEvent &operator=(IntroduceAmodiaquineMutantEvent &&) = delete;

  // OBJECTPOOL(IntroduceAQMutantEvent)
private:
  int location_;
  double fraction_;
  std::vector<std::tuple<int, int, char>> alleles_;

public:
  /**
   * @brief Constructs a new Introduce Amodiaquine Mutant Event
   * @param location The location to introduce the mutant
   * @param time The time at which to introduce the mutant
   * @param fraction The fraction of the population to convert
   * @param alleles The alleles to introduce
   */
  explicit IntroduceAmodiaquineMutantEvent(
      const int &location = -1,
      const int &execute_at = -1,
      const double &fraction = 0,
      const std::vector<std::tuple<int, int, char>> &alleles = {});

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroduceAmodiaquineMutantEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"introduce_amodiaquine_mutant"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};
