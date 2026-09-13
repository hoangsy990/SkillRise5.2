#include "../Main5.2_RISE/RISE/GrowLancerSpinCross.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>

using namespace rise::growlancer;
struct Record
{
    bool Live = true;
    float Timer = 0.f, LifeTime = 10.f, Alpha = 1.f;
    float Scale = 0.f, Distance = 0.f;
    int updates = 0, destructions = 0;
};
static void Near(float actual, float expected)
{
    assert(std::fabs(actual - expected) < .0001f);
}
int main()
{
    for (float scale : {12.f, 17.f})
    for (float factor : {.25f, .5f, 1.f, 2.f})
    {
        Record r;
        r.Scale = scale * .5f;
        r.Distance = scale;
        auto update = [scale](Record& v, float tick) {
            assert(tick == 1.f);
            UpdateSpinCrossTick(v);
            ++v.updates;
            if (v.updates == 1) Near(v.Scale, scale * .6f);
            if (v.updates == 5) Near(v.Scale, scale);
            if (v.updates == 6) Near(v.Scale, scale * .9f);
        };
        auto destroy = [](Record* v) {
            assert(v->LifeTime == 0.f && v->updates == 11);
            ++v->destructions;
            v->Live = false;
        };
        int frames = 0;
        while (r.Live && frames++ < 100)
            AdvanceWholeTicks(r, factor, update, destroy);
        assert(!r.Live && r.updates == 11 && r.destructions == 1);
        Near(r.Scale, scale * .4f);
        Near(r.Alpha, -.1f);
        AdvanceWholeTicks(r, 100.f, update, destroy);
        assert(r.updates == 11 && r.destructions == 1);
    }
    std::puts("PASS: Spin cross shared runtime math, 8 scale/FPS cases, grow/shrink threshold, zero-life tick, single destruction");
    std::puts("Scope excludes constructor, renderer, pool allocation and visual parity.");
}
