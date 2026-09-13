#pragma once

#include <cmath>

namespace rise { namespace pegasus {

constexpr int kSwordWrathRootLifetime = 200;
constexpr int kSwordWrathLayerPulseLifetime = 200;
constexpr int kSwordWrathGroundStarLifetime = 30;
constexpr int kSwordWrathWaveLifetime = 40;
constexpr int kSwordWrathLineLifetime = 20;
constexpr int kSwordWrathSmokeLifetime = 20;
constexpr int kSwordWrathPinStarLifetime = 30;
constexpr int kSwordWrathSmokeEmitThreshold = 160;
constexpr int kSwordWrathAtlasFrameCount = 15;
constexpr int kSwordWrathAtlasColumns = 4;

constexpr float kSwordWrathSmokeScaleSplit = 14.0f;
constexpr float kSwordWrathSmokeScaleDelta = 0.75f;
constexpr float kSwordWrathPinStarScaleDelta = 0.1f;
constexpr float kSwordWrathPinStarRise = 10.0f;
constexpr float kSwordWrathPinStarInputScale = 0.45f;
constexpr float kSwordWrathAtlasCell = 0.25f;
constexpr float kSwordWrathLineVelocityFactor = 0.06f;
constexpr int kSwordWrathLineTailMin = 5;
constexpr int kSwordWrathLineTailMax = 10;

constexpr float kSwordWrathRootScale = 0.9f;
constexpr float kSwordWrathIronScale = 2.0f;
constexpr float kSwordWrathIronInnerScale = 1.5f;
constexpr float kSwordWrathGroundStarScale = 0.75f;
constexpr float kSwordWrathWaveScale = 2.0f;

struct SwordWrathColor
{
    float r;
    float g;
    float b;
};

constexpr SwordWrathColor kSwordWrathRedLight = { 1.0f, 0.2f, 0.19f };
constexpr SwordWrathColor kSwordWrathParticleLight = { 1.0f, 0.15f, 0.05f };
constexpr SwordWrathColor kSwordWrathWaveLight = { 1.0f, 0.158f, 0.119f };

inline float SwordWrathPulse(int lifetime, int period, float depth)
{
    const int tick = ((lifetime > 0 ? lifetime : 1) - 1) % period + 1;
    return 1.0f - std::fabs(static_cast<float>(tick) * 0.1f - 1.0f) * depth;
}

inline float SwordWrathGroundStarAlphaDelta(int lifetime, float animationFactor)
{
    return (lifetime > 15 ? 0.06666667f : -0.06666667f) * animationFactor;
}

inline float SwordWrathGroundStarScaleDelta(int lifetime, float animationFactor)
{
    return (lifetime > 15 ? 0.1f : -0.1f) * animationFactor;
}

inline bool ShouldEmitSwordWrathAccent(bool isRoot, float lifetime,
    float animationFactor)
{
    return isRoot && std::sin(lifetime + animationFactor) > std::sin(lifetime);
}

inline int SwordWrathAtlasFrame(float lifetime)
{
    const int tick = lifetime > 0.0f ? static_cast<int>(lifetime) : 0;
    return tick % kSwordWrathAtlasFrameCount;
}

inline float SwordWrathAtlasU(float lifetime)
{
    return static_cast<float>(SwordWrathAtlasFrame(lifetime) %
        kSwordWrathAtlasColumns) * kSwordWrathAtlasCell;
}

inline float SwordWrathAtlasV(float lifetime)
{
    return static_cast<float>(SwordWrathAtlasFrame(lifetime) /
        kSwordWrathAtlasColumns) * kSwordWrathAtlasCell;
}

inline bool IsSwordWrathRenderAction(int action)
{
    return action == 0x43 || action == 0x9D || action == 0x9E ||
        action == 0x9F;
}

inline float SwordWrathLineLightFactor(float lifetime)
{
    if (lifetime <= 0.0f) return 0.0f;
    if (lifetime >= static_cast<float>(kSwordWrathLineLifetime)) return 1.0f;
    return lifetime / static_cast<float>(kSwordWrathLineLifetime);
}

}}
