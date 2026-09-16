#include "../../GrowLancer/compat/ClassBodyAppearance.h"
using namespace rise::growlancer;
constexpr bool TestAppearance()
{
    for (unsigned cls = 0; cls < 512; ++cls)
        for (unsigned part = 0; part < 7; ++part)
            for (unsigned flags = 0; flags < 8; ++flags)
            {
                const bool equipped = (flags & 1) != 0;
                const bool showEquipment = (flags & 2) != 0;
                const bool hide = (flags & 4) != 0;
                const auto expected = cls != 7 || part >= 5 ? BaseBodyChoice::Unsupported
                    : equipped && showEquipment ? BaseBodyChoice::KeepEquipment
                    : hide ? BaseBodyChoice::Hidden : BaseBodyChoice::PrivateBase;
                if (ChooseBaseBody(cls, part, equipped, showEquipment, hide) != expected) return false;
            }
    return true;
}
static_assert(TestAppearance(), "source class boundary and full appearance truth table");
