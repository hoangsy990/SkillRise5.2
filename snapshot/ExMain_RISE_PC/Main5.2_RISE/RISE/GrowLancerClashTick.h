#pragma once

namespace rise { namespace growlancer {
inline bool ClashEmissionDue(int counter, float frame)
{
    // S21 157DA26/2D compares against pinned float1.0, not zero.
    return counter == 0 && frame > 1.0f && frame <= 6.5f;
}

// Caller supplies the already normalized XYZ direction from native math.
inline void ClashInitialPosition(const float* owner, const float* direction,
    bool front, float* position)
{
    const float offset = front ? 30.0f : 1.0f;
    for (int axis = 0; axis < 3; ++axis)
        position[axis] = owner[axis] + direction[axis] * offset;
}

inline void ClashStoredPosition(const float* owner, const float* storedDirection,
    float height, float* position)
{
    for (int axis = 0; axis < 3; ++axis)
        position[axis] = owner[axis] + storedDirection[axis];
    position[2] += height;
}

// S21 subtype0:157E060..157E168; integer half27/2=13, denominator14.
template<class Object> void UpdateClashFrontScalarTick(Object& effect)
{
    const float phase = effect.LifeTime > 13.0f
        ? (27.0f - effect.LifeTime) / 14.0f
        : effect.LifeTime / 14.0f;
    effect.Alpha = phase * 0.7f;
    effect.Scale += 0.005f;
    effect.Angle[1] += 10.0f;
}

// S21 subtype0:157E26C..157E327; owner-relative positioning stays native.
template<class Object> void UpdateClashRearScalarTick(Object& effect)
{
    const float phase = effect.LifeTime > 5.0f
        ? (10.0f - effect.LifeTime) / 5.0f
        : effect.LifeTime / 5.0f;
    effect.Alpha = phase * 0.9f;
    effect.Scale += 0.05f;
}
} }
