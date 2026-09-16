#include "../Main5.2_RISE/RISE/GrowLancerRuntimeCapacity.h"
using namespace rise::growlancer;
constexpr bool TestBodyCapacity()
{
    constexpr int legacy = 10000;
    for (unsigned part = 0; part < 5; ++part)
    {
        const int id = ClassBodyModelId(part, legacy);
        if (id != legacy + 32 + static_cast<int>(part) || !IsRuntimeModelId(id, legacy)) return false;
    }
    return ClassBodyModelId(5, legacy) == -1 && ClassBodyModelId(~0u, legacy) == -1
        && RuntimeModelLimit(legacy) == legacy + 37
        && !IsRuntimeModelId(legacy + 37, legacy)
        && IsRuntimeModelId(legacy + 30, legacy);
}
static_assert(TestBodyCapacity(), "body range disjoint from existing skill tail");
