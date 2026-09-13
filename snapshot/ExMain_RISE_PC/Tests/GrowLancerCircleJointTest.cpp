#include "../Main5.2_RISE/RISE/GrowLancerCircleJoint.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>
struct Record {
    bool Live = true;
    float Velocity = 0.f, LifeTime = 18.f;
    int moves = 0, turns = 0, tails = 0, matrixVersion = -1;
    float travel = 0.f;
};
int main()
{
    for (float factor : {.25f,.5f,1.f,2.f}) {
        Record r; float remainder=0.f; int frames=0;
        auto move=[](Record& v) {
            assert(v.moves==v.tails);
            assert(v.Velocity==2.f*v.moves);
            v.matrixVersion=v.turns;
            v.travel+=v.Velocity;
            ++v.moves;
        };
        auto turn=[](Record& v) {
            assert(v.LifeTime>10.f && v.moves==v.tails+1);
            assert(v.Velocity==2.f*v.moves);
            ++v.turns;
        };
        auto tail=[](Record& v) {
            assert(v.moves==v.tails+1);
            assert(v.matrixVersion==v.turns-(v.LifeTime>10.f ? 1 : 0));
            ++v.tails;
        };
        while(r.Live && frames++<100)
            rise::growlancer::AdvanceCircleJoint(r,remainder,factor,move,turn,tail);
        assert(!r.Live && r.LifeTime==-1.f);
        assert(r.moves==19 && r.tails==19 && r.turns==8);
        assert(r.Velocity==38.f && r.travel==342.f);
        rise::growlancer::AdvanceCircleJoint(r,remainder,100.f,move,turn,tail);
        assert(r.moves==19);
        // Pool-reuse caller contract: allocation resets remainder.
        r=Record{}; remainder=0.f;
        rise::growlancer::AdvanceCircleJoint(r,remainder,.5f,move,turn,tail);
        assert(r.moves==0);
        rise::growlancer::AdvanceCircleJoint(r,remainder,.5f,move,turn,tail);
        assert(r.moves==1 && r.travel==0.f && r.Velocity==2.f);
    }
    std::puts("PASS Circle joint: four FPS factors, move/turn/tail order, 19 ticks, 8 homing ticks, expiry and reuse");
    std::puts("Scope excludes GPU trails, real pool stress and visual acceptance.");
}
