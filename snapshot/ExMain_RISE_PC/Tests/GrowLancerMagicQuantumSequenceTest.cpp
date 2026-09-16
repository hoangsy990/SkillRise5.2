#include "../../GrowLancer/compat/MagicQuantumSequence.h"
struct State { unsigned trace; unsigned frames; unsigned models; unsigned particles; bool casting; bool valid; };
struct Valid { State& s; constexpr bool operator()() { return s.valid; } };
struct Actor { State& s; constexpr void operator()() { s.trace=s.trace*10+1; if(s.casting) { ++s.frames; s.casting=false; } } };
struct Models { State& s; constexpr void operator()() { s.trace=s.trace*10+2; ++s.models; } };
struct Particles { State& s; constexpr void operator()() { s.trace=s.trace*10+3; ++s.particles; } };
constexpr bool Sequence()
{
    State s{0,0,0,0,true,true};
    Valid valid{s}; Actor actor{s}; Models models{s}; Particles particles{s};
    if(rise::growlancer::RunMagicQuantumSequence(0,valid,actor,models,particles)!=0 || s.trace) return false;
    if(rise::growlancer::RunMagicQuantumSequence(2,valid,actor,models,particles)!=2) return false;
    if(s.trace!=123123 || s.frames!=1 || s.models!=2 || s.particles!=2) return false;
    s.valid=false;
    return rise::growlancer::RunMagicQuantumSequence(3,valid,actor,models,particles)==0 && s.trace==123123;
}
static_assert(Sequence(), "tick-major sequence and post-cast effect continuation");
