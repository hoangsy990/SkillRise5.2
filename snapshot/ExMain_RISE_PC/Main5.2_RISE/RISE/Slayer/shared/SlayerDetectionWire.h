#pragma once

#include <cstdint>

namespace rise { namespace slayer {

// Private RISE 5.2 start signal for the authoritative Detection buff.
// S21 proves the buff/minimap behavior, not this envelope or layout.
enum { kDetectionHead = 0xF4, kDetectionSub = 0xE4 };

struct DetectionWire
{
    std::uint8_t type;
    std::uint8_t size;
    std::uint8_t head;
    std::uint8_t sub;
    std::uint8_t skill[2];
    std::uint8_t caster[2];
    std::uint8_t map[2];
    std::uint8_t durationSeconds[2];
};

static_assert(sizeof(DetectionWire) == 12,
    "Detection wire must remain an unpadded C1 envelope");

} }
