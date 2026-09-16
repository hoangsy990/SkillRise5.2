#include "../Main5.2_RISE/RISE/GrowLancerMagicPinTick.h"
#include "../../GrowLancer/compat/S21CastAnimationSpeed.h"
using namespace rise::growlancer;
// Historical PID16052 first recorded cast, before-controller-update frames.
// This regression reproduces a missed gate, NOT a proposed catch-up policy.
constexpr bool RecordedMiss()
{
    const float frames[] = {.882826567f,1.471298814f,2.353149176f,2.941817284f,
        3.530900240f,4.412719250f,5.001288414f,5.883203983f,6.472346783f,
        7.354052544f,7.943107128f,8.532241821f,9.415320396f,10.004589081f,
        10.591607094f,11.472456932f};
    int stage = 0;
    for (float frame : frames)
        if (MagicPinStageDue(stage,frame)) ++stage;
    return stage == 0;
}
static_assert(RecordedMiss(), "observed stage0 remains blocked even if later stage windows are sampled");
static_assert(MagicPinStageDue(0,5.2f) && MagicPinStageDue(0,5.8f), "inclusive source bounds");
static_assert(!MagicPinStageDue(0,5.001288414f) && !MagicPinStageDue(0,5.883203983f), "recorded gap");
static_assert(MagicPinStageDue(1,7.354052544f) && MagicPinStageDue(2,10.004589081f), "later windows do not bypass stage0");

// Conditional model only: frame starts at zero, constant adjusted speed,
// one animation advance followed by one controller sample per source tick.
// These assumptions are NOT a captured S21 cast or a native scheduling fix.
constexpr int SynchronizedStages(float adjustedSpeed)
{
    float frame = 0.f;
    int stage = 0;
    for (int tick = 0; tick < 100 && frame < 11.f; ++tick)
    {
        frame += S21CastAnimationSpeed(188, adjustedSpeed);
        if (MagicPinStageDue(stage, frame)) ++stage;
    }
    return stage;
}
static_assert(SynchronizedStages(0.f) == 3, "zero adjusted speed samples all windows");
static_assert(SynchronizedStages(100.f) == 3, "intermediate adjusted speed samples all windows");
static_assert(SynchronizedStages(S21GrowLancerInitialSpeedLimit) == 2,
    "synchronization alone does not guarantee third gate at the initial source speed cap");
