#pragma once
namespace rise { namespace growlancer {
// Controller5DD, both supported subtypes; called with whole-tick lifetime.
inline bool CirclePairTick(float life) { return life >= 20.f && life < 25.f; }
inline bool CircleChildTick(float life)
{
    return life > 10.f && life < 20.f && static_cast<int>(life) % 3 == 0;
}
// S21 1576851..15768AE: no alpha clamp, including above1 and negative.
template<class Object>
void UpdateCircleShinyTick(Object& object)
{
    object.Alpha += object.LifeTime < 10.f ? -.1f : .2f;
    if (object.Alpha < 0.f)
        object.LifeTime = 0.f;
}
} }
