#include "../Main5.2_RISE/RISE/PegasusSkillIconCatalog.h"

#include <cassert>
#include <cstring>

int main()
{
    using namespace rise::pegasus;
    assert(kHighSkillIconRouteCount == kHighSkillSeedCount);
    for (std::size_t i = 0; i < kHighSkillSeedCount; ++i)
    {
        const HighSkillIconRoute* route = FindHighSkillIconRoute(kHighSkillSeeds[i].skillId);
        assert(route != nullptr);
        assert(route->bitmapId >= kPegasusSkillIconBegin);
        assert(route->bitmapId < kPegasusSkillIconEnd);
        assert(std::strstr(route->relativeBitmapPath, "RISE\\Pegasus\\UI\\Skill\\") == route->relativeBitmapPath);
        assert(std::strstr(route->relativeBitmapPath, ".jpg") != nullptr);
    }
    const HighSkillIconRoute* swordBlow = FindHighSkillIconRoute(kSwordBlowSkill);
    const HighSkillIconRoute* fly = FindHighSkillIconRoute(kSwordBlowFlySkill);
    assert(swordBlow != nullptr && fly == swordBlow);
    assert(FindHighSkillIconRoute(1) == nullptr);
    return 0;
}
