#pragma once
namespace rise { namespace growlancer {
// S21 17017F5..17018FE, before manager decrement172343C.
template<class Particle> void UpdateXsuperTick(Particle& p)
{
    p.SubType=6-static_cast<int>(p.LifeTime);
    for(int axis=0;axis<3;++axis) p.Light[axis]*=.95f;
}
} }
