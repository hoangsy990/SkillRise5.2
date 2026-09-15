#pragma once
#include "../../../GrowLancer/compat/ClassBodyAppearance.h"

namespace rise { namespace growlancer {

// Native BODYPART indices are 1..5; the private class-body descriptor is 0..4.
// This selection does not alter the native equipment/model stride of SS6.
constexpr BaseBodyChoice ChooseNativeBaseBody(unsigned sourceClassByte,
    unsigned nativeBodyPart, bool hasEquipment, bool renderEquipment,
    bool hideSkin)
{
    return nativeBodyPart >= 1 && nativeBodyPart <= 5 ?
        ChooseBaseBody(sourceClassByte, nativeBodyPart - 1, hasEquipment,
            renderEquipment, hideSkin) : BaseBodyChoice::Unsupported;
}

} }
