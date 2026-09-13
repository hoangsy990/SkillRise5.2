#pragma once

#include <cstddef>

namespace rise { namespace pegasus {

constexpr float kShiningBirdRootAdvance = 250.0f;
constexpr float kShiningBirdRootLifetime = 26.0f;
constexpr float kShiningBirdRootScale = 1.3f;
constexpr float kShiningBirdAnimationSpeed = 0.5f;
constexpr int kShiningBirdLightBones[] =
    { 21, 50, 36, 43, 46, 47, 42, 57, 60, 63, 56, 70 };
constexpr std::size_t kShiningBirdLightBoneCount =
    sizeof(kShiningBirdLightBones) / sizeof(kShiningBirdLightBones[0]);

struct ShiningBirdColor
{
    float r;
    float g;
    float b;
};

constexpr ShiningBirdColor kShiningBirdRootLight = { 0.5f, 0.7f, 0.8f };
constexpr ShiningBirdColor kShiningBirdSmokeLight = { 0.5f, 0.4f, 1.0f };
constexpr ShiningBirdColor kShiningBirdPinStarLight = { 0.9f, 0.8f, 0.8f };
constexpr ShiningBirdColor kShiningBirdWindLight = { 0.5f, 0.65f, 0.9f };
constexpr ShiningBirdColor kShiningBirdPillarLight = { 0.4f, 0.65f, 1.0f };
constexpr ShiningBirdColor kShiningBirdBoneLight = { 0.1f, 0.25f, 0.4f };

}}
