#include "../../Shared/PegasusChargeRangeServerContract.h"
#include "../../Shared/PegasusSwordWrathServerContract.h"

#include <cstdio>

int main()
{
    using namespace rise::pegasus;

    static_assert(ChargeEffectiveCastRange(2012, 3, false, false, false) == 3,
        "Chaos Blade base range drifted");
    static_assert(ChargeEffectiveCastRange(2012, 3, true, false, false) == 6,
        "Spiral Charge must set Chaos Blade range to 6");
    static_assert(ChargeEffectiveCastRange(2012, 3, false, true, false) == 5,
        "Crusher Charge must set Chaos Blade range to 5");
    static_assert(ChargeEffectiveCastRange(2013, 6, false, false, true) == 7,
        "Elemental Charge must set Havoc Spear range to 7");
    static_assert(ChargeEffectiveCastRange(242, 6, true, true, true) == 6,
        "charge markers changed an unrelated skill");
    static_assert(IsWithinPegasusDistance(0, 0, 6, 0,
        ChargeEffectiveCastRange(2012, 3, true, false, false)),
        "Spiral range rejected its boundary tile");
    static_assert(!IsWithinPegasusDistance(0, 0, 7, 0,
        ChargeEffectiveCastRange(2012, 3, true, false, false)),
        "Spiral range accepted a tile beyond its boundary");
    static_assert(IsWithinPegasusDistance(0, 0, 7, 0,
        ChargeEffectiveCastRange(2013, 6, false, false, true)),
        "Elemental range rejected its boundary tile");

    static_assert(ChargeBuffWeaponAllowed(2014,
        PegasusChargeWeaponFamily::Sword, true),
        "Spiral Charge rejected a two-handed sword");
    static_assert(!ChargeBuffWeaponAllowed(2014,
        PegasusChargeWeaponFamily::Sword, false),
        "Spiral Charge accepted a one-handed sword");
    static_assert(ChargeBuffWeaponAllowed(2015,
        PegasusChargeWeaponFamily::Sword, false),
        "Crusher Charge rejected a one-handed sword");
    static_assert(!ChargeBuffWeaponAllowed(2015,
        PegasusChargeWeaponFamily::Staff, false),
        "Crusher Charge accepted a staff");
    static_assert(ChargeBuffWeaponAllowed(2016,
        PegasusChargeWeaponFamily::Staff, false) &&
        ChargeBuffWeaponAllowed(2016,
            PegasusChargeWeaponFamily::Staff, true),
        "Elemental Charge rejected a staff");
    static_assert(!ChargeBuffWeaponAllowed(2016,
        PegasusChargeWeaponFamily::Sword, true),
        "Elemental Charge accepted a sword");
    static_assert(ChargeBuffWeaponAllowed(2012,
        PegasusChargeWeaponFamily::None, false),
        "charge equipment contract changed an unrelated skill");

    std::puts("PASS: Pegasus charge markers apply exact ranges and weapon families");
    return 0;
}
