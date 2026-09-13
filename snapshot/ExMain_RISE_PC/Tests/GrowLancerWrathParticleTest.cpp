#include "../Main5.2_RISE/RISE/GrowLancerWrathParticle.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
struct Particle {bool Live=true;float LifeTime=10,Scale=2,Alpha=1,Light[3]={1,1,.62f};};
struct Controller {bool Live=true;float LifeTime=70,Timer=0;};
struct Aura : Controller {int SubType=0;float Alpha=0,Scale=.95f,BlendMeshTexCoordU=0,BlendMeshTexCoordV=-.7f;};
int main() {
    for(int subtype:{0,1})for(float factor:{.25f,.5f,1.f,2.f}) {
        Aura a;a.SubType=subtype;a.LifeTime=30;
        if(subtype){a.Scale=.7f;a.BlendMeshTexCoordU=.25f;}
        int visits=0,kills=0,continuations=0,frames=0;
        auto tick=[&](Aura& v,float f) {
            assert(f==1 && v.LifeTime==30-visits);
            rise::growlancer::UpdateWrathAuraScalarTick(v);
            if(v.LifeTime==1)++continuations; // Eligibility, not real buff query.
            ++visits;
            const int early=visits<11 ? visits : 11,late=visits-early;
            assert(std::fabs(v.Alpha-(early*(subtype ? .06f : .073f)-late*(subtype ? .07f : .045f)))<.00001f);
        };
        auto kill=[&](Aura* v){v->Live=false;++kills;};
        while(a.Live && frames++<150)rise::growlancer::AdvanceWholeTicks(a,factor,tick,kill);
        assert(visits==31 && kills==1 && continuations==1);
        const float scale=subtype ? .7051f : .95f-.15f*static_cast<float>(std::sin(30.0));
        assert(std::fabs(a.Scale-scale)<.00001f);
        assert(std::fabs(a.BlendMeshTexCoordU-(subtype ? .52f : .496f))<.00001f);
        assert(std::fabs(a.BlendMeshTexCoordV-(subtype ? -.17f : .043f))<.00001f);
    }
    const float starts[]={3.f,3.5f,5.f,5.6f},ends[]={3.5f,4.2f,5.5f,6.5f};
    for(int stage=0;stage<4;++stage) {
        assert(rise::growlancer::WrathStageDue(stage,starts[stage]));
        assert(rise::growlancer::WrathStageDue(stage,ends[stage]));
        assert(!rise::growlancer::WrathStageDue(stage,starts[stage]-.001f));
        assert(!rise::growlancer::WrathStageDue(stage,ends[stage]+.001f));
    }
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Controller c;int visits=0,frames=0,kills=0,stage=0;
        auto tick=[&](Controller& v,float) {
            assert(v.LifeTime==70-visits);
            const float frame=visits<5 ? 3.5f : visits<10 ? 5.f : 6.f;
            for(int expected=0;expected<4;++expected)
                if(stage==expected && rise::growlancer::WrathStageDue(expected,frame))++stage;
            if(visits==0)assert(stage==2); // Both stages in the same tick.
            ++visits;
        };
        auto kill=[&](Controller* v){v->Live=false;++kills;};
        while(c.Live && frames++<350)rise::growlancer::AdvanceWholeTicks(c,factor,tick,kill);
        assert(visits==71 && stage==4 && kills==1);
    }
    for(int kind:{0,1,2})for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p;int visits=0,frames=0;float remainder=0;
        if(kind){p.LifeTime=20;p.Alpha=.5f;}
        auto tick=[&](Particle& v,float) {
            assert(v.LifeTime==(kind ? 20 : 10)-visits);
            if(kind)rise::growlancer::UpdateWrathAlphaTick(v);
            else rise::growlancer::UpdateWrathComboTick(v);
            ++visits;
            if(kind) {
                const float expected=visits<=17 ? .5f-.03f*visits : 0.f;
                assert(std::fabs(v.Alpha-expected)<.00001f);
                if(visits==17)assert(v.Alpha<0.f);
            } else {
                assert(std::fabs(v.Scale-(2.f+static_cast<float>(std::sin(30.0))*.18f*visits))<.00001f);
                assert(std::fabs(v.Light[0]-std::pow(.85f,visits))<.00001f);
            }
        };
        while(p.Live && frames++<100)rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,tick);
        assert(!p.Live && visits==(kind ? 20 : 10));
    }
    Particle edge;edge.Scale=.01f;
    rise::growlancer::UpdateWrathComboTick(edge);assert(edge.Scale<0 && edge.Alpha==1);
    const float light=edge.Light[0];
    rise::growlancer::UpdateWrathComboTick(edge);assert(edge.Scale==0 && edge.Alpha==0 && edge.Light[0]==light);
    std::puts("PASS24 Wrath aura/controller/particle cases plus boundaries; excludes real buff queries,animation,pools,GPU");
}
