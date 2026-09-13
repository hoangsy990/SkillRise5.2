#pragma once

namespace rise { namespace growlancer {
// S21 effects 7FDD/subtype0 and 8003/subtype0, not particle7FDD/subtype4.
// Constructor1481D69 and update157D468, pinned dump in SPIN_STEP_REVERSE.md.
// Hole constructor1481FB8/update157D8CC have the same audited math.
template<class Object>
void InitializeSpinFlare(Object& object, int randomRotation)
{
    object.LifeTime = 10.f;
    object.Timer = 0.f;
    object.Alpha = 0.f;
    object.Angle[2] = static_cast<float>(randomRotation);
}
template<class Object>
void UpdateSpinFlareTick(Object& object)
{
    object.Alpha += object.LifeTime > 5.f ? .2f : -.2f;
}
} }
