#include "../../GrowLancer/compat/ParticleBirthOrder.h"
#include "../Main5.2_RISE/RISE/GrowLancerTick.h"
using namespace rise::growlancer;
struct Record { bool Live; float LifeTime; int updates; };
struct Update
{
    ParticleBirthOrder<4>& order;
    Record* particles;
    constexpr void operator()(Record& record, float factor)
    {
        if (factor == 1.f) ++record.updates;
        if (&record == &particles[3] && record.updates == 1)
        {
            particles[0] = {true, 1.f, 0};
            order.Append(0);
        }
    }
};
constexpr bool PhaseContract()
{
    ParticleBirthOrder<4> order;
    Record particles[4] = {};
    particles[1] = {false, 20.f, 0};
    particles[2] = {true, 0.f, 0};
    particles[3] = {true, 2.f, 0};
    order.Append(1); order.Append(2); order.Append(3);
    Update update{order, particles};
    StepOrderedParticlePhase(order, particles, update);
    if (order.Contains(1) || order.Contains(2) || particles[2].Live) return false;
    if (particles[3].updates != 1 || particles[0].updates != 1) return false;
    // life1 executes, reaches0 and remains live until the next pre-dispatch.
    if (!particles[0].Live || particles[0].LifeTime != 0.f) return false;
    StepOrderedParticlePhase(order, particles, update);
    if (order.Contains(0) || particles[0].Live || particles[0].updates != 1) return false;
    if (particles[3].updates != 2 || particles[3].LifeTime != 0.f) return false;
    StepOrderedParticlePhase(order, particles, update);
    return !particles[3].Live && order.First() == order.End();
}
static_assert(PhaseContract(), "ordered native phase retirement/newborn contract");
