#pragma once
#include <cmath>

namespace rise { namespace pegasus {

inline bool SpiralChargeEmitterTick(float life, float step)
{
    // Original 0x67704B: FLOOR(life+step) > FLOOR(life).
    return std::floor(life + step) > std::floor(life);
}

// Exact constants recovered from the hash-pinned Pegasus x86 block
// 0x676CFA..0x677589. Runtime IDs are translated separately for RISE 5.2.
constexpr int kSpiralChargeSourceSkill = 2014;
constexpr int kSpiralChargeSourceBuff = 214;
constexpr int kSpiralChargeOwnerBone = 17;
constexpr float kSpiralChargeAnchorHeight = 10.0f;
constexpr float kSpiralChargeAnchorCreateScale = 2.0f;
constexpr float kSpiralChargeAnchorRuntimeScale = 1.5f;
constexpr float kSpiralChargeRootLifetime = 30.0f;
constexpr float kSpiralChargeFlareLifetime = 20.0f;
constexpr float kSpiralChargeShockwaveLifetime = 20.0f;
constexpr float kSpiralChargeEmitterLifetime = 40.0f;
constexpr int kSpiralChargeShockwaveCount = 2;
constexpr int kSpiralChargeRootJointCount = 9;
constexpr float kSpiralChargeRootJointScale = 20.0f;
constexpr float kSpiralChargeRootJointLifetime = 30.0f;
constexpr int kSpiralChargeRootJointMaxTails = 5;
constexpr float kSpiralChargeRootJointActivationFrame = 0.75f;
constexpr float kSpiralChargeRootAnimationSpeed = 0.3f;
constexpr int kSpiralChargeSteamParticleSubtype = 4;
constexpr int kSpiralChargeSmokeParticleSubtype = 12;
constexpr float kSpiralChargeSteamParticleLifetime = 10.0f;
constexpr float kSpiralChargeSmokeParticleLifetime = 12.0f;
constexpr float kSpiralChargeSmokeFadeSplit = 6.0f;
constexpr float kSpiralChargeParticleGrowth = 0.1f;
constexpr float kSpiralChargeSmokeAlphaStep = 1.0f / 6.0f;
constexpr int kSpiralChargeAnchorBoneCount = 17;
constexpr int kSpiralChargeAnchorBones[kSpiralChargeAnchorBoneCount] = {
    12, 17, 5, 10, 36, 27, 37, 28, 11, 35, 2, 3, 36, 20, 27, 4, 26
};

}}
