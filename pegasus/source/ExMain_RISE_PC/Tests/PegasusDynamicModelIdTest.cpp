// Runtime headers only require the MAX_MODELS base for their isolated offset
// declarations. A test-local value keeps this contract test pure and avoids
// compiling the legacy multi-megabyte engine enum surface.
#define MAX_MODELS 10000
#include "../Main5.2_RISE/RISE/PegasusSwordWrathRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusSpiralChargeRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusCrusherChargeRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusShiningBirdRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusDexBoosterRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusChaosBladeRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusDragonViolentRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusElementalChargeRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusHavocSpearRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusSpearstormRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusRainingArrowRuntime.h"
#include "../Main5.2_RISE/RISE/PegasusDeathsideRuntime.h"

#include <array>
#include <cstdio>

template <std::size_t Count>
constexpr bool Unique(const std::array<int, Count>& ids)
{
    for (std::size_t left = 0; left < Count; ++left)
        for (std::size_t right = left + 1; right < Count; ++right)
            if (ids[left] == ids[right]) return false;
    return true;
}

int main()
{
    using namespace rise::pegasus;
    constexpr std::array<int, 53> ids = {
        kSwordWrathRootModel, kSwordWrathLayerModel,
        kSpiralChargeRootModel, kSpiralChargeFlareModel,
        kSpiralChargeShockwaveModel, kSpiralChargeSteamModel,
        kSpiralChargeSmokeModel, kSpiralChargeAnchorModel,
        kCrusherChargeCoreModel, kCrusherChargeRingModel,
        kCrusherChargeShardModel, kShiningBirdModel,
        kDexBoosterAuraModel, kDexBoosterOrchestratorModel,
        kDexBoosterGroundModel, kDexBoosterFlareModel,
        kChaosBladeRootModel, kChaosBladeArcModel, kChaosBladeCrackModel,
        kDragonViolentBaseModel, kDragonViolentHeadModel,
        kDragonViolentCrackModel, kDragonViolentWindModel,
        kDragonViolentLayerModel, kDragonViolentEndModel,
        kElementalChargeModel,
        kHavocNovaModel, kHavocLineModel, kHavocShockwaveModel,
        kHavocRootModel, kHavocImpactModel, kHavocEndModel,
        kSpearstormRootModel, kSpearstormBladeModel, kSpearstormCrackModel,
        kRainingArrowRootModel, kRainingArrowFollowModel,
        kRainingArrowImpactModel, kRainingArrowGroundModel,
        kRainingArrowRing01Model, kRainingArrowRing02Model,
        kRainingArrowRing03Model, kRainingArrowRing04Model,
        kRainingArrowRing05Model, kRainingArrowRing06Model,
        kRainingArrowRing07Model, kRainingArrowRing08Model,
        kDeathsideReaperModel, kDeathsideHeadModel,
        kDeathsideScythe01Model, kDeathsideScythe02Model,
        kDeathsideScythe03Model,
        kCrusherChargeAuraModel
    };
    static_assert(Unique(ids), "Pegasus dynamic model IDs overlap");
    for (std::size_t index = 0; index < ids.size(); ++index)
        if (ids[index] != MAX_MODELS + static_cast<int>(index)) return 1;
    static_assert(ids.back() < MAX_MODELS + kDynamicModelCapacity,
        "Pegasus dynamic model tail exceeds its reserved capacity");
    std::puts("PASS: 53 Pegasus dynamic model IDs uniquely cover MAX_MODELS+0..52");
    return 0;
}
