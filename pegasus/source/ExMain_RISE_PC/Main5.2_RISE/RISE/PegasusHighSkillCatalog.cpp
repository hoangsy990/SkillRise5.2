#include "stdafx.h"
#include "PegasusHighSkillCatalog.h"
#include "ZzzInfomation.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"
#include "../../../Shared/PegasusSwordBlowProtocol.h"

namespace rise { namespace pegasus {
void ApplyHighSkillCatalog()
{
    static_assert(MAX_SKILL_TYPES > 2024, "high skill ID capacity too small");
    static_assert(MAX_CLASS == 7, "Pegasus class contract requires seven classes");
    for (const HighSkillSeed& seed : kHighSkillSeeds) {
        SKILL_ATTRIBUTE& value = SkillAttribute[seed.skillId];
        ZeroMemory(&value, sizeof(value));
        strcpy_s(value.Name, seed.name);
        value.Level = static_cast<WORD>(seed.level);
        value.Damage = static_cast<WORD>(seed.damage);
        value.Mana = static_cast<WORD>(seed.mana);
        value.AbilityGuage = static_cast<WORD>(seed.abilityGauge);
        value.Distance = seed.distance;
        value.Delay = seed.delay;
        value.Energy = seed.energy;
        value.MasteryType = 255;
        memcpy(value.RequireClass, seed.requiredClass, sizeof(value.RequireClass));
        value.IsDamage = seed.isDamage;
        value.Magic_Icon = seed.icon;
    }
#if defined(PEGASUS_ISOLATED_TEST)
    // Sword Blow Fly is an isolated custom skill, not a fourteenth Pegasus
    // catalog row. Clone the gameplay/UI requirements without mutating 1501.
    SKILL_ATTRIBUTE& fly = SkillAttribute[kSwordBlowFlySkill];
    fly = SkillAttribute[kSwordBlowSkill];
    strcpy_s(fly.Name, "Sword Blow Fly");
#endif
}
}}
