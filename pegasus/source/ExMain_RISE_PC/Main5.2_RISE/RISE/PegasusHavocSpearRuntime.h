#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kHavocNovaModel = MAX_MODELS + 26;
constexpr int kHavocLineModel = MAX_MODELS + 27;
constexpr int kHavocShockwaveModel = MAX_MODELS + 28;
constexpr int kHavocRootModel = MAX_MODELS + 29;
constexpr int kHavocImpactModel = MAX_MODELS + 30;
constexpr int kHavocEndModel = MAX_MODELS + 31;
constexpr int kHavocFirstModel = kHavocNovaModel;
constexpr int kHavocLastModel = kHavocEndModel;
static_assert(32 <= kDynamicModelCapacity, "Havoc Spear model tail is too small");
bool EnsureHavocSpearModel(int modelId);
void CreateHavocSpearRoot(OBJECT& target, int skill);
void CreateHavocSpearTargetImpact(OBJECT& target, int ordinal, int skill);
void InitializeHavocSpear(OBJECT& effect);
void UpdateHavocSpear(OBJECT& effect, float animationFactor);
bool RenderHavocSpear(OBJECT& effect);
}}
