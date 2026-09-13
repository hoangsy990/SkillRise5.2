#pragma once

#include "PegasusRuntimeCapacity.h"
#include "PegasusTextureIds.h"

class OBJECT;

namespace rise { namespace pegasus {
constexpr int kShiningBirdModel = MAX_MODELS + 11;
static_assert(12 <= kDynamicModelCapacity, "Shining Bird model tail is too small");
bool EnsureShiningBirdModel();
void CreateShiningBird(OBJECT& owner, int skill);
void InitializeShiningBird(OBJECT& effect);
void UpdateShiningBird(OBJECT& effect, float animationFactor);
bool RenderShiningBird(OBJECT& effect);
}}
