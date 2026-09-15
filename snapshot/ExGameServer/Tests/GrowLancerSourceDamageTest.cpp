#include "../GameServer/RISE/GrowLancerSourceDamage.h"

using rise::growlancer::ComputeSourceDamage;

constexpr auto base = ComputeSourceDamage(30, 30, 24);
static_assert(base.physicalMinLeft == 6.75 &&
    base.physicalMinRight == base.physicalMinLeft,
    "S21 GL fist min uses Strength/8 + Dexterity/10");
static_assert(base.physicalMaxLeft == 12.5 &&
    base.physicalMaxRight == base.physicalMaxLeft,
    "S21 GL fist max uses Strength/4 + Dexterity/6");
static_assert(base.magicMin == 24.0 / 9.0 &&
    base.magicMax == 6.0,
    "S21 GL magic uses Energy/9 and Energy/4");
static_assert(base.physicalMinLeft != static_cast<int>(base.physicalMinLeft),
    "integer bridge must not be hidden in source formula");

int main() { return 0; }
