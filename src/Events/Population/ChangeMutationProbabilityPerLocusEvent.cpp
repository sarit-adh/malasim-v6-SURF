//
// Created by kient on 8/22/2023.
//

#include "ChangeMutationProbabilityPerLocusEvent.h"

#include "Configuration/Config.h"
#include "Simulation/Model.h"

ChangeMutationProbabilityPerLocusEvent::ChangeMutationProbabilityPerLocusEvent(const double &value,
                                                                               const int &at_time)
    : value{value} {
  set_time(at_time);
}
void ChangeMutationProbabilityPerLocusEvent::do_execute() {
  Model::get_config()->get_genotype_parameters().set_mutation_probability_per_locus(value);
  spdlog::info("{}: Change mutation probability per locus to {}",
               Model::get_scheduler()->get_current_date_string(), value);
}
