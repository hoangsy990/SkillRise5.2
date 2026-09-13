#pragma once
#include <cmath>

// Reconstructed from Pegasus 5.2.3.0, SHA256 02922A0C...DEA0AA.
// Pure behavior only: no foreign addresses, layouts or resource IDs.
// Integration with native OBJECT/PARTICLE ownership remains separate.
namespace rise { namespace pegasus {

constexpr float kSwordBlowModelPlayDivisor = 6.0f;

struct SwordBlowLight
{
    float red;
    float green;
    float blue;
};

// Guarded SS21 runtime trace authority.  Keep these values separate from the
// old Pegasus constructor defaults: 39 root casts / 936 blades proved that
// 0x6A2, 0x6A3 and 0x6A4 all receive the same neutral object light.  Their
// blue-white appearance is produced by the recovered bright/material passes,
// not by tinting the blade or plane object itself.
constexpr SwordBlowLight kSwordBlowNeutralModelLight = {0.2f, 0.2f, 0.2f};
// The old 5.2 blend-mesh path consumes RenderMesh's fifth argument as the
// additive material multiplier. This bounded transfer comes from the RISE
// Visual Studio compatibility renderer, not an exact SS21 constant; it is
// separate from the neutral controller/object light above.
// 5.2's additive pass is more exposed than the SS21 capture. The measured
// RGB ratio is applied through BodyLight and this bounded gain prevents the
// blue-violet tail from clipping back to white.
constexpr float kSwordBlowSmallSwordBrightPassTransfer = 0.65f;
constexpr float kSwordBlowFlySmallSwordBrightPassTransfer = 0.32f;
// Measured from the SS21 owner frame (2026-09-10): median RGB of the
// brightest blue-violet sword-tail pixels was (143,155,233). Keep the image
// sample separate from the neutral constructor light and the material gain.
constexpr SwordBlowLight kSwordBlowFlySmallSwordImageLight =
    {143.0f / 255.0f, 155.0f / 255.0f, 233.0f / 255.0f};
// Image-derived candidate, NOT an SS21 constructor constant. Reference video
// frames31/61 minus frame1 (ROI800,560..1250,880, unsaturated blue deltas)
// measured R/B .392/.395 and G/B .479/.473. Local before/after ONE/ONE draw
// measured .61278/.66441 for the previous143/233,155/233 material, confirming
// that this native shader/texture passes the ratios to framebuffer deltas.
// Use the midpoint candidate, preserving blue energy. Full scene parity and
// exact reference registration still require validation; do not label PASS.
constexpr SwordBlowLight kSwordBlowTailReferenceDeltaRatio = {0.394f, 0.476f, 1.0f};
constexpr SwordBlowLight SwordBlowSmallSwordTailLight(float blue)
{
    return {blue * kSwordBlowTailReferenceDeltaRatio.red,
        blue * kSwordBlowTailReferenceDeltaRatio.green, blue};
}
constexpr SwordBlowLight kSwordBlowPlanLight = {0.7f, 0.7f, 1.0f};
constexpr SwordBlowLight kSwordBlowMultishotLight = {0.4f, 0.5f, 1.2f};
constexpr SwordBlowLight kSwordBlowMultishot3Light = {0.35f, 0.4f, 0.48f};
constexpr SwordBlowLight kSwordBlowLightningLight = {0.3f, 0.6f, 1.0f};
constexpr SwordBlowLight kSwordBlowFlareLight = {0.65f, 0.7f, 0.9f};

// Custom PLAYER_ATTACK_ONETOONE_FLY choreography requested for the isolated
// Sword Blow upgrade. The player model receives a separately allocated,
// re-keyed action; this curve supplies only the world-space jump arc. XY and
// server position remain untouched so the old lunge-to-target regression
// cannot return.
constexpr unsigned kSwordBlowFlightDurationMs = 1500;
constexpr float kSwordBlowFlightHeight = 150.0f;
constexpr float kSwordBlowFlightCrouchEnd = 0.12f;
constexpr float kSwordBlowFlightTakeoffEnd = 0.34f;
constexpr float kSwordBlowFlightDiveStart = 0.55f;
constexpr float kSwordBlowFlightLandingStart = 0.82f;
// Live held-mouse trace on the isolated client measured ~1.55 s between
// accepted casts. Stay airborne longer than that cadence so the next cast
// refreshes hover instead of landing and immediately taking off again.
constexpr unsigned kSwordBlowFlightHoldAfterCastMs = 1900;
constexpr unsigned kSwordBlowFlightLandingDurationMs = 450;
constexpr float kSwordBlowFlightWingOpacity = 0.58f;
// BloodCastle01 mesh 3 is fully skinned to its model bone 45.  Vertices 8/12
// are the coincident left/right roots where the two planes meet, unlike the
// rejected geometric-centre anchor which moved the wings beside the player.
constexpr int kSwordBlowWingModelBone = 45;
constexpr float kSwordBlowWingRootX = 2.85384768f;
constexpr float kSwordBlowWingRootY = 8.55563068f;
constexpr float kSwordBlowWingRootZ = -24.29822159f;
// The airborne thrust is a deliberately quicker replay than the complete
// takeoff clip. Network acceptance remains governed by the native skill delay.
constexpr float kSwordBlowFlyThrustSpeedMultiplier = 1.35f;
constexpr float kSwordBlowFlyThrustStartFrame = 4.0f;

inline float SwordBlowFlightPlaySpeed(float sourcePlaySpeed,
    int sourceKeys, int flightKeys)
{
    if (sourcePlaySpeed <= 0.0f || sourceKeys <= 1 || flightKeys <= 1)
        return sourcePlaySpeed;
    // Preserve the native one-to-one action duration even though the custom
    // choreography contains more keys.
    return sourcePlaySpeed * static_cast<float>(flightKeys - 1) /
        static_cast<float>(sourceKeys - 1) *
        kSwordBlowFlyThrustSpeedMultiplier;
}

inline float SwordBlowSmoothStep(float value)
{
    if (value < 0.0f) value = 0.0f;
    if (value > 1.0f) value = 1.0f;
    return value * value * (3.0f - 2.0f * value);
}

inline float SwordBlowFlightHeightAt(float progress)
{
    if (progress <= 0.0f || progress >= 1.0f) return 0.0f;
    if (progress < kSwordBlowFlightCrouchEnd)
        return 0.0f;
    if (progress < kSwordBlowFlightTakeoffEnd)
        return kSwordBlowFlightHeight * SwordBlowSmoothStep(
            (progress - kSwordBlowFlightCrouchEnd) /
            (kSwordBlowFlightTakeoffEnd - kSwordBlowFlightCrouchEnd));
    if (progress < kSwordBlowFlightDiveStart)
        return kSwordBlowFlightHeight;
    if (progress < kSwordBlowFlightLandingStart)
        return kSwordBlowFlightHeight - 55.0f * SwordBlowSmoothStep(
            (progress - kSwordBlowFlightDiveStart) /
            (kSwordBlowFlightLandingStart - kSwordBlowFlightDiveStart));
    return 95.0f * (1.0f - SwordBlowSmoothStep(
        (progress - kSwordBlowFlightLandingStart) /
        (1.0f - kSwordBlowFlightLandingStart)));
}

inline float SwordBlowFlightWingAlphaAt(float progress)
{
    if (progress <= 0.0f || progress >= 1.0f) return 0.0f;
    if (progress < 0.18f)
        return kSwordBlowFlightWingOpacity * SwordBlowSmoothStep(progress / 0.18f);
    if (progress < 0.72f)
        return kSwordBlowFlightWingOpacity;
    return kSwordBlowFlightWingOpacity * (1.0f - SwordBlowSmoothStep(
        (progress - 0.72f) / 0.28f));
}

// 0x6779E5: field4C / field98 form a symmetric, squared fade envelope.
inline float SwordBlowOpacity(float remainingLifetime, float initialLifetime)
{
    const float duration = initialLifetime > 1.0f ? initialLifetime : 1.0f;
    const float half = duration * 0.5f;
    float fraction = duration >= remainingLifetime * 2.0f
        ? remainingLifetime / half
        : (duration - remainingLifetime) / half;
    if (fraction < 0.0f) fraction = 0.0f;
    if (fraction > 1.0f) fraction = 1.0f;
    return fraction * fraction;
}

// 0x677C54..0x677C80. Keep the observed branch direction: the checked
// buff produces the narrower 24-child spread; the other branch produces 32.
struct SwordBlowSpread
{
    int childCount;
    float lateralExtent;
};

inline SwordBlowSpread GetSwordBlowSpread(bool hasSwordWrath)
{
    return hasSwordWrath ? SwordBlowSpread{24, 300.0f}
                         : SwordBlowSpread{32, 500.0f};
}

// 0x677CA2..0x677DE1. Pegasus first builds the common destination from
// local (0,-1500,0), then builds each sword's launch position from
// (random lateral,+600..+1000,random -100..+250). The random height is on
// the launch position; moving it to the destination reverses the recovered
// pitch relationship and makes the small-sword stream point off-axis.
constexpr float kSwordBlowChildStartForwardMin = 600.0f;
constexpr float kSwordBlowChildStartForwardMax = 1000.0f;
constexpr float kSwordBlowChildStartHeightMin = -100.0f;
constexpr float kSwordBlowChildStartHeightMax = 250.0f;
constexpr float kSwordBlowChildEndForward = -1500.0f;
constexpr float kSwordBlowChildEndHeight = 0.0f;

// 0x677E19..0x677E84: asin((launchZ-destinationZ)/1500), in degrees.
// Update rotates local -Y, so its resulting world-Z sign is the opposite of
// this angle and therefore travels from launch height toward destination.
inline float SwordBlowChildPitchDegrees(float launchZ, float destinationZ)
{
    float vertical = (launchZ - destinationZ) / 1500.0f;
    if (vertical < -1.0f) vertical = -1.0f;
    if (vertical > 1.0f) vertical = 1.0f;
    return std::asin(vertical) * (180.0f / 3.14159265f);
}

inline bool CrossedDescendingCadence(float remainingLifetime, float frameStep,
    float interval)
{
    if (interval <= 0.0f) return false;
    return std::floor((remainingLifetime + frameStep) / interval) * interval >=
        remainingLifetime;
}

}} // namespace rise::pegasus
