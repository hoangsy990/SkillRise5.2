#pragma once
namespace rise { namespace growlancer {
// Only private Spin particle7FDD/sub4. Manager movement precedes body;
// AdvanceParticleWholeTicks owns pre-expiry check and post-body decrement.
template<class Particle, class Move, class Random>
void UpdateSpinParticleTick(Particle& particle, Move move, Random random)
{
    if (particle.bEnableMove)
        move(particle);
    particle.Alpha -= 1.f / particle.fRepeatedlyHeight;
    for (int axis = 0; axis < 3; ++axis)
        particle.Velocity[axis] += static_cast<float>(random() % 20 - 10) * .2f;
    particle.Rotation += static_cast<float>(random() % 16);
}
} }
