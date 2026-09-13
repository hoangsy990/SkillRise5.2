#include "../Main5.2_RISE/RISE/GrowLancerClashTick.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>

struct Item {
    bool Live = true;
    float LifeTime = 27, Timer = 0, Alpha = 0, Scale = .75f;
    float Angle[3] = {0, 17, 0};
};

int main()
{
    for (bool front : {true, false}) for (float factor : {.25f, .5f, 1.f, 2.f}) {
        Item e;
        const int lifetime = front ? 27 : 10;
        e.LifeTime = static_cast<float>(lifetime);
        int visits = 0, kills = 0, frames = 0;
        float peak = 0;
        auto tick = [&](Item& item, float step) {
            assert(step == 1 && item.LifeTime == lifetime - visits);
            if (front) rise::growlancer::UpdateClashFrontScalarTick(item);
            else rise::growlancer::UpdateClashRearScalarTick(item);
            const int distance = visits <= lifetime / 2 ? visits : lifetime - visits;
            const float expected = front ? distance * .05f : distance * .18f;
            assert(std::fabs(item.Alpha - expected) < .00001f);
            if (item.Alpha > peak) peak = item.Alpha;
            ++visits;
        };
        auto kill = [&](Item* item) { item->Live = false; ++kills; };
        while (e.Live && frames++ < 200)
            rise::growlancer::AdvanceWholeTicks(e, factor, tick, kill);
        assert(!e.Live && kills == 1 && visits == lifetime + 1);
        assert(e.Alpha == 0);
        assert(std::fabs(peak - (front ? .65f : .9f)) < .00001f);
        assert(std::fabs(e.Scale - (front ? .89f : 1.30f)) < .00001f);
        assert(e.Angle[1] == (front ? 297.f : 17.f));
    }
    // All three components matter; rear creation pose is NOT its first update.
    const float owner[3] = {10,20,30};
    const float directions[3][3] = {{0,0,1}, {.6f,0,.8f}, {0,0,0}};
    for (const auto& direction : directions) for (bool front : {true,false}) {
        float position[3], stored[3];
        rise::growlancer::ClashInitialPosition(owner, direction, front, position);
        for (int axis=0;axis<3;++axis) {
            assert(std::fabs(position[axis]-owner[axis]-direction[axis]*(front?30.f:1.f))<.00001f);
            stored[axis]=direction[axis]*(front?30.f:-50.f);
        }
        const float movedOwner[3] = {13,24,35};
        rise::growlancer::ClashStoredPosition(movedOwner,stored,front?150.f:0.f,position);
        for (int axis=0;axis<3;++axis)
            assert(std::fabs(position[axis]-movedOwner[axis]-stored[axis]-(front&&axis==2?150.f:0.f))<.00001f);
    }
    assert(!rise::growlancer::ClashEmissionDue(0,0));
    assert(!rise::growlancer::ClashEmissionDue(0,0.9999f));
    assert(!rise::growlancer::ClashEmissionDue(0,1.0f));
    assert(rise::growlancer::ClashEmissionDue(0,1.0001f));
    assert(rise::growlancer::ClashEmissionDue(0,6.5f));
    assert(!rise::growlancer::ClashEmissionDue(0,6.5001f));
    assert(!rise::growlancer::ClashEmissionDue(1,1));
    for (float factor : {.25f,.5f,1.f,2.f}) {
        Item controller; controller.LifeTime=30;
        int visits=0,emissions=0,kills=0,frames=0;
        auto tick=[&](Item& e,float step) {
            assert(step==1 && e.LifeTime==30-visits);
            ++visits;
            // Two missing-target ticks do not spend the one-shot counter.
            if(visits<=2) return;
            if(rise::growlancer::ClashEmissionDue(emissions,1.5f)) ++emissions;
        };
        auto kill=[&](Item* e){e->Live=false;++kills;};
        while(controller.Live && frames++<200)
            rise::growlancer::AdvanceWholeTicks(controller,factor,tick,kill);
        assert(visits==31 && kills==1 && emissions==1);
    }
    std::puts("PASS12 Clash tick cases +6 pose cases and frame boundaries; excludes normalization API, native owner/pool/GPU");
}
