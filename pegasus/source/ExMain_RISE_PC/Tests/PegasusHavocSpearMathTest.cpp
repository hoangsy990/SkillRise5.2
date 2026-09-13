#include "../Main5.2_RISE/RISE/PegasusHavocSpearMath.h"
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
    static_assert(kHavocLinePlaySpeed == 1.0f,
        "0x66664F line BMD play speed drift");
    static_assert(kHavocShockwavePlaySpeed == 0.3f,
        "0x66653D shockwave BMD play speed drift");
    static_assert(kHavocNovaLifetime == 20.0f &&
        kHavocLineLifetime == 30.0f &&
        kHavocShockwaveLifetime == 12.0f &&
        kHavocEndLifetime == 24.0f &&
        kHavocNovaModelHeight == 234.0f,
        "Havoc constructor provenance drifted");

    if (!Near(HavocLifetimeAlpha(20.0f, 20.0f), 1.0f) ||
        !Near(HavocLifetimeAlpha(10.0f, 20.0f), 0.5f) ||
        !Near(HavocLifetimeAlpha(-1.0f, 20.0f), 0.0f)) return 1;
    if (!Near(HavocNovaHeight(100.0f, 0.25f, 20.0f), 51.5f) ||
        !Near(HavocNovaHeight(100.0f, 0.25f, 15.0f), 110.0f) ||
        !Near(HavocNovaHeight(100.0f, 0.25f, 0.0f), 110.0f)) return 2;
    if (!Near(HavocShockwaveScaleStep(1.6f, 1.6f, 1.0f), 2.4f) ||
        !Near(HavocEndAlpha(24.0f), 1.0f) ||
        !Near(HavocEndAlpha(18.0f), 1.0f) ||
        !Near(HavocEndAlpha(9.0f), 0.5f) ||
        !Near(HavocEndAlpha(0.0f), 0.0f)) return 3;

    if (!Near(HavocTargetScale(0.5f), 0.8f) ||
        !Near(HavocTargetScale(1.2f), 1.2f) ||
        !Near(HavocTargetScale(3.0f), 2.5f)) return 4;
    if (sizeof(kHavocRootJointSpecs) / sizeof(kHavocRootJointSpecs[0]) != 6 ||
        !Near(HavocRootJointScale(kHavocRootJointSpecs[0]), 32.4f) ||
        !Near(HavocRootJointScale(kHavocRootJointSpecs[1]), 30.0f) ||
        !Near(HavocRootJointScale(kHavocRootJointSpecs[2]), 24.0f) ||
        !Near(HavocRootJointScale(kHavocRootJointSpecs[3]), 58.5f) ||
        kHavocRootJointSpecs[1].subtype != 1 ||
        !Near(kHavocRootJointSpecs[0].red, 0.06f) ||
        !Near(kHavocRootJointSpecs[5].green, 0.75f)) return 5;

    int rootBursts = 0;
    int oddBursts = 0;
    for (int tick = 0; tick <= 17; ++tick)
    {
        rootBursts += HavocCreatesRootBurstAtTick(tick) ? 1 : 0;
        oddBursts += HavocCreatesOddBurstAtTick(tick) ? 1 : 0;
    }
    if (kHavocRootLastTick != 15 || rootBursts != 1 || oddBursts != 8 ||
        !HavocCreatesRootBurstAtTick(1) ||
        !HavocCreatesOddBurstAtTick(1) ||
        !HavocCreatesOddBurstAtTick(3) ||
        !HavocCreatesOddBurstAtTick(15) ||
        HavocCreatesOddBurstAtTick(0) ||
        HavocCreatesOddBurstAtTick(2) ||
        HavocCreatesOddBurstAtTick(17)) return 6;

    std::puts("PASS: Havoc Spear constructor, scale, six joints, tick-1 Nova, movement and fade provenance");
}
