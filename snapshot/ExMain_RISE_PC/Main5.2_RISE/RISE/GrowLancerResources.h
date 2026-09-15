#pragma once

#include "../_enum.h"
#include "../DSPlaySound.h"
#include "GrowLancerRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace growlancer {

enum SkillId
{
    kSpinStepSkill = 271,
    kCircleShieldSkill = 272,
    kObsidianSkill = 273,
    kMagicPinSkill = 274,
    kClashSkill = 275,
    kHarshStrikeSkill = 276,
    kShiningPeakSkill = 277,
    kWrathSkill = 278,
    kBrecheSkill = 279,
    kSpinStepExplosionSkill = 280,
    kMagicPinExplosionSkill = 281
};

enum ModelId
{
    kWrathAuraModel = MAX_MODELS + 0,
    kWrathTravelModel = MAX_MODELS + 1,
    kObsidianRotatingModel = MAX_MODELS + 2,
    kObsidianAuraModel = MAX_MODELS + 3,
    kCircleShieldModel = MAX_MODELS + 4,
    kSpinCrossModel = MAX_MODELS + 5,
    kMagicPin01Model = MAX_MODELS + 6,
    kMagicPin03Model = MAX_MODELS + 7,
    kMagicPinRootModel = MAX_MODELS + 8,
    kMagicPinAuxModel = MAX_MODELS + 9,
    kShiningPeakBodyModel = MAX_MODELS + 10,
    kShiningPeakPinModel = MAX_MODELS + 11,
    kShiningPeakSpinModel = MAX_MODELS + 12,
    kShiningPeakShockwaveModel = MAX_MODELS + 13,
    kHarshWind01Model = MAX_MODELS + 14,
    kHarshWind02Model = MAX_MODELS + 15,
    kClashFrontModel = MAX_MODELS + 16,
    kClashRearModel = MAX_MODELS + 17,
    kSpinControllerModel = MAX_MODELS + 18,
    kMagicPinControllerModel = MAX_MODELS + 19,
    kShiningPeakControllerModel = MAX_MODELS + 20,
    kCircleShieldControllerModel = MAX_MODELS + 21,
    kHarshStrikeControllerModel = MAX_MODELS + 22,
    kWrathControllerModel = MAX_MODELS + 23,
    kClashControllerModel = MAX_MODELS + 24,
    // Bitmap-backed layers that are true sprite nodes use this native effect-
    // pool carrier. Particle/effect resources with recovered SS21 lifecycle
    // contracts are dispatched directly by their private runtime IDs.
    kSpriteCarrierModel = MAX_MODELS + 25,
    kBrecheControllerModel = MAX_MODELS + 26,
    kBrecheLightMarksModel = MAX_MODELS + 27,
    kBrecheTwilight02Model = MAX_MODELS + 28,
    kBrecheTwilight01Model = MAX_MODELS + 29,
    kBrecheEmitterModel = MAX_MODELS + 30,
    // S21 5FD/subtype0 owner-side producer.  These are deliberately separate
    // from the receive-root children even when they reuse the same texture;
    // the owner branch has different timing, scale and attachment semantics.
    kBrecheOwnerRingModel = MAX_MODELS + 31,
    kBrecheOwnerLightMarksModel = MAX_MODELS + 32,
    kBrecheOwnerFireRingModel = MAX_MODELS + 33,
    kBrecheOwnerWindModel = MAX_MODELS + 34,
    kBrecheOwnerTwilight02Model = MAX_MODELS + 35,
    kBrecheOwnerTwilight01Model = MAX_MODELS + 36,
    kFirstModel = kWrathAuraModel,
    kLastVisibleModel = kClashRearModel,
    kLastModel = kBrecheOwnerTwilight01Model
};

static_assert(31 <= kDynamicModelCapacity,
    "Grow Lancer dynamic model tail is too small");

// Reserved unused slots inside RISE's native effect-texture range. Do not use
// the >=33001 nonamed/custom-item stream.
enum BitmapId
{
    kWrathComboBitmap = 32940,
    kWrathAlphaLightBitmap = 32941,
    kWrathBrokenBitmap = 32942,
    // 32943 remains deliberately unused: Thunder01 is byte-identical to the
    // native BITMAP_ENERGY resource and must stay in that particle family.
    // 32944 remains deliberately unused: force_Pillar is byte-identical to
    // native BITMAP_FORCEPILLAR and Circle Shield uses CreateJoint subtype 2.
    kCircleShinyBitmap = 32945,
    // 32946 remains deliberately unused: flare01 is byte-identical to the
    // native SS6 BITMAP_LIGHT and Circle Shield uses CreateParticle directly.
    kFlareBlueBitmap = 32947,
    kHoleBitmap = 32948,
    kSpinGroundBitmap = 32949,
    // 32950 remains deliberately unused: clud64 is byte-identical to native
    // BITMAP_CLUD64; subtype 19 renders native BITMAP_SMOKE.
    kShockwave2Bitmap = 32951,
    // 32952 remains deliberately unused: pin_lights is byte-identical to the
    // native BITMAP_PIN_LIGHT resource and runs through CreateJoint.
    kXsuper0001Bitmap = 32953,
    kMagicGround2Bitmap = 32954,
    kFlare01RedBitmap = 32955,
    kImpact03Bitmap = 32956,
    kMagicGround3Bitmap = 32957,
    kDamageMonoBitmap = 32958,
    kBrecheRedBitmap = 32959,
    // S21 renders the xsuper particle as Type + its 0..5 animation subtype.
    // Extra frames use unused effect-texture slots while the base ID remains
    // stable for the isolated particle dispatcher.
    kXsuper0002Bitmap = 32960,
    kXsuper0003Bitmap = 32961,
    kXsuper0004Bitmap = 32962,
    kXsuper0005Bitmap = 32963,
    kXsuper0006Bitmap = 32964,
    // Exact S21 512x512 skill atlases. Kept outside the shared SS6 UI IDs so
    // Grow Lancer does not replace icons used by existing classes.
    kSkillIconAtlasBitmap = 32965,
    kDisabledSkillIconAtlasBitmap = 32966,
    // Private native slots, NOT the S21 runtime resource numbers.
    kBrecheLightMarksBitmap = 32967,
    kBrecheTwilight02Bitmap = 32968,
    kBrecheTwilight01Bitmap = 32969,
    kWrathMono01Bitmap = 32970,
    kWrathMono02Bitmap = 32971,
    kWrathMono03Bitmap = 32972,
    kWrathScatter01Bitmap = 32973,
    kWrathScatter02Bitmap = 32974,
    kWrathScatter03Bitmap = 32975,
    kWrathLightmarksBitmap = 32976,
    kWrathFlare01Bitmap = 32977,
    kWrathFlareBlueBitmap = 32978,
    kWrathShockwaveBitmap = 32979,
    kWrathBuffAtlasBitmap = 32980,
    // firering01 is not present in the SS6 named texture registry.  It is
    // staged privately and loaded only for the Breche owner-side branch.
    kBrecheOwnerFireRingBitmap = 32981,
    // Owner-side 0x80BC is kept on a private registration even though the
    // image is also used by unrelated effects in the source client.
    kBrecheOwnerRingBitmap = 32982,
    // S21 Circle Shield buff216/221/222: upper-arm firehik_mono01/subtype12.
    // Private Circle registration; no reuse of Wrath's bitmap slot/ownership.
    kCircleUpperArmMonoBitmap = 32983,
    // S21 master-icon page 0 is a distinct bitmap from the base skill atlas.
    // Only the source-pinned Grow Lancer master 895 uses this private slot.
    kWrathMasterIconAtlasBitmap = 32984,
    kWrathMasterDisabledIconAtlasBitmap = 32985,
    // S21 motion_blur registers NEAREST/CLAMP; SS6's shared BITMAP_BLUR+1
    // is NEAREST/CLAMP_TO_EDGE. Keep only Spin's style-1 ribbon private.
    kSpinMotionBlurBitmap = 32986
};

enum SkillIconContract
{
    kSkillIconAtlasColumns = 25,
    kSkillIconWidth = 20,
    kSkillIconHeight = 28,
    kSkillIconAtlasSize = 512
};

enum WrathMasterIconContract
{
    kWrathMasterSkillId = 895,
    kWrathMasterSkillGroup = 354,
    kWrathMasterIconNumber = 408
};

enum SoundId
{
    kSpinStepCastSound = MAX_BUFFER + 0,
    kSpinStepHitSound = MAX_BUFFER + 1,
    kCircleShieldSound = MAX_BUFFER + 2,
    kObsidianSound = MAX_BUFFER + 3,
    kMagicPinCastSound = MAX_BUFFER + 4,
    kMagicPinHitSound = MAX_BUFFER + 5,
    kClashSound = MAX_BUFFER + 6,
    kHarshStrikeSound = MAX_BUFFER + 7,
    kShiningPeakSound = MAX_BUFFER + 8,
    kWrathSound = MAX_BUFFER + 9,
    kBrecheSound = MAX_BUFFER + 10,
    kSoundBufferCapacity = MAX_BUFFER + 11
};

bool EnsureModel(int modelId);
// Source-class contract is explicit; this does not enable native class learning.
// Caller supplies sampler policy, which still needs gameplay parity validation.
bool EnsureClassBodyModel(unsigned sourceClassByte, unsigned part, unsigned filter, unsigned wrap);
enum class BaseBodySubmitResult { Unavailable, Skipped, Submitted };
// Ordinary base texture pass only. Caller owns equipment selection, final class
// identity, shadows/buffs/selection passes and the validity of its bone array.
BaseBodySubmitResult SubmitClassBaseBody(unsigned sourceClassByte, unsigned part,
    OBJECT* owner, unsigned boneCount, const float* light, bool hideSkin,
    bool translate, unsigned filter, unsigned wrap);
bool IsVisibleModel(int modelId);
bool EnsureBitmaps();
bool EnsureBrecheBitmaps();
bool EnsureWrathPersistentBitmaps();
bool EnsureCirclePersistentBitmap();
bool EnsureSpinMotionBlurBitmap();
bool EnsureWrathScatterBitmaps();
bool EnsureWrathGroundSpriteBitmaps();
bool EnsureWrathBuffAtlas();
void ReleaseWrathBuffAtlas();
void LoadSounds();
void ApplySkillCatalog();

}}
