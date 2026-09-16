#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
struct Record { bool Live; float LifeTime; int type; int updates; int destroyed; };
struct Select { constexpr bool operator()(const Record& r) { return r.type == 1; } };
struct Destroy { constexpr void operator()(Record* r) { ++r->destroyed; r->Live = false; } };
struct Update
{
    Record* records;
    constexpr void operator()(Record& r, float factor)
    {
        if (factor == 1.f) ++r.updates;
        if (&r == &records[1] && r.updates == 1)
        {
            records[0] = {true, 2.f, 1, 0, 0};
            records[2] = {true, 2.f, 1, 0, 0};
        }
    }
};
constexpr bool PhaseContract()
{
    Record records[5] = {};
    records[1] = {true, 0.f, 1, 0, 0};
    records[3] = {true, 0.f, 9, 0, 0};
    Select select; Destroy destroy; Update update{records};
    rise::growlancer::StepPrimaryModelPhase(records, 5, select, update, destroy);
    if (records[0].updates != 0 || records[2].updates != 1) return false;
    if (records[1].updates != 1 || records[1].destroyed != 1 || records[1].Live) return false;
    if (records[3].updates || records[3].destroyed || !records[3].Live) return false;
    rise::growlancer::StepPrimaryModelPhase(records, 5, select, update, destroy);
    return records[0].updates == 1 && records[2].updates == 2 && records[1].destroyed == 1;
}
static_assert(PhaseContract(), "forward native model slots, life0 update/destroy and private filter");
