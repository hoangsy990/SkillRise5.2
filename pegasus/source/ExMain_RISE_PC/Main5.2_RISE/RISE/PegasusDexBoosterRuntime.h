#pragma once

#include "PegasusRuntimeCapacity.h"
#include "PegasusTextureIds.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kDexBoosterAuraModel = MAX_MODELS + 12;
constexpr int kDexBoosterOrchestratorModel = MAX_MODELS + 13;
constexpr int kDexBoosterGroundModel = MAX_MODELS + 14;
constexpr int kDexBoosterFlareModel = MAX_MODELS + 15;
constexpr int kDexBoosterFirstModel = kDexBoosterAuraModel;
constexpr int kDexBoosterLastModel = kDexBoosterFlareModel;
static_assert(16 <= kDynamicModelCapacity, "Dex Booster model tail is too small");

bool EnsureDexBoosterBitmaps();
void CreateDexBooster(OBJECT& owner, int skill);
void InitializeDexBooster(OBJECT& effect);
void CreateDexBoosterLayers(OBJECT& orchestrator);
void UpdateDexBooster(OBJECT& effect, float animationFactor);
bool RenderDexBooster(OBJECT& effect);
}}
