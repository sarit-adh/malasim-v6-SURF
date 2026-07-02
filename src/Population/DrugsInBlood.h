#ifndef DRUGSINBLOOD_H
#define    DRUGSINBLOOD_H

#include <map>
#include <memory>

#include "Treatment/Therapies/Drug.h"

class Person;

class Event;

class DrugType;

using DrugPtrMap = std::map<int, std::unique_ptr<Drug>>;

class DrugsInBlood {
  // OBJECTPOOL(DrugsInBlood)
  // Disallow copy
  DrugsInBlood(const DrugsInBlood&) = delete;
  DrugsInBlood& operator=(const DrugsInBlood&) = delete;

  // Disallow move
  DrugsInBlood(DrugsInBlood&&) = delete;
  DrugsInBlood& operator=(DrugsInBlood&&) = delete;

 private:
  Person *person_{nullptr};
  DrugPtrMap drugs_{};

 public:
  // Iterator type definitions for proxy access
  using iterator = DrugPtrMap::iterator;
  using const_iterator = DrugPtrMap::const_iterator;

  // Iterator proxy methods
  iterator begin() { return drugs_.begin(); }
  iterator end() { return drugs_.end(); }
  const_iterator begin() const { return drugs_.begin(); }
  const_iterator end() const { return drugs_.end(); }
  const_iterator cbegin() const { return drugs_.cbegin(); }
  const_iterator cend() const { return drugs_.cend(); }

  // Map-like proxy methods
  Drug* at(const int& key) const { return drugs_.at(key).get(); }
  bool contains(const int& key) const { return drugs_.find(key) != drugs_.end(); }
  
  Person *person() const {
    return person_;
  }
  void set_person(Person *value) {
    person_ = value;
  }


 public:
  explicit DrugsInBlood(Person *person = nullptr);

  //    DrugsInBlood(const DrugsInBlood& orig);
  virtual ~DrugsInBlood();

  void init();

  Drug *add_drug(std::unique_ptr<Drug> drug);

  std::size_t size() const;

  void clear();

  void update();

  void clear_cut_off_drugs();

};

#endif    /* DRUGSINBLOOD_H */
