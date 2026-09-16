#pragma once

#include "S21CastAnimationSpeed.h"

namespace rise { namespace growlancer {

struct S21CastSpeedPair
{
    float attack;
    float magic;
};

// Source-domain return values of C5B813(type1) and B60918(id1008).
// This does not identify native 5.2 stat/buff fields or authorize class7.
constexpr S21CastSpeedPair ComposeS21CastSpeedPair(unsigned short sourceAttack,
    unsigned short sourceMagic, int managerType1Bonus, int lookup1008Bonus,
    unsigned char sourceFlags, bool tornadoPresent, bool hasCap, float cap)
{
    const float attackAfterBonuses =
        static_cast<float>(sourceAttack) + static_cast<float>(managerType1Bonus) +
        static_cast<float>(lookup1008Bonus);
    const float magicAfterBonuses =
        static_cast<float>(sourceMagic) + static_cast<float>(managerType1Bonus) +
        static_cast<float>(lookup1008Bonus);
    return {S21AdjustCastSpeedTail(attackAfterBonuses, sourceFlags,
                tornadoPresent, hasCap, cap),
            S21AdjustCastSpeedTail(magicAfterBonuses, sourceFlags,
                tornadoPresent, hasCap, cap)};
}

}}
