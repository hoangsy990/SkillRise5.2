#define MAX_MODELS 1000
#include "../Main5.2_RISE/RISE/PegasusDeathsideRuntime.h"

#include <cassert>
#include <iostream>

int main()
{
    using namespace rise::pegasus;
    static_assert(DeathsideAttackPlaySpeed(0) == 0.02f &&
        DeathsideAttackPlaySpeed(1) == 0.02f, "single-key action fallback drift");
    static_assert(DeathsideAttackPlaySpeed(20) > 1.19f &&
        DeathsideAttackPlaySpeed(20) < 1.20f, "recovered attack duration drift");
    static_assert(kDeathsideReaperModel == MAX_MODELS + 47, "Reaper ID drift");
    static_assert(kDeathsideLastModel == MAX_MODELS + 51, "model tail drift");
    static_assert(kDeathsideRootHeight == 80.0f, "0xB4F45C drift");
    static_assert(kDeathsideFollowFactor == 0.056f, "0xB6B0E8 drift");
    static_assert(kDeathsideFollowStartDistance == 10.0f,
        "0xB4AE00 drift");
    static_assert(kDeathsideFollowStopDistance == 20.0f,
        "0xB4AE14 drift");
    static_assert(kDeathsideTurnStep == 20.0f, "turn-step drift");
    static_assert(kDeathsideTrailStartFrame == 1.5f,
        "0xB4B008 trail threshold drift");
    static_assert(kDeathsideTrailLight == 0.32f,
        "0x65F0D7 trail light drift");
    static_assert(kDeathsideTrailLifetime == 100,
        "0x65F1D5 object-blur lifetime drift");
    static_assert(kDeathsideMirroredTailLifetime == -1,
        "0x65F47A default object-blur lifetime drift");
    static_assert(kDeathsideParticleHeight == 130.0f, "0xB4F49C drift");
    static_assert(kDeathsideActiveMilliseconds == 10000, "0xB6B120 drift");
    static_assert(kDeathsideScytheLifetime == 60.0f,
        "0xB4AE2C scythe lifetime drift");
    static_assert(kDeathsideDisappearAction == 6, "disappear action drift");
    static_assert(kDeathsideDisappearLifetime == 20.0f, "disappear lifetime drift");
    static_assert(kDeathsideInitialAlpha == 0.2f, "initial alpha drift");
    static_assert(kDeathsideDisappearAlpha == 0.5f,
        "disappear alpha drift");
    static_assert(kDeathsideGroundSmokeCadence == 13, "ground smoke cadence drift");
    static_assert(kDeathsideSmokeCadence == 10, "upper smoke cadence drift");
    static_assert(kDeathsideWaterfallCadence == 34, "waterfall cadence drift");
    static_assert(kDeathsideGroundSmokeSubtype == 1, "ground smoke subtype drift");
    static_assert(kDeathsideSmokeSubtype == 0x74, "upper smoke subtype drift");
    static_assert(kDeathsideIdleAction == 7, "idle action drift");
    static_assert(kDeathsidePostAttackAction == 0,
        "0x65FB0B post-attack action drift");
    static_assert(kDeathsideMinimumPlaySpeed == 0.02f,
        "0xB55564 minimum play speed drift");
    static_assert(kDeathsideAttackActions[0] == 3 &&
        kDeathsideAttackActions[1] == 4 && kDeathsideAttackActions[2] == 8,
        "0xB6B0CC attack action table drift");
    static_assert(kDeathsideCasterSummonActions[0] == 172 &&
        kDeathsideCasterSummonActions[1] == 173 &&
        kDeathsideCasterSummonActions[2] == 174 &&
        kDeathsideCasterSummonActions[3] == 175,
        "0x6604F2 first-cast player action family drift");
    static_assert(kDeathsideCasterRepeatAction == 185,
        "0x6604A3 repeat-cast player action drift");
    assert(kDeathsideLastModel - kDeathsideFirstModel + 1 == 5);
    std::cout << "PASS: Deathside Reaper root/model/action contract" << std::endl;
    return 0;
}
