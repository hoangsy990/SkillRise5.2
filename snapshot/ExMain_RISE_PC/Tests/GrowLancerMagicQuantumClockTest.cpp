#include "../../GrowLancer/compat/MagicQuantumClock.h"
#include "../../GrowLancer/compat/MagicQuantumSequence.h"
using namespace rise::growlancer;
constexpr bool ClockContract()
{
    MagicQuantumClock clock;
    if (clock.Observe(100)) return false;
    clock.Start(100);
    if (!clock.Observe(139) || clock.Due() || clock.PendingMilliseconds()!=39) return false;
    if (!clock.Observe(140) || clock.Due()!=1 || !clock.Consume(1)) return false;
    if (!clock.Observe(140) || clock.Due()) return false;
    clock.Observe(351); //211ms stall, no100ms clamp
    if (clock.Due()!=5 || clock.Consume(6)) return false;
    if (!clock.Consume(2) || clock.Due()!=3 || clock.PendingMilliseconds()!=131) return false;
    clock.Consume(3);
    if (clock.PendingMilliseconds()!=11) return false;
    clock.Start(0xfffffff0u);
    if (!clock.Observe(0x18u) || clock.Due()!=1) return false;
    clock.Reset();
    return !clock.Observe(900) && clock.Due()==0;
}
struct State { unsigned completed; };
struct Valid { State& s; constexpr bool operator()() { return s.completed<2; } };
struct Noop { constexpr void operator()() {} };
struct Finish { State& s; constexpr void operator()() { ++s.completed; } };
constexpr bool PartialBatch()
{
    MagicQuantumClock clock;
    clock.Start(0); clock.Observe(215);
    State s{0}; Valid valid{s}; Noop actor, models; Finish particles{s};
    const unsigned completed=RunMagicQuantumSequence(static_cast<unsigned>(clock.Due()),valid,actor,models,particles);
    return completed==2 && clock.Consume(completed) && clock.PendingMilliseconds()==135;
}
static_assert(ClockContract(), "40ms quantum, remainder, no lost stall time, rollover/reset");
static_assert(PartialBatch(), "consume completed quanta only, preserve unprocessed time");
