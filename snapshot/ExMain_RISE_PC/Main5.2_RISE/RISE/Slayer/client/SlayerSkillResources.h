#pragma once

class OBJECT;

// Slayer's private S21 model tail is allocated after the Grow Lancer slots.
// These ids are only accepted by the isolated Slayer QA renderer; legacy 5.2
// model ids and the Grow Lancer namespace remain untouched.
namespace rise { namespace slayer {

enum ModelId
{
    // +37/+38 were previously assigned to ak_skill_sword*.  Dump xrefs show
    // those models are not part of Slayer 292, so the slots stay unused.
    kUnusedAkSwordModel = MAX_MODELS + 37,
    kUnusedAkSwordAuraModel = MAX_MODELS + 38,
    kBatFlockModel = MAX_MODELS + 39,
    kBatFlockTrailModel = MAX_MODELS + 40,
    // Native Pierce 0x81CD subtype-2 child 0x5D8 is marks_cylinder.bmd.
    kPierceMarksCylinderModel = MAX_MODELS + 41,
    kDetectionMarkModel = MAX_MODELS + 42,
    kDetectionImpactModel = MAX_MODELS + 43,
    kPierceSwordLineModel = MAX_MODELS + 44,
    kFirstModel = kBatFlockModel,
    kLastModel = kPierceSwordLineModel,

    // Non-rendering S21 controller objects. They deliberately live outside
    // the BMD range so ownership/timing is not collapsed into child models.
    kSwordInertiaController = MAX_MODELS + 45, // native 0x68A
    kBatFlockController = MAX_MODELS + 46,
    kBatFlockTargetController = MAX_MODELS + 47,
    kBatFlockDotController = MAX_MODELS + 48,
    kPierceController = MAX_MODELS + 49,
    kDetectionController = MAX_MODELS + 50,
    kDetectionChildController = MAX_MODELS + 51,
    kSwordSecondaryController = MAX_MODELS + 52, // native 0x689
    kBatFlockOrbitController = MAX_MODELS + 53,  // native 0x685
    kPierce67AController = MAX_MODELS + 54,
    kPierce67BController = MAX_MODELS + 55,
    kPierce67CController = MAX_MODELS + 56,
    kPierce67DController = MAX_MODELS + 57,
    kPierce67EController = MAX_MODELS + 58,
    kPierce680Controller = MAX_MODELS + 59,
    // Demolish is the fifth imported skill. S21 root 0x695 owns a distinct
    // 0x696 bat-emitter child; it must not reuse Detection's 0x693 child.
    kDemolishController = MAX_MODELS + 60,
    kDemolishChildController = MAX_MODELS + 61,
    kBatFlock686Controller = MAX_MODELS + 62,
    kBatFlock687Controller = MAX_MODELS + 63,
    kSword68BController = MAX_MODELS + 64,
    kSword68CController = MAX_MODELS + 65,
    kSword68DController = MAX_MODELS + 66,
    kSword68EController = MAX_MODELS + 67,
    kSword68FController = MAX_MODELS + 68,
    kSword690Controller = MAX_MODELS + 69,
    kLastEffect = kSword690Controller,
    // Bitmap-backed secondary-pool objects created by S21 0x143E57C.
    // They are not particle-pool entries (0x1724176).
    kFlare01RedEffect = MAX_MODELS + 70, // native 0x80BA
    kRingOfGradation2Effect = MAX_MODELS + 71, // native 0x82F6
    kEnemyRing01Effect = MAX_MODELS + 72, // native 0x82F7
    kMagicGround12Effect = MAX_MODELS + 73, // native 0x81CF
    kFlareBlueEffect = MAX_MODELS + 74, // native 0x7FDD
    kFlareEffect = MAX_MODELS + 75, // native 0x7F78
    // Pierce E4 one-shot action initializer: 0x81CD subtype 2 owns the
    // native child graph; 0x81CE subtypes 3/4/5 are bitmap-backed children.
    kPierce81CDController = MAX_MODELS + 76,
    kPierce81CEEffect = MAX_MODELS + 77,
    kPierce80BAEffect = MAX_MODELS + 78, // native 0x80BA modes 6/7
    kPierce8149Effect = MAX_MODELS + 79, // native 0x8149 mode 2
    kPierceShockWaveEffect = MAX_MODELS + 80 // native 0x8012 mode 17
};

enum BitmapId
{
    // Grow Lancer reserves 32940..32982. The isolated Slayer allocation
    // starts at 32983; 33001..33015 are reserved from the 5.2 unnamed
    // allocator in GlobalBitmap when RISE_SLAYER_PORT is enabled.
    kMagicGround12Bitmap = 32983,
    kRingOfGradation2Bitmap = 32984,
    kPinStar03Bitmap = 32985,
    kFlare01RedBitmap = 32986,
    kEnemyRing01Bitmap = 32987,
    kGhostMark02RedBitmap = 32988,
    kSmoke01Bitmap = 32989,
    kSmokeLines01Bitmap = 32990,
    kSmokeLines02Bitmap = 32991,
    kSmokeLines03Bitmap = 32992,
    kPinStar02RedBitmap = 32993,
    kFireHik01MagentaBitmap = 32994,
    kEmpact01Bitmap = 32995,
    kGhostMark02Bitmap = 32996,
    kAlphaRingX256Bitmap = 32997,
    kDamage1MonoBitmap = 32998,
    kGroundStarBitmap = 32999,
    kFlareBlueBitmap = 33000,
    kFlareBitmap = 33001,
    kWaterBoardRedBitmap = 33002,
    kWaterWallBitmap = 33003,
    kSmoke01StrongBitmap = 33004,
    kBlur02MonoLongVan2Bitmap = 33005,
    kBetGrilsShot2RedBitmap = 33006,
    kImpack03Bitmap = 33007,
    kPinStarBitmap = 33008,
    kJujugBitmap = 33009,
    // Native S21 0x8020 = Effect\\WATERFALL4.jpg. 5.2 has no enum row for
    // that S21 id, so the isolated overlay uses the next private gap slot.
    kWaterFall4Bitmap = 33010,
    // Native S21 0x82EC, selected by 0x681 subtypes 0..2.
    kBlur02MonoLongVanBitmap = 33011,
    // Native S21 bitmap 0x81CE is NPC\\marks_m04.JPG, used by Pierce's
    // subtype-2 0x81CD initializer children 3/4/5.
    kMarksM04Bitmap = 33012,
    // S21 0x81CE subtype 4/5 render with native bitmap 0x81CD,
    // loaded from NPC\\marks_m03.JPG at 0xAA995F.
    kMarksM03Bitmap = 33013,
    kFlare01Bitmap = 33014, // native 0x7EF7 Effect\\flare01.JPG
    kShockWaveBitmap = 33015 // native 0x8012 Effect\\ShockWave.JPG
};

// Numeric controller ids recovered from the S21 ReceiveMagic dispatcher at
// 0x12CEB00. They are evidence labels for the ported graph, not 5.2 model
// indices. The previous generic high-skill labels are deliberately not used
// here: these are the Slayer 292..297 roots from the actual S21 main dump.
enum S21VisualCode
{
    // 292 base Sword: action branch 0x10E68A3 emits 0x679.
    // ReceiveMagic 0x12D0F0F emits 0x68A/0x689 only for upgrade 0x818.
    kS21SwordInertiaRoot = 0x679,
    kS21SwordInertiaUpgradeRoot = 0x68A,
    kS21SwordInertiaUpgradeSecondaryRoot = 0x689,
    // 293 Bat Flock: handler 0x12D10A0 emits 0x682/0x683/0x684.
    // Do not label any extra child until a direct xref is recovered.
    kS21BatFlockRoot = 0x682,
    kS21BatFlockTargetRoot = 0x683,
    kS21BatFlockDotRoot = 0x684,
    // 294 base Pierce: action branch 0x10E6D1B emits 0x68A; its 0x679
    // child and ReceiveMagic 0x12D1209 both require upgrade 0x816.
    kS21PierceRoot = 0x679,
    // 295 Detection: handler 0x12D139B emits 0x692. No child is asserted
    // from this handler because the dump does not push a child id there.
    kS21DetectionRoot = 0x692,
    // 297 Demolish: handler 0x12D1443 emits 0x695.
    kS21DemolishRoot = 0x695
};

// The isolated player.bmd keeps all 284 native 5.2 actions byte-identical and
// appends all five recovered S21 action slots (224,227,228,232,233). S21
// action 233 (Demolish) is byte-identical to Detection's 232 clip, but must
// keep a distinct action ID: the 0x692/0x695 updater gates on E8/E9
// separately, even when their animation keys happen to match.
enum PlayerActionId
{
    kSwordInertiaAction = 284,
    kBatFlockAction = 285,
    kPierceAttackAction = 286,
    kDetectionAction = 287,
    kDemolishAction = 288
};

bool IsEffectType(int type);
bool RequiresModel(int type);
bool EnsureModel(int modelId);
void InitializeEffect(OBJECT& effect, float incomingScale);
void SetBatFlockTargets(OBJECT& effect, const short* targetIndexes, int count);
void SetPierceTargets(OBJECT& effect, const short* targetIndexes, int count,
    unsigned char castSerial);
void UpdateEffect(OBJECT& effect, float animationFactor);
bool RenderEffect(OBJECT& effect);
void ApplyPlayerActionSpeeds(int attackSpeed);
bool ApplyCastAction(OBJECT& actor, int skillId);
void LoadSounds();
void PlayCastSound(int skillId, OBJECT* actor);
void PlayImpactSound(int skillId, OBJECT* actor, int ordinal = 0);

void ApplySkillCatalog();

}}
