#include "../../GrowLancer/compat/S21CastSpeedProducer.h"

using rise::growlancer::ComposeS21CastSpeedPair;
constexpr auto base = ComposeS21CastSpeedPair(100, 70, 0, 0, 0, false, false, 0);
static_assert(base.attack == 100 && base.magic == 70, "base words");
constexpr auto both = ComposeS21CastSpeedPair(100, 70, 11, 9, 9, false, false, 0);
static_assert(both.attack == 140 && both.magic == 110, "both bonuses plus priority flag");
constexpr auto capped = ComposeS21CastSpeedPair(100, 70, 11, 9, 9, true, true, 55);
static_assert(capped.attack == 55 && capped.magic == 55, "source tail order");
constexpr auto missing = ComposeS21CastSpeedPair(100, 70, 0, 0, 8, false, false, 0);
static_assert(missing.attack == 120 && missing.magic == 90, "fallback lookup zero");
