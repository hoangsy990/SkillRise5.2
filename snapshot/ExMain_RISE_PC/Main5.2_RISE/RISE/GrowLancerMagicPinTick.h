#pragma once
#include <cmath>
namespace rise { namespace growlancer {
template<class Particle> void UpdateMagicSmokeTick(Particle& particle) {
    for(int axis=0;axis<3;++axis)particle.Light[axis]*=.86f;
    particle.Scale+=.03f;
    particle.Rotation-=.7f;
    particle.Position[2]+=3.f;
}
template<class Particle> void UpdateMagicShockwaveTick(Particle& particle) {
    // S21 only checks non-null owner; it never dereferences that owner here.
    if(!particle.Target) {
        particle.Live=false;
        particle.LifeTime=0.f;
        return;
    }
    particle.fRepeatedlyHeight+=.133333f;
    particle.Scale=particle.Gravity*particle.fRepeatedlyHeight;
    if(particle.LifeTime<6.f)particle.Alpha-=.08f;
    for(int axis=0;axis<3;++axis)
        particle.Light[axis]=particle.TurningForce[axis]*particle.Alpha;
}
inline bool MagicPinHitDue(float life) { return life < 5.f; }
// S21 controller samples inclusive windows; it does not catch up skipped ones.
inline bool MagicPinStageDue(int stage, float frame) {
    if(stage==0)return frame>=5.2f && frame<=5.8f;
    if(stage==1)return frame>=7.f && frame<=7.5f;
    if(stage==2)return frame>=10.f && frame<=10.5f;
    return false;
}
template<class Particle> void InitMagicThunder(Particle& particle) {
    particle.LifeTime=3;
    particle.Scale=1.5f;
    particle.Alpha=.5f;
    particle.Light[0]=particle.Light[1]=particle.Light[2]=1.f;
}
// S21 16C01D3..16C03D0: comparison uses remaining life, not max life.
// Therefore both life2 and life1 take the color/fade branch; life3 does not.
template<class Particle> void UpdateMagicThunderTick(Particle& particle) {
    particle.Rotation=0.f;
    if(particle.SubType==10)particle.Scale+=.45f;
    const int life=static_cast<int>(particle.LifeTime);
    if(life==life/2+1) {
        particle.Light[0]*=.55f;
        particle.Light[1]*=.55f;
        if(particle.SubType==9)particle.Alpha-=.32f;
    }
}
// Audited life10..0 child envelopes; no global SS6 alpha helper change.
inline float MagicPinTriangle(float life, float peak) {
    return (life > 5.f ? (10.f-life)/5.f : life/5.f)*peak;
}
template<class Object> void UpdateMagicPin01Tick(Object& effect) {
    effect.Alpha += effect.LifeTime > 5.0f ? 0.2f : -0.2f;
}
template<class Object> void UpdateMagicPin03Tick(Object& effect) {
    effect.Alpha=MagicPinTriangle(effect.LifeTime,.8f);
}
template<class Object> void UpdateMagicPinRootTick(Object& effect) {
    effect.Angle[2]+=20.f;
    effect.Alpha-=.038f;
}
template<class Object> void UpdateMagicPinAuxTick(Object& effect) {
    effect.Alpha=MagicPinTriangle(effect.LifeTime,.75f);
    // S21 float->double sin->float, then negate and multiply .005.
    effect.Scale += -static_cast<float>(std::sin(30.0)) * 0.005f;
}
} }
