#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kElementalChargeModel = MAX_MODELS + 25;
static_assert(26 <= kDynamicModelCapacity, "Elemental Charge model tail is too small");
bool EnsureElementalChargeModel();
void CreateElementalCharge(OBJECT& target, int skill);
void CreateElementalChargeBuffAura(OBJECT& target);
void DeleteElementalChargeAuras(OBJECT& target);
void InitializeElementalCharge(OBJECT& effect);
void UpdateElementalCharge(OBJECT& effect, float animationFactor);
bool RenderElementalCharge(OBJECT& effect);
bool IsElementalChargeChild(int type);
void InitializeElementalChargeChild(OBJECT& effect);
void UpdateElementalChargeChild(OBJECT& effect, float animationFactor);
}}
