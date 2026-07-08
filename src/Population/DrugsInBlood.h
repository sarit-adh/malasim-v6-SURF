#ifndef DRUGSINBLOOD_H
#define DRUGSINBLOOD_H

#include <memory>
#include <unordered_map>

#include "Treatment/Therapies/Drug.h"

class Person;

class Event;

class DrugType;

using DrugPtrMap = std::unordered_map<int, std::unique_ptr<Drug>>;

class DrugsInBlood {
public:
  // OBJECTPOOL(DrugsInBlood)
  // Disallow copy
  DrugsInBlood(const DrugsInBlood &) = delete;
  DrugsInBlood &operator=(const DrugsInBlood &) = delete;

  // Disallow move
  DrugsInBlood(DrugsInBlood &&) = delete;
  DrugsInBlood &operator=(DrugsInBlood &&) = delete;

  // Iterator type definitions for proxy access
  using Iterator = DrugPtrMap::iterator;
  using ConstIterator = DrugPtrMap::const_iterator;

  // Iterator proxy methods
  Iterator begin() { return drugs_.begin(); }
  Iterator end() { return drugs_.end(); }
  [[nodiscard]] ConstIterator begin() const { return drugs_.begin(); }
  [[nodiscard]] ConstIterator end() const { return drugs_.end(); }
  [[nodiscard]] ConstIterator cbegin() const { return drugs_.cbegin(); }
  [[nodiscard]] ConstIterator cend() const { return drugs_.cend(); }

  // Map-like proxy methods
  [[nodiscard]] Drug* at(const int &key) const { return drugs_.at(key).get(); }
  [[nodiscard]] bool contains(const int &key) const { return drugs_.contains(key); }

  [[nodiscard]] Person* person() const { return person_; }
  void set_person(Person* value) { person_ = value; }

  explicit DrugsInBlood(Person* person = nullptr);

  //    DrugsInBlood(const DrugsInBlood& orig);
  virtual ~DrugsInBlood();

  void init();

  Drug* add_drug(std::unique_ptr<Drug> drug);

  [[nodiscard]] std::size_t size() const;

  void clear();

  void update();

  void clear_cut_off_drugs();

private:
  Person* person_{nullptr};
  DrugPtrMap drugs_;
};

#endif /* DRUGSINBLOOD_H */
