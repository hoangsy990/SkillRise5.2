#include "../Main5.2_RISE/RISE/PegasusElementalChargeMath.h"
#include <cmath>
#include <cstdio>

namespace {
bool Near(float left, float right)
{
    return std::fabs(left - right) <= 0.00001f;
}
}

int main()
{
    using namespace rise::pegasus;
    static_assert(kElementalLayerCount == 5, "Elemental layer count drifted");
    static_assert(kElementalCastSubtype == 0 &&
        kElementalBuffFallbackSubtype == 1,
        "Elemental cast/buff ownership drifted");
    static_assert(kElementalConstructorChildCount == 5,
        "Elemental constructor child count drifted");
    static_assert(kElementalSourceFlare03Effect == 0x7F20 &&
        kElementalSourceShockwaveEffect == 0x7F21,
        "Elemental source child-effect roles drifted");
    static_assert(kElementalConstructorChildRoles[0] == 0x7F20 &&
        kElementalConstructorChildRoles[1] == 0x7F20 &&
        kElementalConstructorChildRoles[2] == 0x7F20 &&
        kElementalConstructorChildRoles[3] == 0x7F21 &&
        kElementalConstructorChildRoles[4] == 0x7F21,
        "Elemental constructor child-effect order drifted");
    static_assert(kElementalConstructorChildSubtypes[0] == 0 &&
        kElementalConstructorChildSubtypes[2] == 0 &&
        kElementalConstructorChildSubtypes[3] == 1 &&
        kElementalConstructorChildSubtypes[4] == 1,
        "Elemental constructor child-effect subtypes drifted");
    static_assert(kElementalConstructorChildScales[0] == 3.0f &&
        kElementalConstructorChildScales[1] == 3.0f &&
        kElementalConstructorChildScales[2] == 4.0f &&
        kElementalConstructorChildScales[3] == 1.5f &&
        kElementalConstructorChildScales[4] == 1.5f,
        "Elemental constructor child-effect scales drifted");
    static_assert(kElementalFlare03Lifetime == 50.0f &&
        kElementalShockwaveLifetime == 40.0f &&
        kElementalShockwaveScale == 1.5f &&
        kElementalShockwaveGrowthStart == 0.1f,
        "Elemental child-effect initialization drifted");
    static_assert(kElementalBones[0] == 3 && kElementalBones[1] == 6 &&
        kElementalBones[2] == 9 && kElementalBones[3] == 12 &&
        kElementalBones[4] == 15, "Elemental bone order drifted");
    static_assert(kElementalSourceBitmapRoles[0] == 0x7F22 &&
        kElementalSourceBitmapRoles[4] == 0x7F26,
        "Elemental bitmap-role order drifted");
    static_assert(kElementalPulseRates[0] == 0.009f &&
        kElementalPulseRates[4] == 0.005f,
        "Elemental pulse table drifted");

    if (!Near(ElementalPulse(0.0f, 0), 0.0f) ||
        !Near(ElementalPulse(100.0f, 0), std::fabs(std::sin(0.9f))) ||
        !Near(ElementalPulse(100.0f, 4), std::fabs(std::sin(0.5f))))
        return 1;
    if (ShouldCreateElementalBuffFallback(true) ||
        !ShouldCreateElementalBuffFallback(false)) return 2;

    std::puts("PASS: Elemental Charge ownership, five-layer bone, bitmap and pulse provenance");
}
