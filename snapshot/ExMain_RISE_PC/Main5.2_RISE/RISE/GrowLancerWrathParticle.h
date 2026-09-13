#pragma once
#include <cmath>
namespace rise { namespace growlancer {
template<class Object> void UpdateWrathAuraScalarTick(Object& e) {
    const bool first=e.LifeTime>19.f;
    if(first && e.Alpha>=1.f)e.Alpha=1.f;
    else e.Alpha+=e.SubType==0 ? (first ? .073f : -.045f) : (first ? .06f : -.07f);
    if(e.SubType==0) {
        e.Scale+=(first ? -.012f : -.0009f)*static_cast<float>(std::sin(30.0));
        e.BlendMeshTexCoordU+=.016f;
        e.BlendMeshTexCoordV+=first ? .053f : .008f;
    } else {
        e.Scale+=first ? .0001f : .0002f;
        e.BlendMeshTexCoordU+=first ? .01f : .008f;
        e.BlendMeshTexCoordV+=first ? .03f : .01f;
    }
}
inline bool WrathStageDue(int stage,float frame) {
    if(stage==0)return frame>=3.f && frame<=3.5f;
    if(stage==1)return frame>=3.5f && frame<=4.2f;
    if(stage==2)return frame>=5.f && frame<=5.5f;
    if(stage==3)return frame>=5.6f && frame<=6.5f;
    return false;
}
template<class Particle> void UpdateWrathComboTick(Particle& p) {
    if(p.Scale<=0.f) {p.Scale=0.f;p.Alpha=0.f;return;}
    p.Scale+=static_cast<float>(std::sin(30.0))*.18f;
    for(int axis=0;axis<3;++axis)p.Light[axis]*=.85f;
}
template<class Particle> void UpdateWrathAlphaTick(Particle& p) {
    // Clamp on entry, not after subtraction (S21 1709779..17097C4).
    if(p.Alpha<=0.f)p.Alpha=0.f;
    else p.Alpha-=.03f;
}
} }
