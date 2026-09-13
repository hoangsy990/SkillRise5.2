#pragma once

#include "PegasusRuntimeCapacity.h"

class CHARACTER;
class OBJECT;

namespace rise { namespace pegasus {
constexpr int kDeathsideReaperModel = MAX_MODELS + 47;
constexpr int kDeathsideHeadModel = MAX_MODELS + 48;
constexpr int kDeathsideScythe01Model = MAX_MODELS + 49;
constexpr int kDeathsideScythe02Model = MAX_MODELS + 50;
constexpr int kDeathsideScythe03Model = MAX_MODELS + 51;
constexpr int kDeathsideFirstModel = kDeathsideReaperModel;
constexpr int kDeathsideLastModel = kDeathsideScythe03Model;
constexpr float kDeathsideRootHeight = 80.0f;
constexpr float kDeathsideFollowFactor = 0.056f;
constexpr float kDeathsideFollowStartDistance = 10.0f;
constexpr float kDeathsideFollowStopDistance = 20.0f;
constexpr float kDeathsideTurnStep = 20.0f;
constexpr float kDeathsideTrailStartFrame = 1.5f;
constexpr float kDeathsideTrailLight = 0.32f;
// Pegasus model setup0x660079..0x6600AA, float0xB6B128.
constexpr float DeathsideAttackPlaySpeed(int keys)
{
    return keys > 1 ? static_cast<float>(keys) / 16.675f : 0.02f;
}
constexpr int kDeathsideTrailLifetime = 100;
constexpr int kDeathsideMirroredTailLifetime = -1;
constexpr float kDeathsideParticleHeight = 130.0f;
constexpr unsigned int kDeathsideActiveMilliseconds = 10000;
constexpr float kDeathsideScytheLifetime = 60.0f;
constexpr float kDeathsideDisappearLifetime = 20.0f;
constexpr float kDeathsideInitialAlpha = 0.2f;
constexpr float kDeathsideDisappearAlpha = 0.5f;
constexpr int kDeathsideGroundSmokeCadence = 13;
constexpr int kDeathsideSmokeCadence = 10;
constexpr int kDeathsideWaterfallCadence = 34;
constexpr int kDeathsideGroundSmokeSubtype = 1;
constexpr int kDeathsideSmokeSubtype = 116;
constexpr int kDeathsideIdleAction = 7;
constexpr int kDeathsidePostAttackAction = 0;
constexpr float kDeathsideMinimumPlaySpeed = 0.02f;
constexpr int kDeathsideDisappearAction = 6;
constexpr int kDeathsideAttackActions[] = {3, 4, 8};
constexpr int kDeathsideCasterSummonActions[] = {172, 173, 174, 175};
constexpr int kDeathsideCasterRepeatAction = 185;
static_assert(52 <= kDynamicModelCapacity, "Deathside model tail is too small");

bool EnsureDeathsideModels();
bool IsDeathsideBlurBitmap(int bitmapId);
bool IsDeathsideParticleBitmap(int bitmapId);
bool IsDeathsideGroundSmokeBitmap(int bitmapId);
bool IsDeathsideSmokeBitmap(int bitmapId);
void ApplyDeathsideCasterAction(CHARACTER& caster);
void CastDeathside(CHARACTER& caster, int skill);
void InitializeDeathside(OBJECT& effect);
void UpdateDeathside(OBJECT& effect, float animationFactor);
bool RenderDeathside(OBJECT& effect);
}}
