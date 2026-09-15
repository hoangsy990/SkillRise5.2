#pragma once

namespace rise { namespace growlancer {
// Exactly one simulation quantum, with no clock accumulation. A phase-ordered
// caller can use this without crediting a newborn with pre-birth elapsed time.
template<class Particle, class Update>
constexpr void StepParticleTick(Particle& particle, Update& update)
{
    if (!particle.Live)
        return;
    if (particle.LifeTime <= 0.0f)
    {
        particle.Live = false;
        return;
    }
    update(particle, 1.0f);
    // Source reaches the decrement even when the selected update clears Live.
    particle.LifeTime -= 1.0f;
}

// One ordered phase over native records. Updates may append births, but must
// not unlink/reallocate the current slot. Read next AFTER update to see a child
// appended by the previous tail; save next BEFORE retiring a dead entry.
template<class Order, class Particle, class Update>
constexpr void StepOrderedParticlePhase(Order& order, Particle* particles, Update& update)
{
    unsigned slot = order.First();
    while (slot != order.End())
    {
        Particle& particle = particles[slot];
        if (!particle.Live || particle.LifeTime <= 0.f)
        {
            particle.Live = false;
            const unsigned next = order.Next(slot);
            order.Remove(slot);
            slot = next;
            continue;
        }
        StepParticleTick(particle, update);
        slot = order.Next(slot);
    }
}

// Particle manager contract: S21 0x16AA648 checks life before dispatch;
// 0x172343C decrements after dispatch. Native PARTICLE pool uses Live=false
// for reuse; never invoke EffectDestructor for a particle. The caller owns
// a per-slot accumulator and must reset it whenever that slot is allocated.
template<class Particle, class Update>
void AdvanceParticleWholeTicks(Particle& particle, float& remainder,
    float factor, Update update)
{
    if (!particle.Live)
        return;
    remainder += factor;
    while (remainder >= 1.0f && particle.Live)
    {
        remainder -= 1.0f;
        StepParticleTick(particle, update);
    }
}

template<class Object, class Update, class Destroy>
constexpr bool StepModelTick(Object& object, Update& update, Destroy& destroy)
{
    if (!object.Live) return false;
    update(object, 1.0f);
    // S21 model effects update at life0, unlike the particle precheck.
    if (object.LifeTime <= 0.0f)
    {
        destroy(&object);
        return false;
    }
    object.LifeTime -= 1.0f;
    return true;
}

template<class Object, class Select, class Update, class Destroy>
constexpr void StepPrimaryModelPhase(Object* objects, unsigned count,
    Select& select, Update& update, Destroy& destroy)
{
    for (unsigned slot = 0; slot < count; ++slot)
    {
        Object& object = objects[slot];
        if (object.Live && select(object))
            StepModelTick(object, update, destroy);
    }
}

// Only for private types whose S21 update/expiry ordering has been audited.
// Callbacks retain native update and EffectDestructor ownership.
template<class Object, class Update, class Destroy>
void AdvanceWholeTicks(Object& object, float factor, Update update, Destroy destroy)
{
    object.Timer += factor;
    while (object.Timer >= 1.0f && object.Live)
    {
        object.Timer -= 1.0f;
        if (!StepModelTick(object, update, destroy))
            break;
    }
}
} }
