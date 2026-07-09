// ImmuneSystemConstants.h
#pragma once

#include <cmath>

#include "Core/types.h"

namespace immune {
inline constexpr core::Age K_MAX_IMMUNE_AGE_INDEX = 80;
inline constexpr double K_IMMUNE_VALUE_CUTOFF = 1e-5;
constexpr double K_INFANT_IMMUNE_DECAY_RATE = 0.0315;
const double K_ONE_DAY_INFANT_DECAY_FACTOR = std::exp(-K_INFANT_IMMUNE_DECAY_RATE);

}  // namespace immune
