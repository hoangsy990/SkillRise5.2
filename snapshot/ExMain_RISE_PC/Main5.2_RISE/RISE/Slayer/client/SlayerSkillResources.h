#pragma once

class OBJECT;

// Slayer's private S21 model tail is allocated after the Grow Lancer slots.
// These ids are only accepted by the isolated Slayer QA renderer; legacy 5.2
// model ids and the Grow Lancer namespace remain untouched.
namespace rise { namespace slayer {

enum ModelId
{
    kSwordInertiaModel = MAX_MODELS + 37,
    kSwordInertiaAuraModel = MAX_MODELS + 38,
    kBatFlockModel = MAX_MODELS + 39,
    kBatFlockTrailModel = MAX_MODELS + 40,
    kPierceImpactModel = MAX_MODELS + 41,
    kDetectionMarkModel = MAX_MODELS + 42,
    kDetectionImpactModel = MAX_MODELS + 43,
    kFirstModel = kSwordInertiaModel,
    kLastModel = kDetectionImpactModel
};

// Numeric controller ids recovered from the S21 ReceiveMagic dispatcher.
// They are evidence labels for the ported graph, not 5.2 model indices.
enum S21VisualCode
{
    kS21SwordInertiaController = 0x679,
    kS21BatFlockController = 0x682,
    kS21BatFlockTargetController = 0x683,
    kS21BatFlockDotController = 0x684,
    kS21BatFlockTrailChild = 0x688,
    kS21PierceController = 0x68A,
    kS21PierceSwordLineChild = 0x681,
    kS21DetectionController = 0x692,
    kS21DetectionMarkChild = 0x693,
    kS21DetectionImpactChild = 0x694
};

// The isolated player.bmd keeps all 284 native 5.2 actions byte-identical and
// appends the four recovered S21 clips (224,227,228,232) in this order.
enum PlayerActionId
{
    kSwordInertiaAction = 284,
    kBatFlockAction = 285,
    kPierceAttackAction = 286,
    kDetectionAction = 287
};

bool IsEffectType(int type);
bool EnsureModel(int modelId);
void InitializeEffect(OBJECT& effect);
void UpdateEffect(OBJECT& effect, float animationFactor);
bool RenderEffect(OBJECT& effect);
bool ApplyCastAction(OBJECT& actor, int skillId);
void LoadSounds();
void PlayCastSound(int skillId, OBJECT* actor);
void PlayImpactSound(int skillId, OBJECT* actor, int ordinal = 0);

void ApplySkillCatalog();

}}
