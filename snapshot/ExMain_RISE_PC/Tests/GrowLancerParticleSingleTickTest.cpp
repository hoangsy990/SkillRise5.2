#include "../Main5.2_RISE/RISE/GrowLancerTick.h"

struct Record
{
    bool Live;
    float LifeTime;
    int updates;
};
struct Update
{
    bool clearLive;
    constexpr void operator()(Record& record, float factor)
    {
        if (factor == 1.f) ++record.updates;
        if (clearLive) record.Live = false;
    }
};
constexpr bool Contract()
{
    Update update{false};
    Record record{true, 1.f, 0};
    rise::growlancer::StepParticleTick(record, update);
    if (!record.Live || record.LifeTime != 0.f || record.updates != 1) return false;
    rise::growlancer::StepParticleTick(record, update);
    if (record.Live || record.updates != 1) return false;
    rise::growlancer::StepParticleTick(record, update);
    if (record.updates != 1) return false;
    Record early{true, 3.f, 0};
    Update clear{true};
    rise::growlancer::StepParticleTick(early, clear);
    return !early.Live && early.LifeTime == 2.f && early.updates == 1;
}
static_assert(Contract(), "single particle quantum preserves source retirement ordering");
