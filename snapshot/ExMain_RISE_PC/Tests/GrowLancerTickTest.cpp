#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
#include <cassert>
#include <cstdio>
#include <initializer_list>

struct Record
{
    float Timer = 0;
    float LifeTime = 20;
    bool Live = true;
    int updates = 0;
    int destructions = 0;
    float lastUpdatedLife = -1;
};

static void Update(Record& r, float factor)
{
    assert(factor == 1.0f);
    r.lastUpdatedLife = r.LifeTime;
    ++r.updates;
}
static void Destroy(Record* r)
{
    assert(r->lastUpdatedLife == 0);
    assert(r->LifeTime == 0);
    ++r->destructions;
    r->Live = false;
}
static void Step(Record& r, float factor)
{
    rise::growlancer::AdvanceWholeTicks(r, factor, Update, Destroy);
}
int main()
{
    for (int life : {20, 30})
    {
        for (float factor : {0.25f, 0.5f, 1.0f, 2.0f})
        {
            Record r;
            r.LifeTime = static_cast<float>(life);
            for (int frame = 0; frame < static_cast<int>(life / factor); ++frame)
                Step(r, factor);
            assert(r.Live && r.LifeTime == 0 && r.updates == life);
            assert(r.destructions == 0);
            while (r.Live) Step(r, factor);
            assert(r.updates == life + 1 && r.destructions == 1);
            Step(r, 100.0f);
            assert(r.updates == life + 1 && r.destructions == 1);
        }
    }
    Record partial;
    Step(partial, 0.75f);
    assert(partial.updates == 0 && partial.Timer == 0.75f);
    Step(partial, 0.5f);
    assert(partial.updates == 1 && partial.Timer == 0.25f);
    Record burst;
    Step(burst, 100.0f);
    assert(!burst.Live && burst.updates == 21 && burst.destructions == 1);
    std::puts("PASS: shared tick helper; life 20/30, factors .25/.5/1/2, remainder, zero-life update, one destruction, overshoot/dead record");
    std::puts("Scope excludes native OBJECT destructor internals, movement/render and runtime FPS.");
}
