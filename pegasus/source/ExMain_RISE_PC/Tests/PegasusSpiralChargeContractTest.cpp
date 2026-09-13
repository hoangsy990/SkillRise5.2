#include "../Main5.2_RISE/RISE/PegasusSpiralChargeContract.h"
#include "../../Shared/PegasusHighSkillCatalogData.h"
#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    if (!SpiralChargeEmitterTick(39.75f, 0.25f) ||
        SpiralChargeEmitterTick(39.5f, 0.25f) ||
        SpiralChargeEmitterTick(40.0f, 0.25f) ||
        !SpiralChargeEmitterTick(40.0f, 1.0f) ||
        SpiralChargeEmitterTick(39.75f, 0.0f)) return 1;
    int emissions = 0;
    for (int frame = 0; frame < 160; ++frame)
        if (SpiralChargeEmitterTick(40.0f - frame * 0.25f, 0.25f))
            ++emissions;
    if (emissions != 40) return 2;
    static_assert(kSpiralChargeSourceSkill == kSpiralChargeSkill,
        "Spiral Charge source skill drift");
    static_assert(kSpiralChargeSourceBuff == 214 &&
        kSpiralChargeNativeEffect == 169,
        "Pegasus-to-RISE buff translation drift");
    static_assert(kSpiralChargeOwnerBone == 17 &&
        kSpiralChargeAnchorHeight == 10.0f &&
        kSpiralChargeAnchorCreateScale == 2.0f &&
        kSpiralChargeAnchorRuntimeScale == 1.5f,
        "persistent anchor contract drift");
    static_assert(kSpiralChargeRootLifetime == 30.0f &&
        kSpiralChargeFlareLifetime == 20.0f &&
        kSpiralChargeShockwaveLifetime == 20.0f &&
        kSpiralChargeEmitterLifetime == 40.0f &&
        kSpiralChargeShockwaveCount == 2,
        "cast-layer lifetime contract drift");
    static_assert(kSpiralChargeRootJointCount == 9 &&
        kSpiralChargeRootJointScale == 20.0f &&
        kSpiralChargeRootJointLifetime == 30.0f &&
        kSpiralChargeRootJointMaxTails == 5 &&
        kSpiralChargeRootJointActivationFrame == 0.75f,
        "root-bone joint contract drift");
    static_assert(kSpiralChargeRootAnimationSpeed == 0.3f &&
        kSpiralChargeSteamParticleSubtype == 4 &&
        kSpiralChargeSmokeParticleSubtype == 12 &&
        kSpiralChargeSteamParticleLifetime == 10.0f &&
        kSpiralChargeSmokeParticleLifetime == 12.0f &&
        kSpiralChargeSmokeFadeSplit == 6.0f,
        "animation/emitter contract drift");
    static_assert(kSpiralChargeAnchorBoneCount == 17 &&
        kSpiralChargeAnchorBones[0] == 12 &&
        kSpiralChargeAnchorBones[16] == 26,
        "anchor bone table drift");
    std::puts("PASS: Spiral Charge source-214/native-169 anchor contract");
    return 0;
}
