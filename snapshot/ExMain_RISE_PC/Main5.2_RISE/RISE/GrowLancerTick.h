#pragma once

namespace rise { namespace growlancer {
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
        if (particle.LifeTime <= 0.0f)
        {
            particle.Live = false;
            break;
        }
        update(particle, 1.0f);
        // The selected body can clear Live; S21 still reaches this decrement.
        particle.LifeTime -= 1.0f;
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
        update(object, 1.0f);
        // S21 0x1594624 before 0x15946E3, including update at life zero.
        if (object.LifeTime <= 0.0f)
        {
            destroy(&object);
            break;
        }
        object.LifeTime -= 1.0f;
    }
}
} }
