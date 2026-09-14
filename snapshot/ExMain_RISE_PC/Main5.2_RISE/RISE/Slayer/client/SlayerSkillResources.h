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
    kPierceSwordLineModel = MAX_MODELS + 44,
    kFirstModel = kSwordInertiaModel,
    kLastModel = kPierceSwordLineModel,

    // Non-rendering S21 controller objects. They deliberately live outside
    // the BMD range so ownership/timing is not collapsed into child models.
    kSwordInertiaController = MAX_MODELS + 45,
    kBatFlockController = MAX_MODELS + 46,
    kBatFlockTargetController = MAX_MODELS + 47,
    kBatFlockDotController = MAX_MODELS + 48,
    kPierceController = MAX_MODELS + 49,
    kDetectionController = MAX_MODELS + 50,
    kDetectionChildController = MAX_MODELS + 51,
    kPierceMotionController = MAX_MODELS + 52,
    kBatFlockOrbitController = MAX_MODELS + 53,
    kPierceSwordController = MAX_MODELS + 54,
    kPierceFlashController = MAX_MODELS + 55,
    kPierceBurstController = MAX_MODELS + 56,
    kPierceFanController = MAX_MODELS + 57,
    kPierceBatController = MAX_MODELS + 58,
    kSwordFlightController = MAX_MODELS + 59,
    // Demolish is the fifth imported skill.  S21 dispatches its native
    // effect root as 0x695; this slot is only the isolated 5.2 carrier.
    kDemolishController = MAX_MODELS + 60,
    kLastEffect = kDemolishController
};

enum BitmapId
{
    // Private Slayer overlay slots.  These stay below BITMAP_EFFECT_TEXTURE_END
    // and outside the Grow Lancer allocation (32940..32980).
    kMagicGround12Bitmap = 32981,
    kRingOfGradation2Bitmap = 32982,
    kPinStar03Bitmap = 32983,
    kFlare01RedBitmap = 32984,
    kEnemyRing01Bitmap = 32985,
    kGhostMark02RedBitmap = 32986,
    kSmoke01Bitmap = 32987,
    kSmokeLines01Bitmap = 32988,
    kSmokeLines02Bitmap = 32989,
    kSmokeLines03Bitmap = 32990,
    kPinStar02RedBitmap = 32991,
    kFireHik01MagentaBitmap = 32992,
    kEmpact01Bitmap = 32993,
    kGhostMark02Bitmap = 32994,
    kAlphaRingX256Bitmap = 32995,
    kDamage1MonoBitmap = 32996,
    kGroundStarBitmap = 32997,
    kFireHik01GoldBitmap = 32998,
    kFlare01YellowBitmap = 32999
};

// Numeric controller ids recovered from the S21 ReceiveMagic dispatcher at
// 0x12CEB00. They are evidence labels for the ported graph, not 5.2 model
// indices. The previous generic high-skill labels are deliberately not used
// here: these are the Slayer 292..297 roots from the actual S21 main dump.
enum S21VisualCode
{
    // 292 Sword Inertia: handler 0x12D0F0F emits 0x68A then 0x689.
    kS21SwordInertiaRoot = 0x68A,
    kS21SwordInertiaSecondaryRoot = 0x689,
    // 293 Bat Flock: handler 0x12D10A0 emits 0x682/0x683/0x684.
    // Do not label any extra child until a direct xref is recovered.
    kS21BatFlockRoot = 0x682,
    kS21BatFlockTargetRoot = 0x683,
    kS21BatFlockDotRoot = 0x684,
    // 294 Pierce Attack: ReceiveMagic handler 0x12D1209 emits 0x679.
    // The separate skill-action path at 0x010E6830 emits 0x68A then 0x679;
    // those two call sites are kept distinct in the evidence document.
    kS21PierceRoot = 0x679,
    // 295 Detection: handler 0x12D139B emits 0x692. No child is asserted
    // from this handler because the dump does not push a child id there.
    kS21DetectionRoot = 0x692,
    // 297 Demolish: handler 0x12D1443 emits 0x695.
    kS21DemolishRoot = 0x695
};

// The isolated player.bmd keeps all 284 native 5.2 actions byte-identical and
// appends the four unique recovered S21 clips (224,227,228,232). S21 action
// 233 (Demolish) is byte-identical to action 232 in the supplied main dump,
// so it reuses the same merged clip rather than changing the player ABI.
enum PlayerActionId
{
    kSwordInertiaAction = 284,
    kBatFlockAction = 285,
    kPierceAttackAction = 286,
    kDetectionAction = 287,
    kDemolishAction = kDetectionAction
};

bool IsEffectType(int type);
bool RequiresModel(int type);
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
