#include "../Main5.2_RISE/RISE/GrowLancerXsuper.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Particle {
    bool Live=true;
    float LifeTime=6.f, Light[3]={.7f,.7f,1.f};
    int SubType=1;
};
int main() {
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p; float remainder=0.f; int calls=0,frames=0;
        auto update=[&](Particle& v,float tick) {
            assert(tick==1.f);
            rise::growlancer::UpdateXsuperTick(v);
            assert(v.SubType==calls && v.SubType>=0 && v.SubType<6);
            ++calls;
        };
        while(p.Live && frames++<100)
            rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,update);
        assert(calls==6 && p.LifeTime==0.f && p.SubType==5);
        assert(std::fabs(p.Light[2]-std::pow(.95f,6.f))<.00001f);
        rise::growlancer::AdvanceParticleWholeTicks(p,remainder,100.f,update);
        assert(calls==6);
    }
    std::puts("PASS Xsuper4 FPS cases: complete frame0..5, six decays, no life0 update or frame6");
    std::puts("Simulation ticks only; renderer sampling and visual parity require gameplay QA.");
}
