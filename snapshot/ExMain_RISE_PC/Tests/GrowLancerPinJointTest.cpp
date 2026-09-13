#include "../Main5.2_RISE/RISE/GrowLancerPinJoint.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
struct Joint {
    bool Live=true, Collision=false;
    int SubType=2, MaxTails=0, m_byReverseUV=0;
    float Scale=0, Velocity=0, LifeTime=0, Angle[3]={}, Light[3]={};
};
int main() {
    for(int subtype:{2,3,4}) for(float factor:{.25f,.5f,1.f,2.f}) {
        Joint j; j.SubType=subtype;
        rise::growlancer::InitPinJoint(j,12.f);
        assert(j.MaxTails==18 && j.LifeTime==20.f && j.Velocity==60.f);
        assert(j.m_byReverseUV==3 && j.Scale==12.f && j.Angle[0]==-135.f);
        int moves=0,tails=0,frames=0; float remainder=0.f,travel=0.f;
        auto move=[&](Joint& v){assert(moves==tails); ++moves; travel+=v.Velocity;};
        auto tail=[&](Joint& v){
            assert(moves==tails+1); ++tails;
            if(v.LifeTime>16.f) assert(v.Light[2]==0.f && !v.Collision);
            if(v.LifeTime==16.f) assert(std::fabs(v.Light[2]-.88f)<.00001f && v.Collision);
        };
        while(j.Live && frames++<200)
            rise::growlancer::AdvancePinJoint(j,remainder,factor,move,tail);
        assert(moves==21 && tails==21 && travel==1260.f && j.LifeTime==-1.f);
        assert(j.Angle[0]==(subtype==2 ? 5.f : -45.f));
        assert(std::fabs(j.Light[2]-std::pow(.88f,17.f))<.00001f);
        rise::growlancer::AdvancePinJoint(j,remainder,100.f,move,tail);
        assert(moves==21);
    }
    std::puts("PASS Pin joint12 cases: constructor, fixed velocity,21 ticks,color transition,angles,expiry");
    std::puts("Does not prove native geometry, pool or gameplay visual parity.");
}
