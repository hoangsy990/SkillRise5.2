#include "../Main5.2_RISE/RISE/GrowLancerBrecheGround.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>

using namespace rise::growlancer;
struct Record
{
    bool Live = true;
    float Timer = 0.f, LifeTime = 20.f, Alpha = 0.f, Scale = 2.f;
    float Angle[3] = {11.f, 22.f, 30.f};
    int updates = 0, destructions = 0;
};
static void Near(float actual, float expected)
{
    assert(std::fabs(actual - expected) < .0001f);
}
int main()
{
    for (BrecheGroundKind kind : {kBrecheMarks, kBrecheTwilight02, kBrecheTwilight01})
    for (float scale : {.2f, .9f, 2.f, 2.5f})
    for (float factor : {.25f, .5f, 1.f, 2.f})
    {
        Record r;
        r.Alpha = kind == kBrecheMarks ? 0.f : .1f;
        r.Scale = scale;
        auto update = [kind](Record& v, float tick) {
            assert(tick == 1.f);
            UpdateBrecheGroundTick(v, kind);
            ++v.updates;
            if (v.LifeTime == 11.f)
                Near(v.Alpha, kind == kBrecheMarks ? 1.f : 1.1f);
            if (v.LifeTime == 10.f)
                Near(v.Alpha, kind == kBrecheMarks ? .9f : 1.f);
        };
        auto destroy = [](Record* v) {
            assert(v->LifeTime == 0.f && v->updates == 21);
            ++v->destructions;
            v->Live = false;
        };
        int frames = 0;
        while (r.Live && frames++ < 100)
            AdvanceWholeTicks(r, factor, update, destroy);
        assert(!r.Live && r.updates == 21 && r.destructions == 1);
        Near(r.Alpha, kind == kBrecheMarks ? -.1f : 0.f);
        Near(r.Scale, scale + (kind == kBrecheTwilight01 ? 2.1f : 0.f));
        Near(r.Angle[0], 11.f); Near(r.Angle[1], 22.f);
        Near(r.Angle[2], kind == kBrecheMarks ? 30.f : 345.f);
        AdvanceWholeTicks(r, 100.f, update, destroy);
        assert(r.updates == 21 && r.destructions == 1);
    }
    std::puts("PASS: Breche ground shared runtime math; 48 kind/scale/FPS cases, thresholds, overshoot, zero-life tick, single destruction");
    std::puts("Scope excludes renderer, pool allocation, emitter, packets and visual parity.");
}
