#pragma once

#include "PegasusRuntimeCapacity.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kRainingArrowRootModel = MAX_MODELS + 35;
constexpr int kRainingArrowFollowModel = MAX_MODELS + 36; // Pegasus 0x2793
constexpr int kRainingArrowImpactModel = MAX_MODELS + 37; // Pegasus 0x2794
constexpr int kRainingArrowGroundModel = MAX_MODELS + 38; // Pegasus 0x2795
constexpr int kRainingArrowRing01Model = MAX_MODELS + 39; // Pegasus 0x2796
constexpr int kRainingArrowRing02Model = MAX_MODELS + 40; // Pegasus 0x2797
constexpr int kRainingArrowRing03Model = MAX_MODELS + 41; // Pegasus 0x2798
constexpr int kRainingArrowRing04Model = MAX_MODELS + 42; // Pegasus 0x2799
constexpr int kRainingArrowRing05Model = MAX_MODELS + 43; // Pegasus 0x279A
constexpr int kRainingArrowRing06Model = MAX_MODELS + 44; // Pegasus 0x279B
constexpr int kRainingArrowRing07Model = MAX_MODELS + 45; // Pegasus 0x279C
constexpr int kRainingArrowRing08Model = MAX_MODELS + 46; // Pegasus 0x279D
constexpr int kRainingArrowFirstModel = kRainingArrowRootModel;
constexpr int kRainingArrowLastModel = kRainingArrowRing08Model;
static_assert(47 <= kDynamicModelCapacity, "Raining Arrow model tail is too small");
bool EnsureRainingArrowModel(int modelId);
int RainingArrowDamageBitmap();
bool IsRainingArrowDamageBitmap(int bitmapId);
void CreateRainingArrowRoot(OBJECT& target, int skill, short casterIndex);
void CreateRainingArrowTarget(OBJECT& target, int skill);
void CreateRainingArrowChildren(OBJECT& root);
void InitializeRainingArrow(OBJECT& effect);
void UpdateRainingArrow(OBJECT& effect, float animationFactor);
void UpdateRainingArrowProjectile(OBJECT& effect);
bool RenderRainingArrow(OBJECT& effect);
}}
