#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kSpearstormRootModel = MAX_MODELS + 32;
constexpr int kSpearstormBladeModel = MAX_MODELS + 33;
constexpr int kSpearstormCrackModel = MAX_MODELS + 34;
constexpr int kSpearstormFirstModel = kSpearstormRootModel;
constexpr int kSpearstormLastModel = kSpearstormCrackModel;
static_assert(35 <= kDynamicModelCapacity, "Spearstorm model tail is too small");
bool EnsureSpearstormModel(int modelId);
void CreateSpearstormRoot(OBJECT& target, int skill);
void InitializeSpearstorm(OBJECT& effect);
void UpdateSpearstorm(OBJECT& effect, float animationFactor);
bool RenderSpearstorm(OBJECT& effect);
}}
