#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include "../Main5.2_RISE/RISE/GrowLancerFireParticle.h"
#include <cmath>
#include <cassert>
#include <cstdio>
#include <initializer_list>

struct Particle
{
    bool Live = true;
    float LifeTime = 20;
    int updates = 0;
    float lastLife = -1;
    float Alpha = 0, Scale = 1, Gravity = 6.4f, Rotation = 0;
    float Position[3] = {}, Light[3] = {}, TurningForce[3] = {1, .5f, .25f};
};

static void Update(Particle& p, float factor)
{
    assert(factor == 1 && p.LifeTime > 0);
    p.lastLife = p.LifeTime;
    ++p.updates;
}

int main()
{
    for (int variant = 0; variant < 3; ++variant)
    {
        Particle p;
        p.LifeTime = variant == 0 ? 15.f : 10.f;
        p.Alpha = .9f;
        int draws = 0;
        auto random = [&draws]() { ++draws; return 1; };
        rise::growlancer::UpdateBrecheFireParticleTick(p, variant, random);
        assert(draws == 2 && p.Alpha > 1.f); // Do not clamp increment.
        assert(std::fabs(p.Scale - (1.f - (6 + variant) * .01f)) < .00001f);
        assert(p.Position[0] == 0 && p.Position[1] == 0 && p.Position[2] == 6.4f);
        assert(p.Rotation == 3 && p.Light[1] == p.Alpha * .5f);
        draws = 0;
        rise::growlancer::UpdateBrecheFireParticleTick(p, variant, random);
        assert(p.Alpha == 1 && draws == 1);
        p.LifeTime -= 1;
        p.Alpha = .25f;
        p.Scale = .01f;
        draws = 0;
        rise::growlancer::UpdateBrecheFireParticleTick(p, variant, random);
        assert(!p.Live && p.Scale < 0 && draws == 1 && p.Rotation == 9);
        // Live clear must not skip the remaining selected-body operations.
        Particle zeroScale;
        zeroScale.Scale = 0;
        zeroScale.Alpha = 1;
        draws = 0;
        rise::growlancer::UpdateBrecheFireParticleTick(zeroScale, variant, random);
        assert(!zeroScale.Live && draws == 0 && zeroScale.Position[2] == 6.4f);
    }
    using rise::growlancer::AdvanceParticleWholeTicks;
    for (int life : {1, 12, 16, 17, 21, 27, 31})
    {
        for (float factor : {.25f, .5f, 1.f, 2.f})
        {
            Particle p;
            p.LifeTime = static_cast<float>(life);
            float remainder = 0;
            while (p.Live)
                AdvanceParticleWholeTicks(p, remainder, factor, Update);
            assert(p.updates == life && p.LifeTime == 0 && p.lastLife == 1);
            const float stoppedRemainder = remainder;
            AdvanceParticleWholeTicks(p, remainder, 100.f, Update);
            assert(p.updates == life && remainder == stoppedRemainder);
        }
    }
    Particle zero;
    zero.LifeTime = 0;
    float remainder = 0;
    AdvanceParticleWholeTicks(zero, remainder, 1.f, Update);
    assert(!zero.Live && zero.updates == 0);

    Particle partial;
    remainder = 0;
    AdvanceParticleWholeTicks(partial, remainder, .75f, Update);
    assert(partial.updates == 0 && remainder == .75f);
    AdvanceParticleWholeTicks(partial, remainder, .5f, Update);
    assert(partial.updates == 1 && remainder == .25f);

    Particle early;
    remainder = 0;
    AdvanceParticleWholeTicks(early, remainder, 100.f,
        [](Particle& p, float factor) { Update(p, factor); p.Live = false; });
    assert(!early.Live && early.updates == 1 && early.LifeTime == 19);

    // Reallocation owns reset of the external remainder, not the tick helper.
    early = Particle();
    remainder = 0;
    AdvanceParticleWholeTicks(early, remainder, .5f, Update);
    assert(early.Live && early.updates == 0 && remainder == .5f);
    std::puts("PASS: particle tick ordering, life0/1, fractional/multiple ticks, early Live clear and slot reset contract");
    std::puts("Scope: helper only; native pool integration, visuals and frame scheduling remain unverified.");
}
