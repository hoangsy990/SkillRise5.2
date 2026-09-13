#pragma once

#include "PegasusRuntimeCapacity.h"
#include "PegasusTextureIds.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kChaosBladeRootModel = MAX_MODELS + 16;
constexpr int kChaosBladeArcModel = MAX_MODELS + 17;
constexpr int kChaosBladeCrackModel = MAX_MODELS + 18;
constexpr int kChaosBladeFirstModel = kChaosBladeRootModel;
constexpr int kChaosBladeLastModel = kChaosBladeCrackModel;
static_assert(19 <= kDynamicModelCapacity, "Chaos Blade model tail is too small");

bool EnsureChaosBladeModel(int modelId);
void CreateChaosBladeRoot(OBJECT& target, int skill);
void CreateChaosBladeTargetImpact(OBJECT& target, int skill);
void InitializeChaosBlade(OBJECT& effect);
void UpdateChaosBlade(OBJECT& effect, float animationFactor);
bool RenderChaosBlade(OBJECT& effect);
}}
