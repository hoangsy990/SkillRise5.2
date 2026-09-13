#include "../Main5.2_RISE/RISE/GrowLancerCircleShiny.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
using namespace rise::growlancer;
struct Record {
    bool Live = true;
    float LifeTime = 15.f, Timer = 0.f, Alpha = 0.f;
    int updates = 0, destroyed = 0;
};
static void Near(float a,float b) { assert(std::fabs(a-b)<.0001f); }
int main()
{
    for (float factor : {.25f,.5f,1.f,2.f}) {
        Record controller; controller.LifeTime=30.f;
        int pairs=0,children=0,frames=0;
        const int expectedPair[]={24,23,22,21,20};
        const int expectedChild[]={18,15,12};
        auto update=[&](Record& v,float tick) {
            assert(tick==1.f);
            if(CirclePairTick(v.LifeTime)) {
                assert(pairs<5 && static_cast<int>(v.LifeTime)==expectedPair[pairs]); ++pairs;
            }
            if(CircleChildTick(v.LifeTime)) {
                assert(children<3 && static_cast<int>(v.LifeTime)==expectedChild[children]); ++children;
            }
            ++v.updates;
        };
        auto destroy=[](Record* v){ v->Live=false; ++v->destroyed; };
        while(controller.Live && frames++<200)
            AdvanceWholeTicks(controller,factor,update,destroy);
        assert(pairs==5 && children==3 && controller.updates==31 && controller.destroyed==1);
    }
    for(float factor : {.25f,.5f,1.f,2.f}) {
        Record r;
        auto update=[](Record& v,float tick) {
            assert(tick==1.f); UpdateCircleShinyTick(v); ++v.updates;
            if(v.updates==6) Near(v.Alpha,1.2f);
            if(v.updates==7) Near(v.Alpha,1.1f);
        };
        auto destroy=[](Record* v) { ++v->destroyed; v->Live=false; };
        int frames=0;
        while(r.Live && frames++<100) AdvanceWholeTicks(r,factor,update,destroy);
        assert(r.updates==16 && r.destroyed==1); Near(r.Alpha,.2f);
        AdvanceWholeTicks(r,100.f,update,destroy); assert(r.updates==16);
    }
    Record negative; negative.LifeTime=9.f; negative.Alpha=0.f;
    UpdateCircleShinyTick(negative);
    Near(negative.Alpha,-.1f); Near(negative.LifeTime,0.f);
    std::puts("PASS Circle Shiny: four FPS factors, peak1.2, expiry at16 ticks, negative alpha not clamped");
    std::puts("Scope excludes renderer/constructor/owner/visual parity.");
    std::puts("PASS controller shared gates: exact pair24..20 and child18/15/12 sequences across four FPS factors");
}
