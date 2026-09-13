#pragma once

#include "PegasusRuntimeCapacity.h"
#include "PegasusTextureIds.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kSpiralChargeRootModel = MAX_MODELS + 2;
constexpr int kSpiralChargeFlareModel = MAX_MODELS + 3;
constexpr int kSpiralChargeShockwaveModel = MAX_MODELS + 4;
constexpr int kSpiralChargeSteamModel = MAX_MODELS + 5;
constexpr int kSpiralChargeSmokeModel = MAX_MODELS + 6;
constexpr int kSpiralChargeAnchorModel = MAX_MODELS + 7;
constexpr int kSpiralChargeFirstModel = kSpiralChargeRootModel;
constexpr int kSpiralChargeLastModel = kSpiralChargeAnchorModel;
static_assert(8 <= kDynamicModelCapacity, "Spiral Charge model tail is too small");

bool EnsureSpiralChargeModel(int modelId);
bool EnsureSpiralChargeBitmaps();
void CreateSpiralChargeRoot(OBJECT& owner, int skill);
void CreateSpiralChargeAnchor(OBJECT& owner);
void InitializeSpiralCharge(OBJECT& effect);
void CreateSpiralChargeLayers(OBJECT& root);
void UpdateSpiralCharge(OBJECT& effect, float animationFactor);
bool RenderSpiralCharge(OBJECT& effect);
}}
