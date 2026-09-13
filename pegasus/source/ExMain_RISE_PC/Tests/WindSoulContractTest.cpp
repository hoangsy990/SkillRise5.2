#include "../Main5.2_RISE/RISE/WindSoulContract.h"
#include <cassert>

int main()
{
    using rise::windsoul::Base737Contract;
    static_assert(Base737Contract::SkillId == 737, "identity drift");
    static_assert(Base737Contract::RootCount == 3, "root-count drift");
    static_assert(Base737Contract::RootSubtype == 4, "subtype drift");
    static_assert(Base737Contract::RootScale == 60.f, "scale drift");
    assert(Base737Contract::RandomXBase == -119);
    assert(Base737Contract::RandomXBase + Base737Contract::RandomXModulo - 1 == 120);
    assert(Base737Contract::RandomZBase == 49);
    assert(Base737Contract::RandomZBase + Base737Contract::RandomZModulo - 1 == 108);
    assert(Base737Contract::LocalLaunchY == 60.f);
    assert(Base737Contract::InitialTargetZOffset == 90.f);
    static_assert(Base737Contract::SmokeSubtype == 0x50, "S21 smoke subtype drift");
    static_assert(Base737Contract::MaxTails == 12, "tail-count drift");
    static_assert(Base737Contract::LifeTime == 35.f, "lifetime drift");
    static_assert(Base737Contract::InitialVelocity == 10.f, "velocity drift");
    static_assert(Base737Contract::DirectionY == -15.f, "direction drift");
    static_assert(Base737Contract::TargetZOffset == 100.f, "target-height drift");
    static_assert(Base737Contract::JointLightR == 0.4f &&
        Base737Contract::JointLightG == 0.6f &&
        Base737Contract::JointLightB == 0.7f, "joint-color drift");
    static_assert(Base737Contract::SmokeLightR == 0.2f &&
        Base737Contract::SmokeLightG == 0.65f &&
        Base737Contract::SmokeLightB == 0.75f, "smoke-color drift");
    static_assert(Base737Contract::PitchThreshold == -90.f,
        "pitch threshold drift");
    static_assert(Base737Contract::PitchJitterBase == 12 &&
        Base737Contract::PitchJitterModulo == 16, "pitch jitter drift");
    static_assert(Base737Contract::ImpactDistance == 100.f &&
        Base737Contract::ImpactLife == 5, "impact gate drift");
    return 0;
}
