#pragma once

#include <cstdint>

namespace rise { namespace slayer {

// A private RISE 5.2 server-to-client supplement. S21 establishes the
// target-list behavior, not this envelope or its field layout.
enum { kBatFanoutHead = 0xF4, kBatFanoutSub = 0xE3,
       kBatFanoutMaxTargets = 10 };

struct BatFanoutWire
{
    std::uint8_t type;
    std::uint8_t size;
    std::uint8_t head;
    std::uint8_t sub;
    std::uint8_t skill[2];
    std::uint8_t caster[2];
    std::uint8_t count;
    std::uint8_t target[kBatFanoutMaxTargets][2];
};

static_assert(sizeof(BatFanoutWire) == 29,
    "Bat Fanout wire must remain an unpadded C1 envelope");

} }
