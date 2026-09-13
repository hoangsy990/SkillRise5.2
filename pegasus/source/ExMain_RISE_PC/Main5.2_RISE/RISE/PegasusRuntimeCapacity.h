#pragma once

namespace rise { namespace pegasus {
// Models is deliberately randomized by up to 1023 BMD entries. Reserve a
// separate bounded tail for isolated Pegasus resources after MAX_MODELS.
constexpr int kDynamicModelCapacity = 128;

constexpr int RuntimeModelLimit(int legacyModelLimit)
{
    return legacyModelLimit + kDynamicModelCapacity;
}

constexpr bool IsRuntimeModelId(unsigned int modelId, int legacyModelLimit)
{
    return modelId < static_cast<unsigned int>(RuntimeModelLimit(legacyModelLimit));
}
}}
