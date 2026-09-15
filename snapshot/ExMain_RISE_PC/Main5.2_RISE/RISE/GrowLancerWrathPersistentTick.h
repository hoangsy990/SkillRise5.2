#pragma once
class OBJECT;

namespace rise { namespace growlancer {
// variant0/1/2 = mono01/02/03; attachment0/1 = source bone29/38.
// Returns native pool slot (including0), or-1 on rejection/exhaustion.
int CreateWrathPersistentParticle(int variant, int attachment, float* position,
    float* angle, float* light, float scale, OBJECT* owner);
void RetireWrathPersistentParticles(OBJECT* owner);
int CreateWrathScatterParticle(int variant, float* position, float* angle,
    float* light, float scale);

// S21 lighting_mega01..03/subtype0 (16D8F22..16D9080).
// Five-tick constructor lifetime and common decrement are caller-owned.
template<class Particle>
constexpr void UpdateWrathScatterTick(Particle& p)
{
    p.Alpha -= .15f;
    if (p.Alpha < .1f) p.Live = false;
    for (int i = 0; i < 3; ++i) p.Light[i] = p.TurningForce[i] * p.Alpha;
}
// Selected S21 particle update bodies, excluding manager lifetime decrement.
// Caller resolves a CURRENT native bone world position and owns offset storage.
// No binary offsets, S21 pointers, allocation, or buff activation live here.
template<class Particle, class Random>
constexpr void UpdateWrathMono01Tick(Particle& p, float* offset,
    const float* bonePosition, bool ownerLive, Random random)
{
    if (!ownerLive) {
        p.Live = false;
        p.LifeTime = 0;
        return;
    }
    if (p.LifeTime < 15) p.Alpha -= .2f;
    else if (p.Alpha < 1.f) p.Alpha += (random() % 2 + 2) * .1f;
    else p.Alpha = 1.f;
    if (p.Alpha < .1f) p.Live = false;
    for (int i = 0; i < 3; ++i) p.Light[i] = p.TurningForce[i] * p.Alpha;
    if (p.Scale > 0.f) p.Scale -= (random() % 3 + 2) * .01f;
    else p.Live = false;
    offset[2] += p.Gravity;
    for (int i = 0; i < 3; ++i) p.Position[i] = bonePosition[i] + offset[i];
    p.Rotation += 3.f;
}

template<class Particle, class Random>
constexpr void UpdateWrathMono02Tick(Particle& p, float* offset,
    const float* bonePosition, Random random)
{
    p.Scale -= (random() % 20 + 10) * .001f;
    // S21 subtracts .05 from a reset stack temporary, not Particle::Alpha.
    offset[2] += p.Gravity * 10.f;
    for (int i = 0; i < 3; ++i) p.Position[i] = bonePosition[i] + offset[i];
}

template<class Particle>
constexpr void UpdateWrathMono03Tick(Particle& p, const float* bonePosition)
{
    for (int i = 0; i < 3; ++i) p.Position[i] = bonePosition[i];
    p.Scale -= .05f;
    p.Alpha -= 0.0666666030883789f;
    for (int i = 0; i < 3; ++i) p.Light[i] = p.TurningForce[i] * p.Alpha;
}
// Mono02/03 source bodies have no local owner guard. The native caller must
// validate ownership before resolving bones; source outer lifecycle still under audit.
} }
