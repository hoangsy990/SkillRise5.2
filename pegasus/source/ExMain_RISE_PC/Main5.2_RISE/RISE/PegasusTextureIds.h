#pragma once

namespace rise { namespace pegasus {

// Reserved inside the unused tail of RISE's native effect-texture range.
// Never place fixed Pegasus IDs in the >=33001 nonamed/custom-item stream.
constexpr int kPegasusSkillIconBegin = 32940;
constexpr int kPegasusSkillIconEnd = kPegasusSkillIconBegin + 14;
// The icon range is half-open, so 32954 is the first free effect-tail slot.
constexpr int kElementalFlare03Bitmap = 32954;
constexpr int kHavocSomasiJointBitmap = 32955;
constexpr int kHavocAdJointBitmap = 32956;
constexpr int kSpearstormTrailBitmap = 32957;
constexpr int kSpearstormComboBitmap = 32958;
constexpr int kElementalChargeBlueBitmap = 32959;
constexpr int kElementalChargeYellowBitmap = 32960;
constexpr int kElementalChargeRedBitmap = 32961;
constexpr int kElementalChargePurpleBitmap = 32962;
constexpr int kElementalChargeGreenBitmap = 32963;
constexpr int kChaosBladeFlareBlueBitmap = 32964;
constexpr int kChaosBladeMagicGroundBitmap = 32965;
constexpr int kChaosBladePinStarBitmap = 32966;
constexpr int kChaosBladeSteamBitmap = 32967;
constexpr int kChaosBladeSmokeBitmap = 32968;
constexpr int kChaosBladeShockwaveBitmap = 32969;
constexpr int kChaosBladeSmokeLineBitmap = 32970;
constexpr int kChaosBladeSmokeLine03Bitmap = 32971;
constexpr int kChaosBladeBlurLongBitmap = 32972;
constexpr int kChaosBladeBlurBlackBitmap = 32973;
constexpr int kShiningBirdWindBitmap = 32974;
constexpr int kShiningBirdSmokeBitmap = 32975;
constexpr int kShiningBirdPinStarBitmap = 32976;
constexpr int kSwordBlowLightmarksBitmap = 32977;
constexpr int kSwordBlowFlare01Bitmap = 32978;
constexpr int kSpiralChargeFlareBitmap = 32979;
constexpr int kSpiralChargeShockwaveBitmap = 32980;
constexpr int kSpiralChargeSteamBitmap = 32981;
constexpr int kSpiralChargeSmokeBitmap = 32982;
constexpr int kDexBoosterImpactBitmap = 32983;
constexpr int kDexBoosterShinyBitmap = 32984;
constexpr int kDexBoosterHikororaBitmap = 32985;
constexpr int kDexBoosterGroundBitmap = 32986;
constexpr int kDexBoosterFlareBitmap = 32987;
constexpr int kDexBoosterFlare01Bitmap = 32988;
constexpr int kSwordBlowGroundSmokeBitmap = 32989;
constexpr int kSwordBlowSparkBitmap = 32990;
constexpr int kShiningBirdPillarBitmap = 32991;
constexpr int kDragonViolentFlareBitmap = 32992;
constexpr int kDragonViolentLightningBitmap = 32993;
constexpr int kDragonViolentSmokeBitmap = 32994;
constexpr int kDragonViolentImpactBitmap = 32995;
constexpr int kDragonViolentComboBitmap = 32996;
constexpr int kDragonViolentTwilight1Bitmap = 32997;
constexpr int kDragonViolentTwilight2Bitmap = 32998;
constexpr int kDragonViolentRingBitmap = 32999;
// 33000 is still in CBitmapCache's inclusive effect-texture range; the
// custom/nonamed item loader starts at 33001.
constexpr int kElementalShockwaveBitmap = 33000;

constexpr bool IsPegasusPrivateEffectTexture(int type)
{
    return type >= kElementalFlare03Bitmap &&
        type <= kElementalShockwaveBitmap;
}

}}
