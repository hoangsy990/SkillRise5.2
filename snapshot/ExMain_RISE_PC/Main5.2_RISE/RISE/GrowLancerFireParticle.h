#pragma once

namespace rise { namespace growlancer {
// variant 0/1/2 = S21 8084/0, 806E/4, 8085/0 respectively.
// Native pool index including zero on success, -1 on rejection/exhaustion.
// Caller supplies world-space bone position. No owner-following is implied.
int CreateBrecheFireParticle(int variant, float* position, float* angle,
    float* light, float scale);

// Native named PARTICLE fields; no S21 layout/offset transplant.
// random() must return a nonnegative integer and is called in source order.
template<class Particle, class Random>
void UpdateBrecheFireParticleTick(Particle& p, int variant, Random random)
{
    const int fadeThreshold = variant == 0 ? 15 : 10;
    if (p.LifeTime < fadeThreshold)
        p.Alpha -= .2f;
    else if (p.Alpha < 1.f)
        p.Alpha += (random() % 2 + 2) * .1f;
    else
        p.Alpha = 1.f;
    if (p.Alpha < .1f)
        p.Live = false;
    for (int component = 0; component < 3; ++component)
        p.Light[component] = p.TurningForce[component] * p.Alpha;
    if (p.Scale > 0.f)
        p.Scale -= (random() % 3 + 5 + variant) * .01f;
    else
        p.Live = false;
    p.Position[2] += p.Gravity;
    p.Rotation += 3.f;
}
} }
