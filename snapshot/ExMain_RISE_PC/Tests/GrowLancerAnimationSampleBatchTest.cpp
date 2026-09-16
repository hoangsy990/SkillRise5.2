#include "../../GrowLancer/compat/AnimationSampleBatch.h"
#include "../Main5.2_RISE/RISE/GrowLancerMagicPinTick.h"
using rise::growlancer::AnimationSampleBatch;
constexpr bool Samples()
{
    AnimationSampleBatch<3> b;
    float frame = -1;
    if (b.Read(1,1,287,0,frame) || b.Begin(0,1,287)) return false;
    if (!b.Begin(1,1,287) || !b.Append(5.5f) || !b.Append(6.2f)) return false;
    if (b.Read(1,1,287,0,frame) || !b.Append(.2f) || !b.Publish()) return false;
    if (!b.Read(1,1,287,0,frame) || frame != 5.5f) return false;
    if (!b.Read(1,1,287,0,frame) || frame != 5.5f) return false; // second controller
    if (b.Begin(1,1,287) || !b.Read(1,1,287,1,frame) || frame != 6.2f) return false;
    if (b.Read(2,1,287,0,frame) || b.Read(1,2,287,0,frame) ||
        b.Read(1,1,11,0,frame) || b.Read(1,1,287,3,frame)) return false;
    if (!b.Read(1,1,287,2,frame) || frame != .2f) return false;
    // A backward/wrapped frame is retained, never extrapolated into missed gates.
    if (b.Append(1.f) || !b.Read(1,1,287,0,frame)) return false; // immutable published batch
    if (!b.Begin(2,2,11) || !b.Append(0.f) || !b.Publish()) return false; // actor reuse / action exit
    if (b.Read(1,1,287,0,frame) || !b.Read(2,2,11,0,frame)) return false;
    b.Invalidate();
    return !b.Read(2,2,11,0,frame) && !b.Begin(2,2,11);
}
static_assert(Samples(), "bounded broadcast, stale identity, wrap, overflow and explicit reset");
constexpr bool InvalidFrame()
{
    AnimationSampleBatch<1> b;
    float frame = 9.f;
    return b.Begin(1,1,287) && !b.Append(-1.f) &&
        !b.Read(1,1,287,0,frame) && frame == 9.f;
}
static_assert(InvalidFrame(), "failed reads do not modify caller output");

constexpr bool ActionExitBetweenTicks()
{
    AnimationSampleBatch<3> b;
    if (!b.Begin(1,1,287) || !b.AppendStep(287,5.5f) ||
        !b.AppendStep(287,7.2f) || !b.AppendStep(11,0.f) || !b.Publish()) return false;
    int stage = 0;
    bool exited = false;
    unsigned short action = 999;
    float frame = -1;
    for (unsigned i = 0; i < 3; ++i)
    {
        if (!b.ReadStep(1,1,i,action,frame)) return false;
        if (action != 287) { exited = true; break; }
        if (rise::growlancer::MagicPinStageDue(stage,frame)) ++stage;
    }
    // Actual gate helper consumes earlier valid samples before action exit.
    // This is an ordering fixture, not a claimed animation speed or cast.
    if (!exited || stage != 2 || action != 11 || frame != 0.f) return false;
    if (b.Read(1,1,287,2,frame)) return false;
    if (b.ReadStep(2,1,0,action,frame) || action != 11 || frame != 0.f) return false;
    return b.ReadStep(1,1,0,action,frame) && action == 287 && frame == 5.5f;
}
static_assert(ActionExitBetweenTicks(), "Earlier emissions survive a later action exit; steps remain broadcast");

constexpr bool PublicationAndCursors()
{
    AnimationSampleBatch<2> b;
    rise::growlancer::AnimationSampleCursor a, c;
    unsigned short action = 99;
    float frame = -1;
    if (!b.Begin(1,1,287) || !a.Bind(1,1) || !c.Bind(1,1) || !b.Append(5.5f)) return false;
    if (a.Next(b,action,frame) || frame != -1 || action != 99) return false;
    if (!b.Append(7.2f) || !b.Publish()) return false;
    if (!a.Next(b,action,frame) || frame != 5.5f || a.Bind(1,1)) return false;
    if (!c.Next(b,action,frame) || frame != 5.5f) return false;
    if (!a.Next(b,action,frame) || frame != 7.2f || a.Next(b,action,frame)) return false;
    if (!b.Begin(1,2,287) || !b.Append(10.2f) || !b.Publish()) return false;
    if (c.Next(b,action,frame) || !a.Bind(1,2) || !a.Next(b,action,frame)) return false;
    if (frame != 10.2f || a.Bind(1,1) || b.Begin(1,1,287)) return false;
    a.Reset();
    if (a.Next(b,action,frame)) return false;
    if (!b.Begin(2,1,287) || !b.Append(1.f) || !b.Append(2.f)) return false;
    return !b.Append(3.f) && !b.Publish() && !b.ReadStep(2,1,0,action,frame);
}
static_assert(PublicationAndCursors(), "No partial publication, duplicate consumption, backward serial or overflow publication");
