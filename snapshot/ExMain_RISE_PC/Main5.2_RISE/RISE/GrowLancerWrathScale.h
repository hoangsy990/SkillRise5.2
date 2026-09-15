#pragma once
#include "GrowLancerWrathBuffMetadata.h"

namespace rise { namespace growlancer {
// S21 physical buff events assign absolute values, never multiply current scale.
// Caller supplies a verified world exception; no S21 map numbers are transplanted.
constexpr float WrathAddedScale(float current, bool worldException)
{
    return worldException ? current : 1.1f;
}
constexpr float WrathRemovedScale() { return 0.9f; }
constexpr float WrathRefreshedScale(float baseScale, bool wrath, bool strengthened, bool eligible)
{
    return eligible && (wrath || strengthened) ? 1.1f : baseScale;
}
} }
