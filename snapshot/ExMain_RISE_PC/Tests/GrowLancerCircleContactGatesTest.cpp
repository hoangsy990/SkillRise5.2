#include "../Main5.2_RISE/RISE/GrowLancerCircleJoint.h"
using namespace rise::growlancer;
static_assert(CircleContactVelocity(86,10.f) == 10.f);
static_assert(CircleContactVelocity(85,10.f) == 17.f);
static_assert(CircleContactVelocity(84,38.f) == 40.f);
static_assert(CircleContactVelocity(80,-40.f) == -33.f);
static_assert(CircleContactReached(35.f));
static_assert(!CircleContactReached(35.01f));
static_assert(CircleContactBrake(35.f,30.f,10.f) == 10.f);
static_assert(CircleContactBrake(35.01f,30.f,10.f) == -40.f);
static_assert(CircleContactBrake(70.f,20.f,40.f) == 40.f);
static_assert(CircleContactBrake(70.f,20.01f,40.f) == -10.f);
static_assert(CircleContactBrake(70.01f,30.f,40.f) == 40.f);
static_assert(CircleContactBrake(50.f,30.f,9.99f) == 9.99f);
static_assert(CircleContactBrake(50.f,359.f,10.f) == -40.f);

struct ContactJoint { bool Live = true; int LifeTime = 100; };
constexpr bool FullLifetime(float factor)
{
    ContactJoint joint;
    float remainder = 0;
    int moves = 0, tails = 0, earlyMoves = 0;
    for (int frame = 0; frame < 1000 && joint.Live; ++frame)
        AdvanceCircleContactJoint(joint, remainder, factor, [&](ContactJoint& j) {
            ++moves;
            if (j.LifeTime > 85) ++earlyMoves;
            ++tails;
        });
    return !joint.Live && joint.LifeTime == -1 && moves == 101 &&
        tails == 101 && earlyMoves == 15;
}
static_assert(FullLifetime(1.f));
static_assert(FullLifetime(.5f));
static_assert(FullLifetime(.25f));
static_assert(FullLifetime(2.5f));
constexpr bool ContactStopsCatchup()
{
    ContactJoint joint;
    float remainder = 0;
    int tails = 0;
    AdvanceCircleContactJoint(joint, remainder, 2.5f, [&](ContactJoint& j) {
        j.Live = false;
        ++tails; // Tail still emitted after contact death.
    });
    return !joint.Live && joint.LifeTime == 99 && tails == 1 && remainder == 1.5f;
}
static_assert(ContactStopsCatchup());
constexpr bool DormantAndFractional()
{
    ContactJoint joint;
    float remainder = 0;
    int calls = 0;
    auto tick = [&](ContactJoint&) { ++calls; };
    AdvanceCircleContactJoint(joint,remainder,0.f,tick);
    AdvanceCircleContactJoint(joint,remainder,-1.f,tick);
    AdvanceCircleContactJoint(joint,remainder,.5f,tick);
    if (calls != 0 || joint.LifeTime != 100 || remainder != .5f) return false;
    AdvanceCircleContactJoint(joint,remainder,.5f,tick);
    joint.Live = false;
    AdvanceCircleContactJoint(joint,remainder,2.5f,tick);
    return calls == 1 && joint.LifeTime == 99 && remainder == 0.f;
}
static_assert(DormantAndFractional());
