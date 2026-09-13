#include "../Main5.2_RISE/RISE/PegasusRainingArrowMath.h"

#include <cmath>
#include <cstdio>

namespace {
bool Near(float left, float right)
{
    return std::fabs(left - right) < 0.0001f;
}
}

int main()
{
    using namespace rise::pegasus;
    static_assert(kRainingArrowFollowLifetime == 16,
        "Pegasus follow lifetime drift");
    static_assert(kRainingArrowDamageSpriteCount == 2,
        "Pegasus per-tick emitter count drift");
    static_assert(kRainingArrowDamageAtlasColumns == 4 &&
        kRainingArrowDamageAtlasFrames == 16,
        "Pegasus damage atlas contract drift");
    static_assert(kRainingArrowFirstPitch == 270.0f &&
        kRainingArrowFirstYaw == 180.0f &&
        kRainingArrowFirstBankMin == -10.0f &&
        kRainingArrowFirstBankMax == 10.0f,
        "Pegasus first-volley angle drift");
    static_assert(kRainingArrowVolleyOffsetMin == -200.0f &&
        kRainingArrowVolleyOffsetMax == 200.0f &&
        kRainingArrowVolleyHeight == 500.0f,
        "Pegasus later-volley envelope drift");

    if (RainingArrowDamageFrame(16.0f, 16.0f) != 0 ||
        RainingArrowDamageFrame(8.0f, 16.0f) != 8 ||
        RainingArrowDamageFrame(0.0f, 16.0f) != 15 ||
        !Near(RainingArrowDamageLight(16.0f, 16.0f), 1.0f) ||
        !Near(RainingArrowDamageLight(8.0f, 16.0f), 0.5f) ||
        !Near(RainingArrowDamageLight(0.0f, 16.0f), 0.0f) ||
        !Near(RainingArrowDamageScale(0), 1.8f) ||
        !Near(RainingArrowDamageScale(1000), 3.6f) ||
        !Near(RainingArrowPulse(25.0f, 25.0f), 0.0f) ||
        !Near(RainingArrowPulse(12.5f, 25.0f), 1.0f) ||
        !Near(RainingArrowPulse(0.0f, 25.0f), 0.0f) ||
        !Near(RainingArrowPulse(30.0f, 20.0f), 0.0f) ||
        !Near(RainingArrowPulse(50.0f - 30.0f, 20.0f), 0.0f) ||
        !Near(RainingArrowPulse(40.0f - 30.0f, 20.0f), 1.0f) ||
        !Near(RainingArrowPulse(30.0f - 30.0f, 20.0f), 0.0f) ||
        !Near(RainingArrowVolleyPitch(500.0f, 0.0f, 500.0f), 45.0f) ||
        !Near(RainingArrowVolleyPitch(100.0f, 0.0f, 100.0f), 45.0f) ||
        !Near(RainingArrowVolleyPitch(10.0f, 0.0f, 0.5f), 90.0f))
        return 1;

    std::puts("PASS: Raining Arrow damage-sheet, impact pulse and volley math match Pegasus");
    return 0;
}
