#include "../Main5.2_RISE/RISE/GrowLancerObsidianTick.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Item {bool Live=true;float LifeTime=15,Timer=0,Alpha=0,Angle[3]={0,0,20},Scale=2,Rotation=90,Light[3]={.5f,.95f,1};};
int main() {
    for(int kind:{0,1,2})for(float factor:{.25f,.5f,1.f,2.f}) {
        Item e;int visits=0,kills=0,frames=0;float remainder=0;
        if(kind==1)e.LifeTime=20;
        auto tick=[&](Item& v,float f) {
            assert(f==1 && v.LifeTime==(kind==1 ? 20 : 15)-visits);
            if(kind==0) {
                rise::growlancer::UpdateObsidianRotatingTick(v);
                const int n=visits<8 ? visits : 15-visits;
                assert(std::fabs(v.Alpha-n*.1125f)<.00001f);
            } else if(kind==1)rise::growlancer::UpdateObsidianAuraTick(v);
            else rise::growlancer::UpdateObsidianThunderTick(v);
            ++visits;
        };
        auto kill=[&](Item* p){p->Live=false;++kills;};
        while(e.Live && frames++<120) {
            if(kind==2)rise::growlancer::AdvanceParticleWholeTicks(e,remainder,factor,tick);
            else rise::growlancer::AdvanceWholeTicks(e,factor,tick,kill);
        }
        assert(!e.Live && visits==(kind==0 ? 16 : kind==1 ? 21 : 15));
        if(kind==0)assert(e.Alpha==0 && e.Angle[2]==980 && kills==1);
        if(kind==1)assert(std::fabs(e.Alpha+.1f)<.00001f && kills==1);
        if(kind==2)assert(e.Rotation==0 && std::fabs(e.Scale-2.45f)<.00001f && std::fabs(e.Light[2]-std::pow(.85f,15))<.00001f);
    }
    std::puts("PASS12 Obsidian scalar/tick cases; excludes actual owners,action317,pools,GPU");
}
