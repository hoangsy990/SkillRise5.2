#include "../Main5.2_RISE/RISE/GrowLancerHarshWind.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Object {
    bool Live=true;
    float LifeTime=8.f,Timer=0.f,Scale=1.f,Alpha=1.f;
    float Light[3]={1.f,.6f,.2f};
};
int main() {
    // Consumer-before-producer model: appends do not reset lifetime/clock.
    for(float factor : {.25f,.5f,1.f,2.f}) {
        Object blur;blur.LifeTime=30.f;
        rise::growlancer::HarshBlurClock clock;clock.Reset(true);
        int ticks=0,frames=0,appends=0;
        float producer=0.f;
        auto tick=[&](Object& b) { ++ticks;if(--b.LifeTime<=0.f)b.Live=false; };
        while(blur.Live && frames++<200) {
            rise::growlancer::AdvanceHarshBlur(blur,clock,factor,tick);
            producer+=factor;
            while(producer>=1.f) { producer-=1.f;if(blur.Live)++appends; }
        }
        assert(ticks==30 && frames*factor==30.f && appends>0);
        rise::growlancer::AdvanceHarshBlur(blur,clock,2.f,tick);
        assert(ticks==30);
        // Reuse resets fractional debt and mode, including a legacy allocation.
        clock.remainder=.75f;clock.Reset(false);blur.Live=true;blur.LifeTime=30.f;
        rise::growlancer::AdvanceHarshBlur(blur,clock,.25f,tick);
        assert(blur.LifeTime==29.f && clock.remainder==0.f);
        clock.Reset(true);
        rise::growlancer::AdvanceHarshBlur(blur,clock,.25f,tick);
        assert(blur.LifeTime==29.f && clock.remainder==.25f);
    }
    // Controller lifecycle only: mock the existing invalid-owner life=0 path.
    for (bool invalidOwner : {false,true}) for (float factor : {.25f,.5f,1.f,2.f}) {
        Object o;o.LifeTime=40.f;
        int visits=0,kills=0,frames=0;
        auto update=[&](Object& v,float tick) {
            assert(tick==1.f);++visits;
            if(invalidOwner) v.LifeTime=0.f;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<200)
            rise::growlancer::AdvanceWholeTicks(o,factor,update,kill);
        assert(visits==(invalidOwner ? 1 : 41) && kills==1);
    }
    for (int kind : {0,1}) for (float factor : {.25f,.5f,1.f,2.f}) {
        Object o;
        const int initialLife = kind == 0 ? 8 : 7;
        o.LifeTime=static_cast<float>(initialLife);
        int visits=0,kills=0,frames=0;
        auto update=[&](Object& v,float tick) {
            assert(tick==1.f && v.LifeTime==initialLife-visits);
            if(kind==0) rise::growlancer::UpdateHarshWind01Tick(v);
            else rise::growlancer::UpdateHarshWind02Tick(v);
            ++visits;
            assert(std::fabs(v.Scale-(1.f+visits*(kind==0 ? .1f : .45f)))<.00001f);
            if(kind==0) assert(v.Alpha==1.f-visits*.125f);
            else {
                const float initial[3]={1.f,.6f,.2f};
                for(int c=0;c<3;++c)
                    assert(std::fabs(v.Light[c]-initial[c]*std::pow(.9f,visits))<.00001f);
            }
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<100)
            rise::growlancer::AdvanceWholeTicks(o,factor,update,kill);
        assert(visits==initialLife+1 && kills==1);
        if(kind==0) assert(o.Alpha==-.125f);
        rise::growlancer::AdvanceWholeTicks(o,100.f,update,kill);
        assert(visits==initialLife+1 && kills==1);
    }
    std::puts("PASS Harsh8cases:9/8 visits,unclamped alpha,RGB fade,scale,single cleanup");
    std::puts("Scope excludes transformed movement,real pool and GPU parity.");
    std::puts("PASS controller8cases:lifecycle-only41 visits or invalid-owner first-tick cleanup");
    std::puts("PASS blur4FPS:30ticks,consumer-before-producer,append/no refresh,dead/reused/legacy clock");
}
