#include "../Main5.2_RISE/RISE/PegasusCrusherChargeMath.h"
#include <cmath>
#include <cstdio>

namespace {
bool Near(float left, float right)
{
    return std::fabs(left - right) <= 0.00001f;
}
}

int main()
{
    using namespace rise::pegasus;
    static_assert(kCrusherChargeAuraLifetime == 9999999.0f,
        "Crusher persistent lifetime drifted");
    static_assert(kCrusherChargeAuraBoneCount == 17,
        "Crusher aura bone count drifted");
    const int expected[17] = {
        12, 17, 5, 10, 36, 27, 37, 28, 11, 35, 2, 3, 36, 20, 27, 4, 26
    };
    for (int index = 0; index < 17; ++index)
        if (kCrusherChargeAuraBones[index] != expected[index]) return 1;
    if (!Near(CrusherChargeCenterPulse(0.0f), 0.9f) ||
        !Near(CrusherChargeBonePulse(0.0f, 0), 0.4f) ||
        !Near(CrusherChargeBonePulse(0.0f, 5), 0.75f) ||
        !Near(CrusherChargeBonePulse(0.0f, 3), 0.6f) ||
        !IsCrusherChargeHighPulseBone(12) ||
        IsCrusherChargeHighPulseBone(13)) return 2;
    std::puts("PASS: Crusher Charge persistent aura, 17 bones and pulse groups");
    return 0;
}
