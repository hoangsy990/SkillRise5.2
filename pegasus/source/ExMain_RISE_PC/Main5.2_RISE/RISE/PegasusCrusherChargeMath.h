#pragma once

#include <cmath>

// Pure Crusher Charge persistent-aura data recovered from Pegasus
// 0x65A46E..0x65B145. No source addresses or object layouts enter runtime.
namespace rise { namespace pegasus {

constexpr float kCrusherChargeAuraLifetime = 9999999.0f;
constexpr int kCrusherChargeAuraBoneCount = 17;
constexpr int kCrusherChargeAuraBones[kCrusherChargeAuraBoneCount] = {
    12, 17, 5, 10, 36, 27, 37, 28, 11, 35, 2, 3, 36, 20, 27, 4, 26
};

inline bool IsCrusherChargeHighPulseBone(int index)
{
    return index == 5 || index == 7 || index == 8 || index == 9 ||
        index == 12 || index == 14 || index == 15 || index == 16;
}

inline float CrusherChargeCenterPulse(float worldTime)
{
    return (std::sin(worldTime * 0.005f) + 1.0f) * 0.5f + 0.4f;
}

inline float CrusherChargeBonePulse(float worldTime, int index)
{
    if (index <= 2)
        return (std::sin(worldTime * 0.01f) + 1.0f) * 0.2f + 0.2f;
    if (IsCrusherChargeHighPulseBone(index))
        return (std::sin(worldTime * 0.005f) + 1.0f) * 0.25f + 0.5f;
    return (std::sin(worldTime * 0.01f) + 1.0f) * 0.3f + 0.3f;
}

}} // namespace rise::pegasus
