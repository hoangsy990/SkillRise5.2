#include "../Main5.2_RISE/RISE/GrowLancerShiningMesh.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Object {
    bool Live=true;
    int SubType=0;
    float LifeTime=12.f,Timer=0.f,Scale=.9f,Alpha=.2f;
    float Angle[3]={};
};
int main(){
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Object o;o.LifeTime=6.f;o.Scale=.3f;o.Alpha=.85f;
        int visits=0,kills=0,frames=0;
        auto update=[&](Object& v,float tick){
            assert(tick==1.f && v.LifeTime==6.f-visits);
            assert(v.Scale==.3f && v.Alpha==.85f);++visits;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<100)
            rise::growlancer::AdvanceWholeTicks(o,factor,update,kill);
        assert(visits==7 && kills==1);
    }
    for(int kind:{0,1}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Object o;const int life=kind==0 ? 8 : 20;
        o.LifeTime=static_cast<float>(life);o.Scale=kind==0 ? 1.8f : .8f;
        int ticks=0,kills=0,frames=0;
        auto update=[&](Object& v,float tick){
            assert(tick==1.f);
            if(kind==0) rise::growlancer::UpdateShiningPinTick(v);
            else rise::growlancer::UpdateShiningSpinTick(v);
            const int elapsed=ticks;
            const int triangle=elapsed<life/2 ? elapsed : life-elapsed;
            const float expected=kind==0 ? .2f+triangle*.075f : .1f+triangle*.07f;
            assert(std::fabs(v.Alpha-expected)<.00001f);++ticks;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<150)
            rise::growlancer::AdvanceWholeTicks(o,factor,update,kill);
        assert(ticks==life+1 && kills==1);
        if(kind==0) assert(std::fabs(o.Scale-3.15f)<.00001f);
        else assert(o.Angle[1]==420.f);
    }
    for(int subtype:{0,1}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Object o;o.SubType=subtype;int ticks=0,kills=0,frames=0;
        auto update=[&](Object& v,float tick){
            assert(tick==1.f && v.LifeTime==12.f-ticks);
            rise::growlancer::UpdateShiningBodyTick(v);++ticks;
            const int up=ticks<5 ? ticks : 5;
            const float expected=.2f+(2*up-ticks)/7.f;
            assert(std::fabs(v.Alpha-expected)<.00001f);
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<100)
            rise::growlancer::AdvanceWholeTicks(o,factor,update,kill);
        assert(ticks==13 && kills==1 && o.Alpha<0.f);
        assert(std::fabs(o.Scale-(subtype==0 ? 1.03f : .9f))<.00001f);
        rise::growlancer::AdvanceWholeTicks(o,100.f,update,kill);
        assert(ticks==13 && kills==1);
    }
    std::puts("PASS Shining body8cases: exact13 samples, unclamped negativealpha, subtype scale, single cleanup");
    std::puts("Scope excludes renderer, real effect pool and gameplay parity.");
    std::puts("PASS pin/spin8cases: half4/10, peak.5/.8,9/21 ticks,scale/pitch endpoints");
    std::puts("PASS shockwave4cases: seven lifecycle visits including life0, single cleanup (lifecycle only)");
}
