#pragma once
namespace rise { namespace growlancer {
// S21 flareBlue5 16B25E8..16B26EE, before172343C decrement.
template<class Particle> void UpdateFlareParticleTick(Particle& p)
{
    const int half=static_cast<int>(p.fRepeatedlyHeight)/2;
    const float span=p.fRepeatedlyHeight-half;
    if(span>0.f)
        p.Alpha=(p.LifeTime>half ? p.fRepeatedlyHeight-p.LifeTime : p.LifeTime)/span*.85f;
}
} }
