#ifndef TREATMENTSELECTION_H
#define TREATMENTSELECTION_H

class Therapy;

enum class TreatmentSector { Public, Private };

struct TreatmentSelection {
  Therapy* therapy;
  TreatmentSector sector;

  [[nodiscard]] bool is_public() const { return sector == TreatmentSector::Public; }
};

#endif  // TREATMENTSELECTION_H
