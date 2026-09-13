#pragma once

// Pure Havoc Spear envelopes recovered from Pegasus 0x665D50..0x666365.
// No foreign addresses, object layouts or numeric resource IDs are carried
// into the runtime adapter.
namespace rise { namespace pegasus {

constexpr float kHavocNovaLifetime = 20.0f;
constexpr float kHavocLineLifetime = 30.0f;
constexpr float kHavocShockwaveLifetime = 12.0f;
constexpr float kHavocEndLifetime = 24.0f;
constexpr float kHavocNovaModelHeight = 234.0f;
constexpr float kHavocLinePlaySpeed = 1.0f;
constexpr float kHavocShockwavePlaySpeed = 0.3f;
constexpr int kHavocRootLastTick = 15;

struct HavocRootJointSpec
{
    int bitmapRole;
    float scaleFactor;
    int subtype;
    float red;
    float green;
    float blue;
};

constexpr int kHavocSomasiJointRole = 0;
constexpr int kHavocAdJointRole = 1;
constexpr HavocRootJointSpec kHavocRootJointSpecs[6] = {
    { kHavocSomasiJointRole, 2.7f, 0, 0.06f, 0.15f, 1.0f },
    { kHavocSomasiJointRole, 2.5f, 1, 0.05f, 0.15f, 0.7f },
    { kHavocSomasiJointRole, 2.0f, 0, 0.25f, 0.35f, 0.7f },
    { kHavocAdJointRole, 6.5f, 0, 0.3f, 0.75f, 1.0f },
    { kHavocAdJointRole, 6.5f, 0, 0.3f, 0.75f, 1.0f },
    { kHavocAdJointRole, 6.5f, 0, 0.3f, 0.75f, 1.0f }
};

inline float HavocTargetScale(float targetScale)
{
    if (targetScale < 0.8f) return 0.8f;
    return targetScale > 2.5f ? 2.5f : targetScale;
}

inline float HavocRootJointScale(const HavocRootJointSpec& spec)
{
    return spec.scaleFactor *
        (spec.bitmapRole == kHavocAdJointRole ? 9.0f : 12.0f);
}

inline float HavocClamp01(float value)
{
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

inline float HavocLifetimeAlpha(float remaining, float initial)
{
    return HavocClamp01(initial > 0.0f ? remaining / initial : 0.0f);
}

inline float HavocNovaHeight(float terrain, float scale, float remaining,
    float initial = kHavocNovaLifetime)
{
    const float rise = HavocClamp01((initial - remaining) / 5.0f);
    return terrain + 10.0f - (1.0f - rise) * scale * kHavocNovaModelHeight;
}

inline float HavocShockwaveScaleStep(float currentScale, float initialScale,
    float frameStep)
{
    return currentScale + initialScale * 0.5f * frameStep;
}

inline float HavocEndAlpha(float remaining)
{
    return HavocLifetimeAlpha(remaining, kHavocEndLifetime * 0.75f);
}

// Pegasus 0x666162..0x6661AA: tick 1 creates the main radial burst and then
// falls through to the same Nova/explosion child used by every later odd tick.
inline bool HavocCreatesRootBurstAtTick(int tick)
{
    return tick == 1;
}

inline bool HavocCreatesOddBurstAtTick(int tick)
{
    return tick >= 1 && tick <= kHavocRootLastTick && (tick & 1) != 0;
}

}} // namespace rise::pegasus
