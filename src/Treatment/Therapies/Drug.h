#ifndef DRUG_H
#define    DRUG_H

class DrugType;
class DrugsInBlood;

class Drug {
    // OBJECTPOOL(Drug)

    // Disallow copy
    Drug(const Drug&) = delete;
    Drug& operator=(const Drug&) = delete;

    // Disallow move
    Drug(Drug&&) = delete;
    Drug& operator=(Drug&&) = delete;

private:
    int dosing_days_;
    int start_time_;
    int end_time_;
    double last_update_value_;
    int last_update_time_;
    double starting_value_;
    DrugType *drug_type_;
    DrugsInBlood *person_drugs_;

public:
    [[nodiscard]] int dosing_days() const {
        return dosing_days_;
    }
    void set_dosing_days(int value) {
        dosing_days_ = value;
    }
    [[nodiscard]] int start_time() const {
        return start_time_;
    }
    void set_start_time(int value) {
        start_time_ = value;
    }
    [[nodiscard]] int end_time() const {
        return end_time_;
    }
    void set_end_time(int value) {
        end_time_ = value;
    }
    [[nodiscard]] double last_update_value() const {
        return last_update_value_;
    }
    void set_last_update_value(double value) {
        last_update_value_ = value;
    }
    [[nodiscard]] int last_update_time() const {
        return last_update_time_;
    }
    void set_last_update_time(int value) {
        last_update_time_ = value;
    }
    [[nodiscard]] double starting_value() const {
        return starting_value_;
    }
    void set_starting_value(double value) {
        starting_value_ = value;
    }
    DrugType *drug_type() const {
        return drug_type_;
    }
    void set_drug_type(DrugType *value) {
        drug_type_ = value;
    }
    DrugsInBlood *person_drugs() const {
        return person_drugs_;
    }
    void set_person_drugs(DrugsInBlood *value) {
        person_drugs_ = value;
    }
 public:
  explicit Drug(DrugType *drug_type = nullptr);

  //    Drug(const Drug& orig);
  virtual ~Drug();

  void update();

  double get_current_drug_concentration(int currentTime);

  double get_mutation_probability() const;

  double get_mutation_probability(double currentDrugConcentration) const;

  void set_number_of_dosing_days(int dosingDays);

  double get_parasite_killing_rate(const int &genotype_id) const;
};

#endif    /* DRUG_H */
