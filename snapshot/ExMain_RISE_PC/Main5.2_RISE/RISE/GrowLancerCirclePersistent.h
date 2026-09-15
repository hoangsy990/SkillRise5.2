#pragma once

class OBJECT;

namespace rise { namespace growlancer {

// Source 13ECB52..13ECFBC, reached ONLY with buff216/221/222.
constexpr int kCircleFlareBones[] = {20, 26, 35, 25, 34};
constexpr int kCircleMonoBones[] = {26, 35};
constexpr int kCircleMonoSubType = 12;

constexpr bool IsCirclePersistentBuff(unsigned id)
{
    return id == 216 || id == 221 || id == 222;
}

// The S21 source's rand()%15 / 37.5 + .4 RGB modulation, not WorldTime.
constexpr float CircleFlarePulse(int randomValue)
{
    return .4f + static_cast<float>(randomValue % 15) / 37.5f;
}

// Selected S21 0x16DB10E..0x16DB3D5 particle subtype12 update.  Native
// particle traversal owns the life decrement and whole-tick scheduling.
template<class Particle, class Random>
constexpr void UpdateCircleUpperArmMonoTick(Particle& p, Random random)
{
    if (p.LifeTime < 15) p.Alpha -= .2f;
    else if (p.Alpha < 1.f) p.Alpha += (random() % 2 + 2) * .1f;
    else p.Alpha = 1.f;
    if (p.Alpha < .1f) p.Live = false;
    for (int i = 0; i < 3; ++i)
        p.Light[i] = p.TurningForce[i] * p.Alpha;
    if (p.Scale > 0.f) p.Scale -= (random() % 3 + 2) * .01f;
    else p.Live = false;
    p.Position[2] += p.Gravity;
    p.Rotation += 3.f;
}

// Uses the native particle allocator but leaves its Target null as in S21.
int CreateCircleUpperArmParticle(float* position, float* angle, float* light,
    float scale);
// The native model pose is already current at the character-render stage.
bool SubmitCirclePersistentVisuals(OBJECT& caster);

} }
