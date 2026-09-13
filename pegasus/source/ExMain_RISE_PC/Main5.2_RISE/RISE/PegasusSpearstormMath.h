#pragma once

#include <cmath>

// Pure constants/envelopes from Pegasus 0x675C19..0x67623B.
namespace rise { namespace pegasus {

constexpr float kSpearstormRootLifetime = 5.0f;
constexpr float kSpearstormBladeLifetime = 72.0f;
constexpr float kSpearstormImpactLifetime = 66.0f;
constexpr float kSpearstormHitSoundLifetime = 64.0f;
constexpr float kSpearstormCrackLifetime = 50.0f;
constexpr float kSpearstormBladeSpeed = -200.0f;
constexpr float kSpearstormSpawnLeadTicks = 6.0f;
constexpr float kSpearstormBladeFadeRate = 0.02331f;
constexpr float kSpearstormBladeFullAlpha = 0.7f;
constexpr float kSpearstormModelScale = 0.5681818128f;
constexpr int kSpearstormStoneChildCount = 5;
constexpr int kSpearstormImpactSmokeCount = 15;
constexpr int kSpearstormImpactScatter = 40;

inline void SpearstormBladeDirection(float pitchDegrees, float yawDegrees,
    float* direction)
{
    constexpr float degreesToRadians = 0.01745329238474369f;
    const float pitch = pitchDegrees * degreesToRadians;
    const float yaw = yawDegrees * degreesToRadians;
    // Pegasus 0x675ECB..0x675F42 stores:
    // AF45F0 is SIN (fallback B0B47F -> fsin B0B49D), AF4050 is COS
    // (fallback B0AE9F -> fcos B0AEBD). Earlier recovery swapped them.
    // X=sin(yaw)*sin(pitch)*speed, Y=cos(pitch)*speed,
    // Z=cos(yaw)*sin(pitch)*speed. Preserve the source's axis order.
    const float radial = std::sin(pitch) * kSpearstormBladeSpeed;
    direction[0] = std::sin(yaw) * radial;
    direction[1] = std::cos(pitch) * kSpearstormBladeSpeed;
    direction[2] = std::cos(yaw) * radial;
}

inline float SpearstormLayerProgress(float remaining, float duration)
{
    return (kSpearstormCrackLifetime - remaining + 1.0f) / duration;
}

inline float SpearstormBladeAlpha(float blendMeshLight)
{
    const float alpha = blendMeshLight / kSpearstormBladeFullAlpha;
    return alpha < 0.0f ? 0.0f : alpha > 1.0f ? 1.0f : alpha;
}

inline bool SpearstormCrossesLifetime(float remaining, float animationFactor,
    float threshold)
{
    return remaining > threshold && remaining - animationFactor <= threshold;
}

inline bool SpearstormShouldImpact(float remaining, float animationFactor,
    float startZ, float nextZ)
{
    // Pegasus 0x67619A..0x6761B1: contact with StartPosition.z OR the
    // next remaining-life value reaching 66 forces the one-shot impact.
    return startZ >= nextZ ||
        remaining - animationFactor <= kSpearstormImpactLifetime;
}

}} // namespace rise::pegasus
