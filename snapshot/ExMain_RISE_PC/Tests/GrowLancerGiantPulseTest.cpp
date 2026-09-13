#include "../../GrowLancer/compat/S21GiantPulse.h"
#include <cstdio>
#include <limits>
#include <cstdlib>

static void Require(bool value) { if (!value) std::abort(); }
int main()
{
    using namespace rise::growlancer;
    Require(GiantPulse(0.0f) == 0.0f);
    Require(GiantPulse(0.9f) == 0.0f); // Signed truncation, not rounding.
    Require(GiantPulse(1570.0f) == 0.0f);
    Require(GiantMeshPulse(0.0f) == 0.4f);
    Require(GiantPulse(1.0f) == GiantPulse(1571.0f));
    Require(GiantPulse(-1.0f) == GiantPulse(1.0f));
    Require(GiantPulse(2147483648.0f) == GiantPulse(-2147483648.0f));
    Require(GiantPulse(std::numeric_limits<float>::infinity()) == GiantPulse(-2147483648.0f));
    Require(GiantPulse(std::numeric_limits<float>::quiet_NaN()) == GiantPulse(-2147483648.0f));
    for (int tick = 0; tick < 1570; ++tick)
    {
        const float value = GiantMeshPulse(static_cast<float>(tick));
        Require(value >= .4f && value <= 1.0f);
    }
    std::puts("PASS fixed Giant pulse boundaries and 1570 ticks; not S21 CRT bit parity or renderer QA");
}
