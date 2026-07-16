#ifndef CHANGETREATMENTCOVERAGEEVENT_H
#define CHANGETREATMENTCOVERAGEEVENT_H

#include <memory>

#include "Events/Event.h"
#include "Treatment/ITreatmentCoverageModel.h"

class ChangeTreatmentCoverageEvent : public WorldEvent {
public:
  std::unique_ptr<ITreatmentCoverageModel> treatment_coverage_model;

  ChangeTreatmentCoverageEvent(const ChangeTreatmentCoverageEvent &) = delete;
  ChangeTreatmentCoverageEvent(ChangeTreatmentCoverageEvent &&) = delete;
  ChangeTreatmentCoverageEvent &operator=(const ChangeTreatmentCoverageEvent &) = delete;
  ChangeTreatmentCoverageEvent &operator=(ChangeTreatmentCoverageEvent &&) = delete;
  explicit ChangeTreatmentCoverageEvent(std::unique_ptr<ITreatmentCoverageModel> tcm);

  ~ChangeTreatmentCoverageEvent() override;

  static constexpr std::string_view EVENT_NAME{"change_treatment_coverage"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  void do_execute() override;
};

#endif  // CHANGETREATMENTCOVERAGEEVENT_H
