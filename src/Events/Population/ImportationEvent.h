#ifndef IMPORTATIONEVENT_H
#define IMPORTATIONEVENT_H

#include <vector>

// #include "Core/ObjectPool.h"
#include "Events/Event.h"

class Scheduler;

class ImportationEvent : public WorldEvent {
public:
  // Disallow copy
  ImportationEvent(const ImportationEvent &) = delete;
  ImportationEvent &operator=(const ImportationEvent &) = delete;

  // Disallow move
  ImportationEvent(ImportationEvent &&) = delete;
  ImportationEvent &operator=(ImportationEvent &&) = delete;

  // OBJECTPOOL(ImportationEvent)
  [[nodiscard]] int get_location() const { return location_; }
  [[nodiscard]] int get_genotype_id() const { return genotype_id_; }
  [[nodiscard]] int get_number_of_cases() const { return number_of_cases_; }

  explicit ImportationEvent(const int &location = -1,
                            const int &execute_at = -1,
                            const int &genotype_id = -1,
                            const int &number_of_cases = -1,
                            const std::vector<std::vector<double>> &allele_distributions =
                                std::vector<std::vector<double>>());

  //    ImportationEvent(const ImportationEvent& orig);
  ~ImportationEvent() override = default;

  static constexpr std::string_view EVENT_NAME{"introduce_parasites"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  int location_;
  int genotype_id_;
  int number_of_cases_;
  std::vector<std::vector<double>> allele_distributions_;

  void do_execute() override;
};

#endif /* IMPORTATIONEVENT_H */
