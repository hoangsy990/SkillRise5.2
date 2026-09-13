#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
// The native model pool reserves MAX_MODELS + 1024 entries. Keep the legacy
// non-UTF8 enum file untouched and consume two isolated dynamic slots.
constexpr int kSwordWrathRootModel = MAX_MODELS;
constexpr int kSwordWrathLayerModel = MAX_MODELS + 1;
static_assert(2 <= kDynamicModelCapacity, "Sword Wrath model tail is too small");
bool EnsureSwordWrathModel(int modelId);
int GetSwordWrathLineBitmap();
bool IsSwordWrathLineBitmap(int bitmapId);
int GetSwordWrathSmokeBitmap();
bool IsSwordWrathSmokeBitmap(int bitmapId);
int GetSwordWrathPinStarBitmap();
bool IsSwordWrathPinStarBitmap(int bitmapId);
void InitializeSwordWrath(OBJECT& effect);
void CreateSwordWrathLayers(OBJECT& root);
void UpdateSwordWrath(OBJECT& effect, float animationFactor);
bool RenderSwordWrath(OBJECT& effect);
}}
