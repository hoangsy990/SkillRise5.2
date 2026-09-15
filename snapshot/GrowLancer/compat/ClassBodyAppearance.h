#pragma once
#include "ClassBodyDescriptor.h"

namespace rise { namespace growlancer {

// Scoped selection result only; never invent/translate equipment or class IDs.
enum class BaseBodyChoice { Unsupported, KeepEquipment, Hidden, PrivateBase };

// Mirrors native ordinary-player empty-slot / render-equipment-off branches.
// Caller must keep Chaos Castle, transformations, NPC and special equipment
// dispatch outside this helper. hideSkin applies only to the private base body:
// every verified base-body mesh is skin or hair, unlike arbitrary equipment.
constexpr BaseBodyChoice ChooseBaseBody(unsigned sourceClassByte, unsigned part,
    bool hasEquipment, bool renderEquipment, bool hideSkin)
{
    if (!HasVerifiedBaseBodyMaterial(sourceClassByte) || part >= 5)
        return BaseBodyChoice::Unsupported;
    if (hasEquipment && renderEquipment) return BaseBodyChoice::KeepEquipment;
    return hideSkin ? BaseBodyChoice::Hidden : BaseBodyChoice::PrivateBase;
}

} }
