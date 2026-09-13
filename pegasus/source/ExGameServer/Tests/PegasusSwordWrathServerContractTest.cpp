#include "../../Shared/PegasusSwordWrathServerContract.h"

#include <cstdio>

int main()
{
    using namespace rise::pegasus;

    static_assert(kSwordWrathRangeBonus == 1, "Sword Wrath range bonus drifted");
    static_assert(kSwordWrathDamageRadiusBonus == 1,
        "Sword Wrath damage-radius bonus drifted");
    static_assert(SwordWrathEffectiveRange(5, false) == 5,
        "inactive marker changed attack range");
    static_assert(SwordWrathEffectiveRange(5, true) == 6,
        "active marker did not add one attack range");
    static_assert(SwordWrathEffectiveDamageRadius(3, false) == 3,
        "inactive marker changed damage radius");
    static_assert(SwordWrathEffectiveDamageRadius(3, true) == 4,
        "active marker did not add one damage radius");
    static_assert(!IsWithinPegasusDistance(0, 0, 6, 0,
        SwordWrathEffectiveRange(5, false)), "inactive range accepted bonus tile");
    static_assert(IsWithinPegasusDistance(0, 0, 6, 0,
        SwordWrathEffectiveRange(5, true)), "active range rejected bonus tile");
    static_assert(!IsWithinPegasusDistance(10, 10, 14, 10,
        SwordWrathEffectiveDamageRadius(3, false)),
        "inactive radius accepted bonus tile");
    static_assert(IsWithinPegasusDistance(10, 10, 14, 10,
        SwordWrathEffectiveDamageRadius(3, true)),
        "active radius rejected bonus tile");

    std::puts("PASS: Sword Wrath applies exact +1 range and +1 damage radius");
    return 0;
}
