#include "../Main5.2_RISE/RISE/PegasusDexBoosterContract.h"
#include <cmath>
#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    static_assert(kDexBoosterAuraLifetime == 80.0f, "aura lifetime drift");
    static_assert(kDexBoosterGroundLifetime == 80.0f, "ground lifetime drift");
    static_assert(kDexBoosterFlareLifetime == 30.0f, "flare lifetime drift");
    static_assert(kDexBoosterGroundStartLight == 0.5f, "ground light drift");
    static_assert(kDexBoosterGroundAlphaStep == 0.00625f &&
        kDexBoosterGroundRotation9 == 10.0f &&
        kDexBoosterGroundRotation10 == 5.0f &&
        kDexBoosterGroundRotation11 == 15.0f,
        "ground alpha/rotation drift");
    static_assert(kDexBoosterAuraRotation == 0.08f,
        "aura sprite rotation drift");
    static_assert(kDexBoosterFlareScale == 0.9f, "flare scale drift");
    static_assert(kDexBoosterFlareParticleSubtype == 5 &&
        kDexBoosterFlareParticleBaseScale == 3.5f,
        "flare-particle route drift");
    static_assert(kDexBoosterInitialFlareJointCount == 19,
        "initial flare-joint count drift");
    static_assert(kDexBoosterInitialImpactParticleCount == 20,
        "initial impact-particle count drift");
    static_assert(kDexBoosterFlareJointSubtype == 48,
        "flare-joint subtype drift");
    static_assert(kDexBoosterFlareJointScale == 40.0f,
        "flare-joint scale drift");
    static_assert(kDexBoosterFlareJointRuntimeScale == 30.0f &&
        kDexBoosterFlareJointLifetime == 20.0f &&
        kDexBoosterFlareJointMaxTails == 80,
        "flare-joint runtime contract drift");
    static_assert(kDexBoosterFlareJointLightR == 1.0f &&
        kDexBoosterFlareJointLightG == 0.75f &&
        kDexBoosterFlareJointLightB == 0.3f,
        "flare-joint light drift");
    static_assert(kDexBoosterImpactParticleScale == 0.005f,
        "impact-particle scale drift");
    static_assert(kDexBoosterImpactParticleLifetime == 60.0f &&
        kDexBoosterImpactFadeStart == 30.0f &&
        kDexBoosterImpactPeakAlpha == 0.7f &&
        kDexBoosterImpactStopAlpha == 0.1f,
        "impact-particle lifetime/alpha drift");
    static_assert(kDexBoosterDurationSeconds == 300 &&
        HighSkillBuffDurationSeconds(kDexBoosterSkill) == 300,
        "Dex Booster server duration drift");
    const float target[3] = { 10.0f, 20.0f, 30.0f };
    float position[3] = {};
    ComputeDexBoosterFlareJointPosition(0.0, 0, target, position);
    if (std::fabs(position[0] - 10.0f) > 0.001f ||
        std::fabs(position[1] - 20.0f) > 0.001f ||
        std::fabs(position[2] - 225.0f) > 0.001f)
    {
        std::puts("FAIL: Dex Booster flare-joint orbit origin drift");
        return 1;
    }
    std::puts("PASS: Dex Booster constructor, ground and particle contract");
    return 0;
}
