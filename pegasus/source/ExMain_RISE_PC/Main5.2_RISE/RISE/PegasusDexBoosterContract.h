#pragma once

#include <cmath>
#include "../../../Shared/PegasusHighSkillCatalogData.h"

// Exact client-side Dex Booster (skill 2024) constructor contract recovered
// from the hash-pinned Pegasus blocks 0x66110F..0x661319. Resource IDs are
// expressed through native RISE semantics in the runtime adapter.
namespace rise { namespace pegasus {

constexpr float kDexBoosterAuraLifetime = 80.0f;
constexpr float kDexBoosterGroundLifetime = 80.0f;
constexpr float kDexBoosterFlareLifetime = 30.0f;
constexpr float kDexBoosterGroundStartLight = 0.5f;
constexpr float kDexBoosterGroundAlphaStep = 0.00625f;
constexpr float kDexBoosterGroundRotation9 = 10.0f;
constexpr float kDexBoosterGroundRotation10 = 5.0f;
constexpr float kDexBoosterGroundRotation11 = 15.0f;
constexpr float kDexBoosterAuraRotation = 0.08f;
constexpr float kDexBoosterFlareScale = 0.9f;
constexpr int kDexBoosterFlareParticleSubtype = 5;
constexpr float kDexBoosterFlareParticleBaseScale = 3.5f;
constexpr int kDexBoosterInitialFlareJointCount = 19;
constexpr int kDexBoosterInitialImpactParticleCount = 20;
constexpr int kDexBoosterFlareJointSubtype = 48;
constexpr float kDexBoosterFlareJointScale = 40.0f;
constexpr float kDexBoosterFlareJointRuntimeScale = 30.0f;
constexpr float kDexBoosterFlareJointLifetime = 20.0f;
constexpr int kDexBoosterFlareJointMaxTails = 80;
constexpr float kDexBoosterFlareJointLightR = 1.0f;
constexpr float kDexBoosterFlareJointLightG = 0.75f;
constexpr float kDexBoosterFlareJointLightB = 0.3f;
constexpr float kDexBoosterFlareJointIndexPhase = 0.935100019f;
constexpr float kDexBoosterFlareJointPhaseMultiplier = 1.7f;
constexpr float kDexBoosterFlareJointFirstTimeMultiplier = 1.3f;
constexpr float kDexBoosterFlareJointSecondTimeMultiplier = 2.2f;
constexpr float kDexBoosterFlareJointRadius = 95.0f;
constexpr float kDexBoosterFlareJointHeight = 100.0f;
constexpr float kDexBoosterImpactParticleScale = 0.005f;
constexpr float kDexBoosterImpactParticleLifetime = 60.0f;
constexpr float kDexBoosterImpactFadeStart = 30.0f;
constexpr float kDexBoosterImpactPeakAlpha = 0.7f;
constexpr float kDexBoosterImpactStopAlpha = 0.1f;

inline void ComputeDexBoosterFlareJointPosition(double worldTimeMilliseconds,
    int jointIndex, const float target[3], float position[3])
{
    const float time = static_cast<float>(worldTimeMilliseconds * 0.001);
    const float indexPhase = static_cast<float>(jointIndex) *
        kDexBoosterFlareJointIndexPhase;
    const float firstAngle = indexPhase *
        kDexBoosterFlareJointPhaseMultiplier +
        time * kDexBoosterFlareJointFirstTimeMultiplier;
    const float secondAngle = time *
        kDexBoosterFlareJointSecondTimeMultiplier + indexPhase;
    const float firstSin = std::sin(firstAngle);

    position[0] = target[0] + firstSin * std::cos(secondAngle) *
        kDexBoosterFlareJointRadius;
    position[1] = target[1] + firstSin * std::sin(secondAngle) *
        kDexBoosterFlareJointRadius;
    position[2] = target[2] + std::cos(firstAngle) *
        kDexBoosterFlareJointRadius + kDexBoosterFlareJointHeight;
}

}} // namespace rise::pegasus
