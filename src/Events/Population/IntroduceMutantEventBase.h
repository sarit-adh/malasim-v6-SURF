/**
 * IntroduceMutantEventBase.h
 *
 * Base class for introduce mutant event.
 */
#ifndef INTRODUCEMUTANTEVENTBASE_H
#define INTRODUCEMUTANTEVENTBASE_H

#include <vector>

// #include "Core/PropertyMacro.h"
#include "Events/Event.h"

class IntroduceMutantEventBase : public WorldEvent {
public:
  IntroduceMutantEventBase(const double &fraction,
                           const std::vector<std::tuple<int, int, char>> &alleles)
      : fraction(fraction), alleles(alleles) {}

  double calculate(std::vector<int> &locations) const;
  int mutate(std::vector<int> &locations, double target_fraction) const;

protected:
  double fraction;
  std::vector<std::tuple<int, int, char>> alleles;
};

#endif
