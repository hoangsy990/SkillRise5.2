#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kCrusherChargeCoreModel = MAX_MODELS + 8;
constexpr int kCrusherChargeRingModel = MAX_MODELS + 9;
constexpr int kCrusherChargeShardModel = MAX_MODELS + 10;
constexpr int kCrusherChargeAuraModel = MAX_MODELS + 52;
constexpr int kCrusherChargeFirstModel = kCrusherChargeCoreModel;
constexpr int kCrusherChargeLastModel = kCrusherChargeShardModel;
static_assert(53 <= kDynamicModelCapacity, "Crusher Charge model tail is too small");

constexpr bool IsCrusherChargeModelId(int modelId)
{
    return (modelId >= kCrusherChargeFirstModel &&
        modelId <= kCrusherChargeLastModel) ||
        modelId == kCrusherChargeAuraModel;
}

bool EnsureCrusherChargeModel(int modelId);
void CreateCrusherChargeRoots(OBJECT& owner, int skill);
void CreateCrusherChargeAura(OBJECT& owner);
void DeleteCrusherChargeAura(OBJECT& owner);
void InitializeCrusherCharge(OBJECT& effect);
void CreateCrusherChargeChildren(OBJECT& ring);
void UpdateCrusherCharge(OBJECT& effect, float animationFactor);
bool RenderCrusherCharge(OBJECT& effect);
}}
