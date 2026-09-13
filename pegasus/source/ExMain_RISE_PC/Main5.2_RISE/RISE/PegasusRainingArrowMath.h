#pragma once

#include <cmath>

namespace rise { namespace pegasus {

constexpr int kRainingArrowFollowLifetime = 16;
constexpr int kRainingArrowDamageSpriteCount = 2;
constexpr int kRainingArrowDamageAtlasColumns = 4;
constexpr int kRainingArrowDamageAtlasFrames = 16;
// Exact first-volley angle vector reconstructed from the stack at Pegasus
// 0x6720C6..0x6722C0: {270, Random(-10, 10), 180}.
constexpr float kRainingArrowFirstPitch = 270.0f;
constexpr float kRainingArrowFirstYaw = 180.0f;
constexpr float kRainingArrowFirstBankMin = -10.0f;
constexpr float kRainingArrowFirstBankMax = 10.0f;
constexpr float kRainingArrowVolleyOffsetMin = -200.0f;
constexpr float kRainingArrowVolleyOffsetMax = 200.0f;
constexpr float kRainingArrowVolleyHeight = 500.0f;

inline float RainingArrowPulse(float lifeTime, float duration)
{
    // Pegasus helper 0x670D6A: clamp(1-abs(duration-2*life)/duration, 0, 1).
    if (duration <= 0.0f)
        return 0.0f;
    const float pulse = 1.0f - std::fabs(duration - 2.0f * lifeTime) /
        duration;
    if (pulse < 0.0f) return 0.0f;
    return pulse > 1.0f ? 1.0f : pulse;
}

inline float RainingArrowVolleyPitch(float launchZ, float targetZ,
    float horizontalDistance)
{
    // Pegasus 0x672228..0x672267: atan((launchZ-targetZ)/distance)*180/pi,
    // with the exact <=1 distance fallback of 90 degrees.
    if (horizontalDistance <= 1.0f)
        return 90.0f;
    const float slope = (launchZ - targetZ) / horizontalDistance;
    return std::atan(slope) * (180.0f / 3.14159265358979323846f);
}

inline int RainingArrowDamageFrame(float lifeTime, float distance)
{
    int frame = static_cast<int>(distance - lifeTime);
    if (frame < 0) frame = 0;
    if (frame >= kRainingArrowDamageAtlasFrames)
        frame = kRainingArrowDamageAtlasFrames - 1;
    return frame;
}

inline float RainingArrowDamageLight(float lifeTime, float distance)
{
    if (distance <= 0.0f) return 0.0f;
    const float light = lifeTime / distance;
    if (light < 0.0f) return 0.0f;
    return light > 1.0f ? 1.0f : light;
}

inline float RainingArrowDamageScale(int randomSample)
{
    if (randomSample < 0) randomSample = 0;
    if (randomSample > 1000) randomSample = 1000;
    return (1.0f + static_cast<float>(randomSample) * 0.001f) * 1.8f;
}

}}
