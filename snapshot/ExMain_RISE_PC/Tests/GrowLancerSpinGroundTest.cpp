#include "../Main5.2_RISE/RISE/GrowLancerSpinGround.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
using namespace rise::growlancer;
struct Record {
    bool Live = true;
    float LifeTime = 0.f, Timer = 12.f, Alpha = 0.f, Distance = 9.f;
    float Light[3] = {.1f,.15f,.4f}, StartPosition[3] = {};
    int updates = 0, destroyed = 0;
};
static void Near(float a, float b) { assert(std::fabs(a-b) < .0001f); }
int main()
{
    for (float factor : {.25f,.5f,1.f,2.f})
    {
        Record r;
        InitializeSpinGround(r);
        Near(r.Distance,0.f); Near(r.Alpha,1.f); Near(r.Timer,0.f);
        Near(r.StartPosition[2],.4f);
        auto update=[](Record& v,float tick) {
            assert(tick==1.f);
            UpdateSpinGroundTick(v); ++v.updates;
            Near(v.Distance,static_cast<float>(v.updates)*.1f);
            Near(v.Alpha,1.f-static_cast<float>(v.updates)*.1f);
            for(int axis=0;axis<3;++axis)
                Near(v.Light[axis],v.StartPosition[axis]*v.Alpha);
        };
        auto destroy=[](Record* v) { assert(v->updates==11); ++v->destroyed; v->Live=false; };
        int frames=0;
        while(r.Live && frames++<100) AdvanceWholeTicks(r,factor,update,destroy);
        assert(!r.Live && r.destroyed==1 && r.updates==11);
        Near(r.Distance,1.1f); Near(r.Alpha,-.1f);
        AdvanceWholeTicks(r,100.f,update,destroy);
        assert(r.destroyed==1 && r.updates==11);
    }
    std::puts("PASS: Spin ground shared ctor/update, four FPS factors, preserved RGB, zero-life update, cleanup");
    std::puts("Scope excludes terrain renderer/pass, allocation and visual parity.");
}
