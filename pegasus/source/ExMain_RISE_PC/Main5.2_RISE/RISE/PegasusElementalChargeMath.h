#pragma once

#include <cmath>

// Exact five-layer table read from the hash-guarded Pegasus x86 image:
// rates 0xB6B1EC, bones 0xB6B200, source bitmap roles 0xB6B214.
// Runtime bitmap IDs remain isolated RISE IDs; only the ordered roles cross.
namespace rise { namespace pegasus {

constexpr int kElementalLayerCount = 5;
constexpr int kElementalCastSubtype = 0;
constexpr int kElementalBuffFallbackSubtype = 1;
constexpr int kElementalConstructorChildCount = 5;
constexpr int kElementalSourceFlare03Effect = 0x7F20;
constexpr int kElementalSourceShockwaveEffect = 0x7F21;
constexpr int kElementalConstructorChildRoles[kElementalConstructorChildCount] = {
    kElementalSourceFlare03Effect,
    kElementalSourceFlare03Effect,
    kElementalSourceFlare03Effect,
    kElementalSourceShockwaveEffect,
    kElementalSourceShockwaveEffect
};
constexpr int kElementalConstructorChildSubtypes[kElementalConstructorChildCount] = {
    0, 0, 0, 1, 1
};
constexpr float kElementalConstructorChildScales[kElementalConstructorChildCount] = {
    3.0f, 3.0f, 4.0f, 1.5f, 1.5f
};
constexpr float kElementalFlare03Lifetime = 50.0f;
constexpr float kElementalShockwaveLifetime = 40.0f;
constexpr float kElementalFlare03AlphaDecay = 0.0133333337f;
constexpr float kElementalShockwaveScale = 1.5f;
constexpr float kElementalShockwaveGrowthStart = 0.1f;
constexpr int kElementalBones[kElementalLayerCount] = { 3, 6, 9, 12, 15 };
constexpr float kElementalPulseRates[kElementalLayerCount] = {
    0.009f, 0.008f, 0.007f, 0.006f, 0.005f
};
constexpr int kElementalSourceBitmapRoles[kElementalLayerCount] = {
    0x7F22, 0x7F23, 0x7F24, 0x7F25, 0x7F26
};

inline float ElementalPulse(float worldTime, int layer)
{
    return std::fabs(std::sin(worldTime * kElementalPulseRates[layer]));
}

inline bool ShouldCreateElementalBuffFallback(bool hasCastRoot)
{
    return !hasCastRoot;
}

}} // namespace rise::pegasus
