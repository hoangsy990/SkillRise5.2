#include "../Main5.2_RISE/RISE/PegasusSpearstormMath.h"
#include <cmath>
#include <cstdio>

namespace {
bool Near(float left, float right, float epsilon = 0.0001f)
{
    return std::fabs(left - right) <= epsilon;
}
}

int main()
{
    using namespace rise::pegasus;
    static_assert(kSpearstormRootLifetime == 5.0f &&
        kSpearstormBladeLifetime == 72.0f &&
        kSpearstormImpactLifetime == 66.0f &&
        kSpearstormHitSoundLifetime == 64.0f &&
        kSpearstormCrackLifetime == 50.0f &&
        kSpearstormSpawnLeadTicks == 6.0f,
        "Spearstorm lifetime provenance drifted");
    static_assert(kSpearstormStoneChildCount == 5 &&
        kSpearstormImpactSmokeCount == 15 &&
        kSpearstormImpactScatter == 40,
        "Spearstorm impact fan provenance drifted");

    float direction[3];
    SpearstormBladeDirection(60.0f, 0.0f, direction);
    if (!Near(direction[0], 0.0f) || !Near(direction[1], -100.0f) ||
        !Near(direction[2], -173.20508f)) return 1;
    SpearstormBladeDirection(60.0f, 10.0f, direction);
    if (!Near(direction[0], -30.076747f) || !Near(direction[1], -100.0f) ||
        !Near(direction[2], -170.57370f)) return 3;
    // Authored 60..70 degree pitch and 0..10 yaw must descend steeply.
    for (int pitch = 60; pitch <= 70; ++pitch)
        for (int yaw = 0; yaw <= 10; ++yaw)
        {
            SpearstormBladeDirection(float(pitch), float(yaw), direction);
            if (direction[2] > -170.0f) return 7;
        }
    if (!Near(SpearstormLayerProgress(49.0f, 20.0f), 0.1f) ||
        !Near(SpearstormLayerProgress(29.0f, 20.0f), 1.1f)) return 2;
    if (!Near(SpearstormBladeAlpha(0.7f), 1.0f) ||
        !Near(SpearstormBladeAlpha(0.35f), 0.5f) ||
        !Near(SpearstormBladeAlpha(-0.1f), 0.0f)) return 4;
    if (!SpearstormShouldImpact(66.5f, 1.0f, 100.0f, 101.0f) ||
        SpearstormShouldImpact(67.5f, 1.0f, 100.0f, 101.0f) ||
        !SpearstormShouldImpact(70.0f, 1.0f, 100.0f, 99.0f)) return 5;
    if (!SpearstormCrossesLifetime(64.5f, 1.0f, 64.0f) ||
        SpearstormCrossesLifetime(63.5f, 1.0f, 64.0f) ||
        SpearstormCrossesLifetime(65.5f, 1.0f, 64.0f)) return 6;

    std::puts("PASS: Spearstorm vector, forced impact, sound crossing, fan and layers");
}
