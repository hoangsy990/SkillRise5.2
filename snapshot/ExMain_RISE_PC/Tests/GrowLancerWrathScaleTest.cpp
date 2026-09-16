#include "../Main5.2_RISE/RISE/GrowLancerWrathScale.h"
using namespace rise::growlancer;
static_assert(WrathAddedScale(0.9f,false) == 1.1f, "Activation assigns source scale");
static_assert(WrathAddedScale(1.35f,false) == 1.1f, "Not relative to actor class scale");
static_assert(WrathAddedScale(WrathAddedScale(0.9f,false),false) == 1.1f, "Repeat does not accumulate");
static_assert(WrathAddedScale(1.35f,true) == 1.35f, "World exception preserves scale");
static_assert(WrathRemovedScale() == 0.9f, "Removal is absolute and world-independent");
static_assert(WrathAddedScale(WrathRemovedScale(),false) == 1.1f, "Reapplication after removal");
static_assert(WrathRefreshedScale(0.93f,true,false,true) == 1.1f, "Base Wrath refresh");
static_assert(WrathRefreshedScale(0.93f,false,true,true) == 1.1f, "Strengthened Wrath refresh");
static_assert(WrathRefreshedScale(0.93f,true,true,true) == 1.1f, "Two memberships do not multiply");
static_assert(WrathRefreshedScale(0.93f,false,false,true) == 0.93f, "No Wrath retains native baseline");
static_assert(WrathRefreshedScale(1.2f,true,true,false) == 1.2f, "Excluded stage retains native scale");
