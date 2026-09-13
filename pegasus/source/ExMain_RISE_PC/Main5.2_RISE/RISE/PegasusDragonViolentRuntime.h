#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kDragonViolentBaseModel = MAX_MODELS + 19;
constexpr int kDragonViolentHeadModel = MAX_MODELS + 20;
constexpr int kDragonViolentCrackModel = MAX_MODELS + 21;
constexpr int kDragonViolentWindModel = MAX_MODELS + 22;
constexpr int kDragonViolentLayerModel = MAX_MODELS + 23;
constexpr int kDragonViolentEndModel = MAX_MODELS + 24;
constexpr int kDragonViolentFirstModel = kDragonViolentBaseModel;
constexpr int kDragonViolentLastModel = kDragonViolentEndModel;
static_assert(25 <= kDynamicModelCapacity, "Dragon Violent model tail is too small");

bool EnsureDragonViolentModel(int modelId);
void CreateDragonViolentRoots(OBJECT& target, int skill);
void CreateDragonViolentTargetImpact(OBJECT& target, int skill);
void InitializeDragonViolent(OBJECT& effect);
void UpdateDragonViolent(OBJECT& effect, float animationFactor);
bool RenderDragonViolent(OBJECT& effect);
}}
