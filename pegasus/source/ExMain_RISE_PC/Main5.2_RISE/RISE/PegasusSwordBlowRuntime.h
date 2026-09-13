#pragma once
#include "PegasusTextureIds.h"
#include "PegasusHighSkillDispatch.h"
class OBJECT;
class CHARACTER;

namespace rise { namespace pegasus {
constexpr int kSwordBlowArchangelWingModel = MAX_MODELS + 53;
enum SwordBlowLogicalAction
{
    // The pinned Pegasus player.bmd adds actions 284..286.  Keep the isolated
    // owner customization after that source-owned tail instead of replacing
    // Raining Arrow action 286.
    PLAYER_ATTACK_ONETOONE_FLY = kPegasusPlayerActionCount,
};
// Appends the isolated re-keyed action to the native Player.bmd skeleton.
bool InstallSwordBlowFlightAction();
// Reapply the character's live attack-speed scaling to the longer custom clip.
void SynchronizeSwordBlowFlightActionSpeed();
// Render-thread only, after native Models/Bitmaps initialization.
bool EnsureSwordBlowModel(int modelId);
bool EnsureSwordBlowArchangelWingModel();
bool RenderSwordBlowModel(OBJECT& effect);
bool RenderSwordBlowArchangelWings(OBJECT& effect);
void CreateSwordBlowCasterRoot(CHARACTER& caster, bool flyVariant = false);
bool StartSwordBlowFlight(CHARACTER& caster, OBJECT& target);
void UpdateSwordBlowFlight(CHARACTER& caster);
void InitializeSwordBlowBigSword(OBJECT& effect);
void InitializeSwordBlowTarget(OBJECT& effect);
void InitializeSwordBlowPlane(OBJECT& effect);
void InitializeSwordBlowRing(OBJECT& effect);
void InitializeSwordBlowMultishot(OBJECT& effect);
void CreateSwordBlowChildren(OBJECT& effect);
// Called after native CreateEffect initializes position, angle, scale and owner.
// Lifetime and speed are supplied by the recovered random-selection caller.
void InitializeSwordBlowSmallSword(OBJECT& effect, float lifetime, float speed);
void InitializeSwordBlowArchangelWings(OBJECT& effect);
void UpdateSwordBlowArchangelWings(OBJECT& effect, float animationFactor);
// Native effect loop retains responsibility for decrementing LifeTime and pool release.
void UpdateSwordBlowSmallSword(OBJECT& effect, float animationFactor);
void UpdateSwordBlowBigSword(OBJECT& effect, float animationFactor);
void UpdateSwordBlowPlane(OBJECT& effect, float animationFactor);
void UpdateSwordBlowRing(OBJECT& effect, float animationFactor);
void UpdateSwordBlowMultishot(OBJECT& effect, float animationFactor);
void UpdateSwordBlowTarget(OBJECT& effect, float animationFactor);
}}
