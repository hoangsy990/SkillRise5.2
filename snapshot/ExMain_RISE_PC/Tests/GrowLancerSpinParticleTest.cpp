#include "../Main5.2_RISE/RISE/GrowLancerSpinParticle.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <initializer_list>
using namespace rise::growlancer;
struct Record {
    bool Live = true, bEnableMove = true;
    float LifeTime = 20.f, fRepeatedlyHeight = 20.f, Alpha = 1.f, Rotation = 0.f;
    float Velocity[3] = {}, Position[3] = {};
    int updates = 0;
};
static void Near(float a, float b) { assert(std::fabs(a-b) < .01f); }
int main()
{
    for (int life : {20, 39})
    for (float factor : {.25f, .5f, 1.f, 2.f})
    for (bool moving : {false, true})
    {
        Record r;
        r.LifeTime = r.fRepeatedlyHeight = static_cast<float>(life);
        r.bEnableMove = moving;
        float remainder = 0.f;
        int draws = 0, frames = 0;
        auto update = [&draws](Record& v, float tick) {
            assert(tick == 1.f);
            UpdateSpinParticleTick(v, [](Record& p) {
                for (int axis = 0; axis < 3; ++axis)
                    p.Position[axis] += p.Velocity[axis];
            }, [&draws]() { const int values[] = {0, 10, 19, 15}; return values[draws++ % 4]; });
            ++v.updates;
        };
        while (r.Live && frames++ < 200)
            AdvanceParticleWholeTicks(r, remainder, factor, update);
        assert(!r.Live && r.updates == life && draws == life * 4);
        Near(r.Alpha, 0.f); Near(r.Rotation, static_cast<float>(life * 15));
        Near(r.Velocity[0], -2.f * life); Near(r.Velocity[1], 0.f);
        Near(r.Velocity[2], 1.8f * life);
        const float sum = moving ? static_cast<float>(life * (life - 1)) / 2.f : 0.f;
        Near(r.Position[0], -2.f * sum); Near(r.Position[2], 1.8f * sum);
        AdvanceParticleWholeTicks(r, remainder, 100.f, update);
        assert(r.updates == life && draws == life * 4);
    }
    std::puts("PASS: 16 Spin particle lifetime/FPS/movement cases, move-before-jitter, RNG order, alpha, rotation, expiry");
    std::puts("Scope excludes native angle matrix, owner snapshot, allocator and visual parity.");
}
