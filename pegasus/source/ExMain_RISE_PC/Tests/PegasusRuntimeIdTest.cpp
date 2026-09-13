#include "../Main5.2_RISE/_TextureIndex.h"
#include "../Main5.2_RISE/RISE/PegasusTextureIds.h"

#include <array>
#include <iostream>

template <std::size_t Count>
constexpr bool Unique(const std::array<int, Count>& ids)
{
    for (std::size_t left = 0; left < ids.size(); ++left)
        for (std::size_t right = left + 1; right < ids.size(); ++right)
            if (ids[left] == ids[right])
                return false;
    return true;
}

int main()
{
    constexpr std::array<int, 47> ids = {
        rise::pegasus::kElementalFlare03Bitmap,
        rise::pegasus::kHavocSomasiJointBitmap,
        rise::pegasus::kHavocAdJointBitmap,
        rise::pegasus::kSpearstormTrailBitmap,
        rise::pegasus::kSpearstormComboBitmap,
        rise::pegasus::kElementalChargeBlueBitmap,
        rise::pegasus::kElementalChargeYellowBitmap,
        rise::pegasus::kElementalChargeRedBitmap,
        rise::pegasus::kElementalChargePurpleBitmap,
        rise::pegasus::kElementalChargeGreenBitmap,
        rise::pegasus::kChaosBladeFlareBlueBitmap,
        rise::pegasus::kChaosBladeMagicGroundBitmap,
        rise::pegasus::kChaosBladePinStarBitmap,
        rise::pegasus::kChaosBladeSteamBitmap,
        rise::pegasus::kChaosBladeSmokeBitmap,
        rise::pegasus::kChaosBladeShockwaveBitmap,
        rise::pegasus::kChaosBladeSmokeLineBitmap,
        rise::pegasus::kChaosBladeSmokeLine03Bitmap,
        rise::pegasus::kChaosBladeBlurLongBitmap,
        rise::pegasus::kChaosBladeBlurBlackBitmap,
        rise::pegasus::kShiningBirdWindBitmap,
        rise::pegasus::kShiningBirdSmokeBitmap,
        rise::pegasus::kShiningBirdPinStarBitmap,
        rise::pegasus::kSwordBlowLightmarksBitmap,
        rise::pegasus::kSwordBlowFlare01Bitmap,
        rise::pegasus::kSpiralChargeFlareBitmap,
        rise::pegasus::kSpiralChargeShockwaveBitmap,
        rise::pegasus::kSpiralChargeSteamBitmap,
        rise::pegasus::kSpiralChargeSmokeBitmap,
        rise::pegasus::kDexBoosterImpactBitmap,
        rise::pegasus::kDexBoosterShinyBitmap,
        rise::pegasus::kDexBoosterHikororaBitmap,
        rise::pegasus::kDexBoosterGroundBitmap,
        rise::pegasus::kDexBoosterFlareBitmap,
        rise::pegasus::kDexBoosterFlare01Bitmap,
        rise::pegasus::kSwordBlowGroundSmokeBitmap,
        rise::pegasus::kSwordBlowSparkBitmap,
        rise::pegasus::kShiningBirdPillarBitmap,
        rise::pegasus::kDragonViolentFlareBitmap,
        rise::pegasus::kDragonViolentLightningBitmap,
        rise::pegasus::kDragonViolentSmokeBitmap,
        rise::pegasus::kDragonViolentImpactBitmap,
        rise::pegasus::kDragonViolentComboBitmap,
        rise::pegasus::kDragonViolentTwilight1Bitmap,
        rise::pegasus::kDragonViolentTwilight2Bitmap,
        rise::pegasus::kDragonViolentRingBitmap,
        rise::pegasus::kElementalShockwaveBitmap
    };
    static_assert(Unique(ids),
        "Pegasus bitmap IDs must be unique");
    static_assert(ids.front() > BITMAP_CLOAK_CACHE &&
        ids.back() <= BITMAP_EFFECT_TEXTURE_END,
        "Pegasus bitmap IDs must stay in the unused native effect tail");
    static_assert(ids.back() < BITMAP_NONAMED_TEXTURES_BEGIN,
        "Pegasus bitmap IDs must not overlap the custom/nonamed loader stream");
    std::cout << "PASS: 47 Pegasus bitmap IDs are isolated from custom texture stream\n";
    return 0;
}
