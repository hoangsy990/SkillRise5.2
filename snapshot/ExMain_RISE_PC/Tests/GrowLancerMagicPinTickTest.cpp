#include "../Main5.2_RISE/RISE/GrowLancerMagicPinTick.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
struct Object {
    bool Live=true;
    float Timer=0.f,LifeTime=10.f,Alpha=0.f,Scale=1.2f,Angle[3]={};
};
struct Particle {
    bool Live=true;
    int SubType=9;
    float LifeTime=0,Scale=0,Alpha=0,Rotation=123,Light[3]={};
    Object* Target=nullptr;
    float fRepeatedlyHeight=1.3f,Gravity=.69f,TurningForce[3]={.3f,.3f,.5f};
    float Position[3]={10.f,20.f,30.f};
};
int main() {
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p;p.LifeTime=20;p.Scale=2.5f;p.Alpha=1.f;
        p.Light[0]=.6f;p.Light[1]=.7f;p.Light[2]=1.f;
        float remainder=0;int visits=0,frames=0;
        auto tick=[&](Particle& v,float) {
            assert(v.LifeTime==20-visits);
            rise::growlancer::UpdateMagicSmokeTick(v);++visits;
            assert(std::fabs(v.Scale-(2.5f+.03f*visits))<.00001f);
            assert(std::fabs(v.Rotation-(123.f-.7f*visits))<.0001f);
            assert(v.Position[0]==10.f && v.Position[1]==20.f && v.Position[2]==30.f+3.f*visits);
            assert(std::fabs(v.Light[2]-std::pow(.86f,visits))<.00001f && v.Alpha==1.f);
        };
        while(p.Live && frames++<100)
            rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,tick);
        assert(!p.Live && visits==20 && p.LifeTime==0);
    }
    for(bool hasOwner:{false,true}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p;Object owner;owner.Live=false;
        p.Target=hasOwner ? &owner : nullptr;p.LifeTime=8;p.Alpha=.6f;
        float remainder=0;int visits=0,frames=0;
        auto tick=[&](Particle& v,float) {
            assert(v.LifeTime==8-visits);
            rise::growlancer::UpdateMagicShockwaveTick(v);
            ++visits;
            if(hasOwner) {
                const float expectedAlpha=.6f-.08f*(visits>3 ? visits-3 : 0);
                assert(std::fabs(v.Alpha-expectedAlpha)<.00001f);
                assert(std::fabs(v.Scale-.69f*(1.3f+.133333f*visits))<.00001f);
                for(int axis=0;axis<3;++axis)
                    assert(std::fabs(v.Light[axis]-v.TurningForce[axis]*expectedAlpha)<.00001f);
            }
        };
        while(p.Live && frames++<50)
            rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,tick);
        assert(!p.Live && visits==(hasOwner ? 8 : 1));
        assert(p.LifeTime==(hasOwner ? 0.f : -1.f));
    }
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Object hit;hit.LifeTime=8;
        int visits=0,bursts=0,kills=0,frames=0;
        auto tick=[&](Object& v,float) {
            assert(v.LifeTime==8-visits);
            if(rise::growlancer::MagicPinHitDue(v.LifeTime))++bursts;
            ++visits;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(hit.Live && frames++<50)
            rise::growlancer::AdvanceWholeTicks(hit,factor,tick,kill);
        assert(visits==9 && bursts==5 && kills==1);
    }
    const float begin[]={5.2f,7.f,10.f}, end[]={5.8f,7.5f,10.5f};
    for(int stage=0;stage<3;++stage) {
        assert(rise::growlancer::MagicPinStageDue(stage,begin[stage]));
        assert(rise::growlancer::MagicPinStageDue(stage,end[stage]));
        assert(!rise::growlancer::MagicPinStageDue(stage,begin[stage]-.001f));
        assert(!rise::growlancer::MagicPinStageDue(stage,end[stage]+.001f));
    }
    assert(!rise::growlancer::MagicPinStageDue(3,10.f));
    for(float factor:{.25f,.5f,1.f,2.f}) {
        Object controller;controller.LifeTime=75;
        int visits=0,kills=0,frames=0,stage=0;
        auto tick=[&](Object& v,float) {
            assert(v.LifeTime==75-visits);
            // Each window is observed repeatedly; only one emission per stage.
            const float frame=visits<4 ? 5.2f : visits<8 ? 7.f : 10.f;
            if(rise::growlancer::MagicPinStageDue(stage,frame))++stage;
            ++visits;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(controller.Live && frames++<400)
            rise::growlancer::AdvanceWholeTicks(controller,factor,tick,kill);
        assert(visits==76 && kills==1 && stage==3);
    }
    for(int subtype:{9,10}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Particle p;p.SubType=subtype;
        rise::growlancer::InitMagicThunder(p);
        int visits=0,frames=0;float remainder=0;
        auto tick=[&](Particle& v,float f) {
            assert(f==1.f && v.LifeTime==3-visits);
            rise::growlancer::UpdateMagicThunderTick(v);
            const float rg[]={1.f,.55f,.3025f};
            const float alpha[]={.5f,.18f,-.14f};
            assert(std::fabs(v.Light[0]-rg[visits])<.00001f);
            assert(v.Light[0]==v.Light[1] && v.Light[2]==1.f && v.Rotation==0.f);
            assert(std::fabs(v.Alpha-(subtype==9 ? alpha[visits] : .5f))<.00001f);
            assert(std::fabs(v.Scale-(subtype==9 ? 1.5f : 1.5f+.45f*(visits+1)))<.00001f);
            ++visits;
        };
        while(p.Live && frames++<40)
            rise::growlancer::AdvanceParticleWholeTicks(p,remainder,factor,tick);
        assert(!p.Live && visits==3 && p.LifeTime==0);
        rise::growlancer::AdvanceParticleWholeTicks(p,remainder,2.f,tick);
        assert(visits==3);
    }
    for(int kind:{0,1,2,3}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Object o;int visits=0,kills=0,frames=0;
        const int life=kind==2 ? 14 : 10;
        o.LifeTime=static_cast<float>(life);
        if(kind>=2)o.Alpha=1.f;
        auto tick=[&](Object& v,float f) {
            assert(f==1.f && v.LifeTime==life-visits);
            if(kind==0)rise::growlancer::UpdateMagicPin01Tick(v);
            if(kind==1)rise::growlancer::UpdateMagicPin03Tick(v);
            if(kind==2)rise::growlancer::UpdateMagicPinRootTick(v);
            if(kind==3)rise::growlancer::UpdateMagicPinAuxTick(v);
            if(kind==1 || kind==3) {
                const int triangle=visits<=5 ? visits : 10-visits;
                assert(std::fabs(v.Alpha-triangle*(kind==1 ? .16f : .15f))<.00001f);
            }
            ++visits;
        };
        auto kill=[&](Object* v){v->Live=false;++kills;};
        while(o.Live && frames++<150)
            rise::growlancer::AdvanceWholeTicks(o,factor,tick,kill);
        assert(visits==life+1 && kills==1);
        if(kind==0)assert(std::fabs(o.Alpha+.2f)<.00001f);
        if(kind==2)assert(std::fabs(o.Alpha-.43f)<.00001f && o.Angle[2]==300.f);
        if(kind==3)assert(std::fabs(o.Scale-1.2543417f)<.00001f);
        rise::growlancer::AdvanceWholeTicks(o,2.f,tick,kill);
        assert(visits==life+1 && kills==1);
    }
    std::puts("PASS44 MagicPin cases plus stage boundaries; excludes live animation,real pool,GPU rendering");
}
