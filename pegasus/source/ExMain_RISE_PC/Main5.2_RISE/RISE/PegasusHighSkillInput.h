#pragma once

#include "../../../Shared/PegasusHighSkillCatalogData.h"
#include "../../../Shared/PegasusSwordBlowProtocol.h"

namespace rise { namespace pegasus {

constexpr bool IsPegasusHighSkillInput(int skillId)
{
    return FindHighSkillById(skillId) != nullptr ||
        skillId == kSwordBlowFlySkill;
}

constexpr bool IsPegasusSelfBuffInput(int skillId)
{
    return skillId == kDarknessSkill ||
        skillId == kDexBoosterSkill ||
        skillId == kSpiralChargeSkill ||
        skillId == kCrusherChargeSkill ||
        skillId == kElementalChargeSkill ||
        skillId == kSwordWrathSkill;
}

constexpr bool IsPegasusTargetAttackInput(int skillId)
{
    return IsPegasusHighSkillInput(skillId) && !IsPegasusSelfBuffInput(skillId);
}

}}
