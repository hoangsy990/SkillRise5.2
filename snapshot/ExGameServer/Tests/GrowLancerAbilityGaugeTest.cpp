#include "../GameServer/RISE/GrowLancerAbilityGauge.h"

using rise::growlancer::CalcGrowLancerAbilityGauge;
constexpr double absDiff(double a, double b) { return a > b ? a - b : b - a; }

static_assert(absDiff(CalcGrowLancerAbilityGauge(25, 27, 32, 20), 38.75) < 1e-12);
static_assert(absDiff(CalcGrowLancerAbilityGauge(1, 1, 1, 1), 1.65) < 1e-12);
static_assert(absDiff(CalcGrowLancerAbilityGauge(150, 200, 300, 400), 552.5) < 1e-12);
static_assert(CalcGrowLancerAbilityGauge(0, 0, 0, 0) == 0);
