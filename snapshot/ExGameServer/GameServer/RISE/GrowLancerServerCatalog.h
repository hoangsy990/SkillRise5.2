#pragma once

#include "../../../Shared/GrowLancerSkillContractData.h"

namespace rise { namespace growlancer {

inline void ApplyServerCatalog(std::map<int, SKILL_INFO>& catalog)
{
    for (unsigned i = 0; i < SkillSeedCount(); ++i)
    {
        const SkillSeed& seed = kSkillSeeds[i];
        SKILL_INFO info = {};
        info.Index = seed.id;
        strncpy_s(info.Name, sizeof(info.Name), seed.name, _TRUNCATE);
        info.Damage = seed.damage;
        info.Mana = seed.mana;
        info.BP = seed.bp;
        info.Range = seed.range;
        info.Radio = 0;
        info.Delay = seed.delayMs;
        info.Type = seed.type;
        info.Effect = seed.buffEffect;
        info.RequireLevel = seed.requiredLevel;
        info.RequireEnergy = seed.requiredEnergy;
        info.RequireLeadership = 0;
        info.RequireKillCount = 0;
        info.RequireGuildStatus = 0;

        // SS6 owns only seven classes and has no Grow Lancer slot. Keeping all
        // class columns zero makes learning/casting fail closed until an
        // owner-selected new native Grow Lancer class is implemented and its
        // numeric class/packet/persistence contract is verified. DK is QA-only.
        for (int n = 0; n < MAX_CLASS; ++n)
            info.RequireClass[n] = 0;

        catalog[seed.id] = info;
    }
}

} }
