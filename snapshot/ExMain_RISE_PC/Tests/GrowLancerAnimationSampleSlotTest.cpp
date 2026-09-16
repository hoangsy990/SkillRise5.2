#include "../../GrowLancer/compat/AnimationSampleSlot.h"
using rise::growlancer::AnimationSampleSlot;
constexpr bool SlotBounds()
{
    unsigned slot = 99;
    if (!AnimationSampleSlot(100u,100u,16u,3,slot) || slot != 0) return false;
    if (!AnimationSampleSlot(132u,100u,16u,3,slot) || slot != 2) return false;
    if (AnimationSampleSlot(148u,100u,16u,3,slot) || slot != 2) return false;
    if (AnimationSampleSlot(99u,100u,16u,3,slot)) return false;
    if (AnimationSampleSlot(101u,100u,16u,3,slot)) return false;
    if (AnimationSampleSlot(100u,100u,0u,3,slot)) return false;
    if (AnimationSampleSlot(100u,0u,16u,3,slot)) return false;
    if (AnimationSampleSlot(100u,100u,16u,0,slot)) return false;
    return AnimationSampleSlot(0xfffffff0u,0xffffffe0u,16u,2,slot) && slot == 1;
}
static_assert(SlotBounds(), "Pool alignment, bounds, null/empty and near-address-limit checks");
