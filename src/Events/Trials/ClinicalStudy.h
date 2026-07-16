/**
 * ClinicalStudy.h
 *
 * This class controls how a clinical study is simulated using the model.
 */
#ifndef CLINICALSTUDY_H
#define CLINICALSTUDY_H

#include <vector>

#include "../Event.h"

class Scheduler;
class Person;

class ClinicalStudy : public WorldEvent {
private:
  static constexpr int OBSERVATION_PERIOD = 28;
  static constexpr int TOTAL_OBSERVATIONS = 10000;

  struct Enrollee {
    Person* person = nullptr;
    int days = 0;
  };

  int observations_ = 0;
  int recrudescence_ = 0;
  std::vector<Enrollee> enrollments_;

public:
  ClinicalStudy() = default;
  ClinicalStudy(const ClinicalStudy &) = delete;
  ClinicalStudy(ClinicalStudy &&) = delete;
  ClinicalStudy &operator=(const ClinicalStudy &) = delete;
  ClinicalStudy &operator=(ClinicalStudy &&) = delete;
  explicit ClinicalStudy(std::vector<Enrollee> enrollments)
      : enrollments_(std::move(enrollments)) {}
  ~ClinicalStudy() override = default;

  static constexpr std::string_view EVENT_NAME{"clinical_study"};
  [[nodiscard]] std::string_view name() const noexcept override { return EVENT_NAME; }

private:
  // Triggered by the scheduler
  void do_execute() override;

  // Check persons that have already been enrolled in the study at T+28
  // to determine if they have cleared the parasite or not.
  //
  // Returns true if the study is complete, false otherwise.
  bool check_enrollees();

  // Check the population for new individuals to enroll.
  void check_population();
};

#endif
