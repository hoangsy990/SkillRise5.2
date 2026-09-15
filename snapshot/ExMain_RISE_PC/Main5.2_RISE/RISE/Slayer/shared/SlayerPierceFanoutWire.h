#pragma once

#include <cstdint>

namespace rise { namespace slayer {

// Private 5.2 supplement for S21's separate 0x126 list-bearing receive
// branch. These bytes are a native RISE envelope, not the S21 packet ABI.
enum { kPierceFanoutHead = 0xF4, kPierceFanoutSub = 0xE5,
       kPierceLaneRequestSub = 0xE6, kPierceFanoutMaxTargets = 10 };

struct PierceFanoutWire
{
    std::uint8_t type;
    std::uint8_t size;
    std::uint8_t head;
    std::uint8_t sub;
    std::uint8_t skill[2];
    std::uint8_t caster[2];
    std::uint8_t serial;
    std::uint8_t count;
    std::uint8_t target[kPierceFanoutMaxTargets][2];
};

static_assert(sizeof(PierceFanoutWire) == 30,
    "Pierce fanout must remain an unpadded C1 envelope");

// Per-lane request corresponding to S21 0x689's local outbound C1:00.
// 5.2 head 0x00 is chat, so this private packet has a distinct F4:E6
// route. The GameServer checks its cast serial, authorized target key,
// target tile and one-use lane before applying any strike.
struct PierceLaneRequestWire
{
    std::uint8_t type;
    std::uint8_t size;
    std::uint8_t head;
    std::uint8_t sub;
    std::uint8_t skill[2];
    std::uint8_t serial;
    std::uint8_t target[2];
    std::uint8_t tileX;
    std::uint8_t tileY;
    std::uint8_t direction;
};

static_assert(sizeof(PierceLaneRequestWire) == 12,
    "Pierce lane request must remain an unpadded C1 envelope");

} }
