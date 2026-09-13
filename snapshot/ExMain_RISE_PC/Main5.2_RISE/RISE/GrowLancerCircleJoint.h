#pragma once
namespace rise { namespace growlancer {
// S21 contact subtype3: 161303F..16130E8, 16131ED..161327D.
// These are handler-local gates, not a complete movement/lifecycle adapter.
constexpr float CircleContactVelocity(int life, float velocity)
{
    if (life > 85) return velocity;
    const float accelerated = velocity + 7.f;
    return accelerated < 40.f ? accelerated : 40.f;
}
constexpr bool CircleContactReached(float homingDistance)
{
    return homingDistance <= 35.f;
}
constexpr float CircleContactBrake(float distance, float absoluteYawDelta, float velocity)
{
    // Preserve the literal subtraction, including negative results. Do not
    // replace with a conventional speed clamp or shortest-arc angle delta.
    return distance > 35.f && distance <= 70.f &&
        absoluteYawDelta > 20.f && velocity >= 10.f ? velocity - 50.f : velocity;
}
// Tick callback includes movement, homing, random consumption and tail.
// A contact kill inside tick must not skip that tick's lifetime decrement.
template<class Joint, class Tick>
constexpr void AdvanceCircleContactJoint(Joint& joint, float& remainder,
    float factor, Tick tick)
{
    if (!joint.Live || factor <= 0.f) return;
    remainder += factor;
    while (remainder >= 1.f && joint.Live)
    {
        remainder -= 1.f;
        tick(joint);
        joint.LifeTime -= 1;
        if (joint.LifeTime < 0) joint.Live = false;
    }
}
// S21 15E58DB..15E5A2A -> 1612F8F -> 1618ADA..1618B67.
// Move preserves the PRE-turn matrix for Tail; callbacks use native APIs.
// Native JOINT pool cleanup is Live=false, never EffectDestructor.
template<class Joint, class Move, class Turn, class Tail>
void AdvanceCircleJoint(Joint& joint, float& remainder, float factor,
    Move move, Turn turn, Tail tail)
{
    if (!joint.Live || factor <= 0.f)
        return;
    remainder += factor;
    while (remainder >= 1.f && joint.Live)
    {
        remainder -= 1.f;
        move(joint);
        joint.Velocity += 2.f;
        if (joint.LifeTime > 10.f)
            turn(joint);
        tail(joint);
        joint.LifeTime -= 1.f;
        if (joint.LifeTime < 0.f)
            joint.Live = false;
    }
}
} }
