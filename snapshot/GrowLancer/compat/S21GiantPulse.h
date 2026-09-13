#pragma once
#include <cmath>
#include <xmmintrin.h>

namespace rise { namespace growlancer {
// Fixed source call18F930D(-4,0), not a generic replacement for Render22.
// Caller owns sampling. No clock/global state is read or modified here.
inline float GiantPulse(float sampledTime)
{
    // CVTTSS2SI intentionally retains the pinned x86 out-of-range result.
    const int tick = _mm_cvtt_ss2si(_mm_set_ss(sampledTime));
    const int remainder = tick % -1570;
    const float radians = (static_cast<float>(remainder) * 0.0010000000474974513f) * -4.0f;
    float wave = static_cast<float>(std::cos(static_cast<double>(radians)));
    if (radians < 3.1415927410125732f)
        wave = -wave;
    return (wave + 1.0f) * 0.5f;
}

inline float GiantMeshPulse(float sampledTime)
{
    return GiantPulse(sampledTime) * 0.6000000238418579f + 0.4000000059604645f;
}
} }
