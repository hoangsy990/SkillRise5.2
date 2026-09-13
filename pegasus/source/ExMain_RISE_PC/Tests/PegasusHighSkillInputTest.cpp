#include "../Main5.2_RISE/RISE/PegasusHighSkillInput.h"
#include "../../Shared/PegasusSwordBlowProtocol.h"
#include <cassert>

int main()
{
    using namespace rise::pegasus;
    static_assert(kHighSkillSeedCount == 14, "catalog-size drift");
    for (const HighSkillSeed& seed : kHighSkillSeeds)
        assert(IsPegasusHighSkillInput(seed.skillId));

    static_assert(IsPegasusSelfBuffInput(kDexBoosterSkill), "Dex Booster route drift");
    static_assert(IsPegasusSelfBuffInput(kSpiralChargeSkill), "Spiral route drift");
    static_assert(IsPegasusSelfBuffInput(kCrusherChargeSkill), "Crusher route drift");
    static_assert(IsPegasusSelfBuffInput(kElementalChargeSkill), "Elemental route drift");
    static_assert(IsPegasusSelfBuffInput(kSwordWrathSkill), "Sword Wrath route drift");
    static_assert(IsPegasusSelfBuffInput(kDarknessSkill), "Darkness route drift");

    static_assert(IsPegasusTargetAttackInput(kDeathsideSkill), "Deathside route drift");
    static_assert(IsPegasusTargetAttackInput(kRainingArrowSkill), "Raining Arrow route drift");
    static_assert(IsPegasusTargetAttackInput(kChaosBladeSkill), "Chaos Blade route drift");
    static_assert(IsPegasusTargetAttackInput(kHavocSpearSkill), "Havoc Spear route drift");
    static_assert(IsPegasusTargetAttackInput(kSwordBlowSkill), "Sword Blow route drift");
    static_assert(IsPegasusTargetAttackInput(kSwordBlowFlySkill),
        "Sword Blow Fly route drift");
    static_assert(IsPegasusTargetAttackInput(kShiningBirdSkill), "Shining Bird route drift");
    static_assert(IsPegasusTargetAttackInput(kDragonViolentSkill), "Dragon Violent route drift");
    static_assert(IsPegasusTargetAttackInput(kSpearstormSkill), "Spearstorm route drift");
    static_assert(!IsPegasusHighSkillInput(737), "Wind Soul must remain standalone");
    return 0;
}
