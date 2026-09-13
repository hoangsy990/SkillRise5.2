#pragma once

#include "PegasusTextureIds.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"
#include "../../../Shared/PegasusSwordBlowProtocol.h"

#include <cstddef>

namespace rise { namespace pegasus {

struct HighSkillIconRoute
{
    int skillId;
    int bitmapId;
    const char* relativeBitmapPath;
};

// These are the individual 80x112 UI icons named by the hash-pinned Pegasus
// Skill.xml. The original PNG bytes stay hash-pinned by the staging test. The
// isolated copy uses maximum-quality JPEG because this RISE 5.2 loader accepts
// JPEG but rejected every staged TGA at runtime.
static constexpr HighSkillIconRoute kHighSkillIconRoutes[] = {
    {kShiningBirdSkill,     kPegasusSkillIconBegin + 0,  "RISE\\Pegasus\\UI\\Skill\\241.jpg"},
    {kDragonViolentSkill,   kPegasusSkillIconBegin + 1,  "RISE\\Pegasus\\UI\\Skill\\242.jpg"},
    {kSpearstormSkill,      kPegasusSkillIconBegin + 2,  "RISE\\Pegasus\\UI\\Skill\\243.jpg"},
    {kDeathsideSkill,       kPegasusSkillIconBegin + 3,  "RISE\\Pegasus\\UI\\Skill\\288.jpg"},
    {kDarknessSkill,        kPegasusSkillIconBegin + 4,  "RISE\\Pegasus\\UI\\Skill\\289.jpg"},
    {kSwordWrathSkill,      kPegasusSkillIconBegin + 5,  "RISE\\Pegasus\\UI\\Skill\\1500.jpg"},
    {kSwordBlowSkill,       kPegasusSkillIconBegin + 6,  "RISE\\Pegasus\\UI\\Skill\\1501.jpg"},
    {kChaosBladeSkill,      kPegasusSkillIconBegin + 7,  "RISE\\Pegasus\\UI\\Skill\\2012.jpg"},
    {kHavocSpearSkill,      kPegasusSkillIconBegin + 8,  "RISE\\Pegasus\\UI\\Skill\\2013.jpg"},
    {kSpiralChargeSkill,    kPegasusSkillIconBegin + 9,  "RISE\\Pegasus\\UI\\Skill\\2014.jpg"},
    {kCrusherChargeSkill,   kPegasusSkillIconBegin + 10, "RISE\\Pegasus\\UI\\Skill\\2015.jpg"},
    {kElementalChargeSkill, kPegasusSkillIconBegin + 11, "RISE\\Pegasus\\UI\\Skill\\2016.jpg"},
    {kRainingArrowSkill,    kPegasusSkillIconBegin + 12, "RISE\\Pegasus\\UI\\Skill\\2023.jpg"},
    {kDexBoosterSkill,      kPegasusSkillIconBegin + 13, "RISE\\Pegasus\\UI\\Skill\\2024.jpg"},
};

static constexpr std::size_t kHighSkillIconRouteCount =
    sizeof(kHighSkillIconRoutes) / sizeof(kHighSkillIconRoutes[0]);

inline const HighSkillIconRoute* FindHighSkillIconRoute(int skillId)
{
    // Sword Blow Fly is a visual/action clone and intentionally shares the
    // source-backed Sword Blow icon while retaining its distinct skill name.
    if (skillId == kSwordBlowFlySkill)
        skillId = kSwordBlowSkill;

    for (const HighSkillIconRoute& route : kHighSkillIconRoutes)
        if (route.skillId == skillId)
            return &route;
    return nullptr;
}

}}
