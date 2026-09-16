#include "../Main5.2_RISE/RISE/GrowLancerWrathPersistentTick.h"
struct ParticleFixture {
    bool Live = true;
    int LifeTime = 20;
    float Alpha = .9f, Scale = .001f, Gravity = 2.f, Rotation = 5.f;
    float Light[3] = {}, TurningForce[3] = { .4f,.6f,1.f }, Position[3] = {};
};
struct ZeroRandom { constexpr int operator()() const { return 0; } };
constexpr bool Near(float a, float b) { return a-b < .00001f && b-a < .00001f; }
constexpr bool Check01() {
    ParticleFixture p{}; float offset[3] = {}; float bone[3] = {10,20,30};
    rise::growlancer::UpdateWrathMono01Tick(p,offset,bone,true,ZeroRandom{});
    if (!p.Live || !Near(p.Alpha,1.1f) || p.Scale>=0 || p.LifeTime!=20 ||
        !Near(p.Position[2],32) || !Near(p.Rotation,8)) return false;
    bone[2]=40;
    rise::growlancer::UpdateWrathMono01Tick(p,offset,bone,true,ZeroRandom{});
    return !p.Live && Near(p.Alpha,1) && Near(p.Position[2],44);
}
constexpr bool CheckOwner() {
    ParticleFixture p{}; float offset[3]={}; float bone[3]={};
    rise::growlancer::UpdateWrathMono01Tick(p,offset,bone,false,ZeroRandom{});
    return !p.Live && p.LifeTime==0 && Near(p.Rotation,5) && Near(offset[2],0);
}
constexpr bool Check02() {
    ParticleFixture p{}; float offset[3]={}; float bone[3]={10,20,30};
    rise::growlancer::UpdateWrathMono02Tick(p,offset,bone,ZeroRandom{});
    return p.Live && Near(p.Alpha,.9f) && Near(p.Position[2],50) &&
        Near(p.Scale,-.009f) && p.LifeTime==20 && Near(p.Rotation,5);
}
constexpr bool Check03() {
    ParticleFixture p{}; float bone[3]={10,20,30};
    rise::growlancer::UpdateWrathMono03Tick(p,bone);
    return p.Live && Near(p.Position[2],30) && Near(p.Scale,-.049f) &&
        Near(p.Alpha,.9f-.0666666030883789f) && Near(p.Light[2],p.Alpha) &&
        p.LifeTime==20 && Near(p.Rotation,5);
}
static_assert(Check01(), "overshoot, pre-subtraction death and moving bone drift");
static_assert(CheckOwner(), "dead owner kills without motion");
static_assert(Check02(), "no invented alpha write or immediate scale death");
static_assert(Check03(), "bone anchor and exact fade without added clamp");
constexpr bool CheckScatter() {
    ParticleFixture p{}; p.Alpha=1.f; p.LifeTime=5;
    for(int i=0;i<5;++i) rise::growlancer::UpdateWrathScatterTick(p);
    if(!p.Live || !Near(p.Alpha,.25f) || p.LifeTime!=5 ||
        !Near(p.Scale,.001f) || !Near(p.Rotation,5.f)) return false;
    p.Alpha=.05f;
    rise::growlancer::UpdateWrathScatterTick(p);
    return !p.Live && Near(p.Alpha,-.1f) && Near(p.Light[2],-.1f);
}
static_assert(CheckScatter(), "fade only, no extra motion/clamp/decrement, light written after death");
