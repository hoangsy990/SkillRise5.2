#pragma once
namespace rise { namespace growlancer {
// Caller owns the clock and excludes legacy updates. Actor action exit does
// not stop model/particle cleanup; only invalid scene/identity stops the batch.
template<class Valid, class Actor, class Models, class Particles>
constexpr unsigned RunMagicQuantumSequence(unsigned ticks, Valid& valid,
    Actor& actor, Models& models, Particles& particles)
{
    unsigned completed = 0;
    while (completed < ticks && valid())
    {
        actor();
        models();
        particles();
        ++completed;
    }
    return completed;
}
} }
