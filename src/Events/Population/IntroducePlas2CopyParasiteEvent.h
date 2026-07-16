#pragma once

#include <tuple>
#include <vector>

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

/**
 * @brief Event that introduces parasites with two copies of plasmepsin
 */
class IntroducePlas2CopyParasiteEvent : public WorldEvent {
private:
  int location_;
  double fraction_;
  std::vector<std::tuple<int, int, char>> alleles_;

  void do_execute() override;

public:
  // Disallow copy
  IntroducePlas2CopyParasiteEvent(const IntroducePlas2CopyParasiteEvent &) = delete;
  IntroducePlas2CopyParasiteEvent &operator=(const IntroducePlas2CopyParasiteEvent &) = delete;

  // Disallow move
  IntroducePlas2CopyParasiteEvent(IntroducePlas2CopyParasiteEvent &&) = delete;
  IntroducePlas2CopyParasiteEvent &operator=(IntroducePlas2CopyParasiteEvent &&) = delete;

  // OBJECTPOOL(IntroducePlas2CopyParasiteEvent)

  /**
   * @brief Constructs a new Introduce Plas2 Copy Parasite Event
   * @param location The location to introduce the parasites
   * @param execute_at The time at which to introduce the parasites
   * @param fraction The fraction of the population to convert
   * @param allele_map The alleles to introduce
   */
  explicit IntroducePlas2CopyParasiteEvent(
      const int &location = -1,
      const int &execute_at = -1,
      const double &fraction = 0.0,
      const std::vector<std::tuple<int, int, char>> &allele_map = {});

  //    ImportationEvent(const ImportationEvent& orig);
  ~IntroducePlas2CopyParasiteEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"introduce_plas2_copy_parasite"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }
};
