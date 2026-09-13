#pragma once
namespace rise { namespace growlancer {
template<class Object> void UpdateObsidianRotatingTick(Object& e) {
    e.Angle[2]+=60.f;
    e.Alpha=(e.LifeTime>7.f ? 15.f-e.LifeTime : e.LifeTime)/8.f*.9f;
}
template<class Object> void UpdateObsidianAuraTick(Object& e) {
    e.Alpha+=e.LifeTime>10.f ? .1f : -.1f;
}
template<class Particle> void UpdateObsidianThunderTick(Particle& p) {
    p.Rotation=0.f;p.Scale+=.03f;
    for(int axis=0;axis<3;++axis)p.Light[axis]*=.85f;
}
} }
