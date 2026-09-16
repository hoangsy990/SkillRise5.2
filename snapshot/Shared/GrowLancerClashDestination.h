#pragma once

#include <math.h>
#include <stdint.h>

namespace rise { namespace growlancer {

struct ClashTile
{
    uint8_t x;
    uint8_t y;
};

// S21 10E4B67..10E4CB5, source map-tile domain. The source accesses a
// 256x256 terrain array without checking range; the isolated adapter rejects
// out-of-range destinations before any terrain lookup or packet send.
inline bool ComputeS21ClashDestination(ClashTile caster, ClashTile cursor,
    ClashTile& destination)
{
    float x = static_cast<float>(cursor.x) - static_cast<float>(caster.x);
    float y = static_cast<float>(cursor.y) - static_cast<float>(caster.y);
    const float squared = x * x + y * y;
    const float length = static_cast<float>(sqrt(static_cast<double>(squared)));
    if (!(length > -0.000001f && length < 0.000001f))
    {
        const float reciprocal = 1.0f / length;
        x *= reciprocal;
        y *= reciprocal;
    }
    const float extent = static_cast<float>(static_cast<int>(length));
    x *= extent;
    y *= extent;
    const int offsetX = static_cast<int>(x > 0.0f ? x + 0.5f : x - 0.5f);
    const int offsetY = static_cast<int>(y > 0.0f ? y + 0.5f : y - 0.5f);
    const int resultX = static_cast<int>(caster.x) + offsetX;
    const int resultY = static_cast<int>(caster.y) + offsetY;
    if (resultX < 0 || resultX > 255 || resultY < 0 || resultY > 255)
        return false;
    destination.x = static_cast<uint8_t>(resultX);
    destination.y = static_cast<uint8_t>(resultY);
    return true;
}

}}
