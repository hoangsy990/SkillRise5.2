#pragma once

#include <cstdint>

namespace rise { namespace pegasus {

constexpr std::uint32_t kCastRootDuplicateWindowMs = 250;

struct CastRootStamp
{
    int sourceKey = 0;
    std::uint32_t tick = 0;
    bool valid = false;
    bool Complete(int source)
    {
        if (!valid || sourceKey != source) return false;
        valid = false;
        return true;
    }
};

constexpr bool IsDuplicateCastRoot(std::uint32_t now, std::uint32_t previous,
    bool hasPrevious)
{
    return hasPrevious &&
        static_cast<std::uint32_t>(now - previous) < kCastRootDuplicateWindowMs;
}

}}
