/**
 * ClinicalStudy.cpp
 *
 * Implement the clinical study class.
 */
#include "ClinicalStudy.h"

bool ClinicalStudy::check_enrollees() {
  // Scan each of the enrollees in turn
  for (auto enrollee : enrollments_) {
    // Update how many days they've been enrolled
    enrollee.days++;

    // Are they at the end of the observation period?
    if (enrollee.days < OBSERVATION_PERIOD) { continue; }

    // TODO Was the treatment successful?
    // Presuming deceased means that treatment failed

    observations_++;
  }

  // If we have enough observations, return true
  return (observations_ >= TOTAL_OBSERVATIONS);
}

void ClinicalStudy::check_population() {
  // TODO Scan the newly clinical population

  // TODO Enroll them in the study
}

void ClinicalStudy::do_execute() {
  check_enrollees();
  check_population();
}
