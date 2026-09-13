#include "../../GrowLancer/compat/S21MountedReaction.h"
using namespace rise::growlancer;
using Clip = S21MountedReactionClip;

static_assert(SelectS21MountedReaction(false, {false,false,0,0}) == Clip::Unarmed);
static_assert(SelectS21MountedReaction(false, {true,false,7,0}) == Clip::Hand0);
static_assert(SelectS21MountedReaction(false, {false,true,0,15}) == Clip::Hand1);
static_assert(SelectS21MountedReaction(false, {true,true,7,15}) == Clip::BothHands);
static_assert(SelectS21MountedReaction(true, {true,true,7,15}) == Clip::Unarmed);
static_assert(SelectS21MountedReaction(true, {true,false,7,0}) == Clip::Unarmed);
static_assert(SelectS21MountedReaction(true, {false,true,0,15}) == Clip::Unarmed);
static_assert(SelectS21MountedReaction(false, {true,true,1,89}) == Clip::Book);
static_assert(SelectS21MountedReaction(true, {true,true,1,89}) == Clip::Unarmed);
static_assert(SelectS21MountedReaction(true, {true,false,90,0}) == Clip::Orb);
static_assert(SelectS21MountedReaction(false, {true,true,90,114}) == Clip::Orb);
// Preserve source ordering even for combinations not normally equip-able.
static_assert(SelectS21MountedReaction(true, {true,true,90,89}) == Clip::Book);
static_assert(SelectS21MountedReaction(false, {true,true,1,114}) == Clip::Elixir);
static_assert(SelectS21MountedReaction(false, {false,false,90,89}) == Clip::Unarmed);

constexpr bool AllEligibleCategories()
{
    const unsigned char categories[] = {0,1,4,7,15};
    for (auto a : categories) for (auto b : categories)
    {
        if (SelectS21MountedReaction(false, {true,true,a,b}) != Clip::BothHands) return false;
        if (SelectS21MountedReaction(true, {true,true,a,b}) != Clip::Unarmed) return false;
        if (SelectS21MountedReaction(false, {true,false,a,b}) != Clip::Hand0) return false;
        if (SelectS21MountedReaction(false, {false,true,a,b}) != Clip::Hand1) return false;
    }
    return true;
}
static_assert(AllEligibleCategories(), "53 catalog items' five KindB categories must preserve source branch choice");
static_assert(MountedCandidateAction(Clip::Unarmed, 298) == 294);
static_assert(MountedCandidateAction(Clip::Hand0, 298) == 295);
static_assert(MountedCandidateAction(Clip::Hand1, 298) == 296);
static_assert(MountedCandidateAction(Clip::BothHands, 298) == 297);
static_assert(MountedCandidateAction(Clip::BothHands, 297) == -1);
static_assert(MountedCandidateAction(Clip::Unarmed, 294) == -1);
static_assert(MountedCandidateAction(Clip::Unarmed, -1) == -1);
static_assert(MountedCandidateAction(Clip::Book, 298) == -1);
static_assert(MountedCandidateAction(Clip::Orb, 298) == -1);
static_assert(MountedCandidateAction(Clip::Elixir, 298) == -1);
static_assert(MountedCandidateAction(static_cast<Clip>(0), 298) == -1);
