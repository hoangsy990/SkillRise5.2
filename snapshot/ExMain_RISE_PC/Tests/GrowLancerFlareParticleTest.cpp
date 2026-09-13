#include "../Main5.2_RISE/RISE/GrowLancerFlareParticle.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Particle {bool Live=true; float LifeTime=20.f,fRepeatedlyHeight=20.f,Alpha=0.f;};
int main() {
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p;float remainder=0.f;int ticks=0,frames=0;
        auto update=[&](Particle& v,float tick){
            assert(tick==1.f);
            rise::growlancer::UpdateFlareParticleTick(v);
            const int life=20-ticks;
            const float expected=(life>10 ? 20-life : life)*.085f;
            assert(std::fabs(v.Alpha-expected)<.00001f);
            ++ticks;
        };
        while(p.Live && frames++<100)
            rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,update);
        assert(!p.Live && ticks==20 && p.LifeTime==0.f);
        assert(std::fabs(p.Alpha-.085f)<.00001f);
        rise::growlancer::AdvanceParticleWholeTicks(p,remainder,100.f,update);
        assert(ticks==20);
    }
    std::puts("PASS flare5 four FPS cases: exact20 alpha samples, peak.85, final.085,expiry/no life0 body");
    std::puts("Does not prove renderer sampling, native pool stress or visual parity.");
}
