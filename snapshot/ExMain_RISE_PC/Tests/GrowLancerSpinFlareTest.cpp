#include "../Main5.2_RISE/RISE/GrowLancerSpinFlare.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
using namespace rise::growlancer;
struct Record {
    bool Live = true;
    float Timer = 99.f, LifeTime = 99.f, Alpha = 1.f, Scale = 5.f;
    float Angle[3] = {11.f, 22.f, 270.f};
    int updates = 0, destructions = 0;
};
static void Near(float a, float b) { assert(std::fabs(a-b) < .0001f); }
int main()
{
    for (int rotation : {0, 90, 179})
    for (float factor : {.25f, .5f, 1.f, 2.f})
    {
        Record r;
        InitializeSpinFlare(r, rotation);
        Near(r.Timer, 0.f); Near(r.Alpha, 0.f); Near(r.LifeTime, 10.f);
        Near(r.Scale, 5.f); Near(r.Angle[2], static_cast<float>(rotation));
        auto update = [](Record& v, float tick) {
            assert(tick == 1.f);
            UpdateSpinFlareTick(v);
            ++v.updates;
            if (v.updates == 1) Near(v.Alpha, .2f);
            if (v.updates == 5) Near(v.Alpha, 1.f);
            if (v.updates == 6) Near(v.Alpha, .8f);
        };
        auto destroy = [](Record* v) {
            assert(v->updates == 11 && v->LifeTime == 0.f);
            ++v->destructions; v->Live = false;
        };
        int frames = 0;
        while (r.Live && frames++ < 100)
            AdvanceWholeTicks(r, factor, update, destroy);
        assert(!r.Live && r.updates == 11 && r.destructions == 1);
        Near(r.Alpha, -.2f); Near(r.Scale, 5.f);
        Near(r.Angle[0], 11.f); Near(r.Angle[1], 22.f);
        AdvanceWholeTicks(r, 100.f, update, destroy);
        assert(r.updates == 11 && r.destructions == 1);
    }
    std::puts("PASS: Spin flare ctor/update shared runtime math, 12 rotation/FPS cases, threshold, zero-life update, cleanup");
    std::puts("Scope excludes random distribution, renderer, particle subtype4, pool allocation and visual parity.");
}
