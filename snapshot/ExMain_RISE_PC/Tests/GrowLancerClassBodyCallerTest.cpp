#include "../Main5.2_RISE/RISE/GrowLancerClassBodyCaller.h"

using rise::growlancer::BaseBodyChoice;
using rise::growlancer::ChooseNativeBaseBody;

static_assert(ChooseNativeBaseBody(7, 1, false, true, false) ==
    BaseBodyChoice::PrivateBase, "class7 head base body");
static_assert(ChooseNativeBaseBody(7, 5, false, true, false) ==
    BaseBodyChoice::PrivateBase, "class7 boots base body");
static_assert(ChooseNativeBaseBody(7, 2, true, true, false) ==
    BaseBodyChoice::KeepEquipment, "do not replace worn equipment");
static_assert(ChooseNativeBaseBody(7, 2, true, false, false) ==
    BaseBodyChoice::PrivateBase, "render-equipment-off uses base body");
static_assert(ChooseNativeBaseBody(7, 2, false, true, true) ==
    BaseBodyChoice::Hidden, "base skin hiding");
static_assert(ChooseNativeBaseBody(6, 2, false, true, false) ==
    BaseBodyChoice::Unsupported, "SS6 class unchanged");
static_assert(ChooseNativeBaseBody(7, 0, false, true, false) ==
    BaseBodyChoice::Unsupported, "slot zero is not a body part");
static_assert(ChooseNativeBaseBody(7, 6, false, true, false) ==
    BaseBodyChoice::Unsupported, "out-of-range part rejected");

int main() { return 0; }
