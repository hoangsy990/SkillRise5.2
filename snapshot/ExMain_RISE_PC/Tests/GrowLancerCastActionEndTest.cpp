#include "../Main5.2_RISE/RISE/GrowLancerEffectRuntime.h"
constexpr bool VerifyCastEndRange()
{
    for (unsigned int i = 0; i < 65536; ++i)
        if (rise::growlancer::IsImportedCastAction(static_cast<unsigned short>(i)) !=
            (i >= 284 && i <= 293))
            return false;
    return true;
}
static_assert(VerifyCastEndRange(), "Only ten private imported cast clips stop");
