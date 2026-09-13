#pragma once

#include <cstddef>
#include <cstdint>

namespace rise { namespace pegasus {

constexpr std::uint8_t kPegasusTargetPacketHead = 0xF3;
constexpr std::uint8_t kPegasusTargetPacketSub = 0x35;
constexpr std::uint16_t kSwordBlowSkill = 1501;
// Isolated custom variant requested by the owner. It deliberately does not
// belong to the recovered Pegasus item/catalog range and never replaces 1501.
constexpr std::uint16_t kSwordBlowFlySkill = 1502;
constexpr std::uint16_t kDragonViolentTargetSkill = 242;
constexpr std::uint16_t kHavocSpearTargetSkill = 2013;
constexpr std::uint16_t kRainingArrowTargetSkill = 2023;
constexpr std::uint16_t kChaosBladeTargetSkill = 2012;
constexpr std::size_t kPegasusTargetLimit = 10;
constexpr std::size_t kPegasusTargetPayloadFixedSize = 6;

// Compatibility names retained while the isolated target-list transport is
// shared by Sword Blow and Dragon Violent.
constexpr std::uint8_t kSwordBlowPacketHead = kPegasusTargetPacketHead;
constexpr std::uint8_t kSwordBlowPacketSub = kPegasusTargetPacketSub;
constexpr std::size_t kSwordBlowTargetLimit = kPegasusTargetLimit;
constexpr std::size_t kSwordBlowPayloadFixedSize = kPegasusTargetPayloadFixedSize;

inline std::uint16_t ReadWordLE(const std::uint8_t* bytes)
{
    return static_cast<std::uint16_t>(bytes[0]) |
        (static_cast<std::uint16_t>(bytes[1]) << 8);
}

inline void WriteWordLE(std::uint8_t* bytes, std::uint16_t value)
{
    bytes[0] = static_cast<std::uint8_t>(value & 0xFF);
    bytes[1] = static_cast<std::uint8_t>(value >> 8);
}

inline std::size_t SwordBlowPayloadSize(std::size_t targetCount)
{
    return kPegasusTargetPayloadFixedSize +
        ((targetCount > kPegasusTargetLimit ? kPegasusTargetLimit : targetCount) * 2);
}

struct SwordBlowTargetPayload
{
    std::uint16_t skill;
    std::uint16_t caster;
    std::size_t targetCount;
    std::uint16_t targets[kPegasusTargetLimit];
};

inline bool DecodeSwordBlowPayload(const std::uint8_t* bytes, std::size_t size,
    SwordBlowTargetPayload& decoded)
{
    if (!bytes || size < kPegasusTargetPayloadFixedSize)
        return false;
    decoded.skill = ReadWordLE(bytes);
    decoded.caster = ReadWordLE(bytes + 2);
    const std::size_t declared = ReadWordLE(bytes + 4);
    const std::size_t available = (size - kPegasusTargetPayloadFixedSize) / 2;
    decoded.targetCount = declared < available ? declared : available;
    if (decoded.targetCount > kPegasusTargetLimit)
        decoded.targetCount = kPegasusTargetLimit;
    for (std::size_t n = 0; n < decoded.targetCount; ++n)
        decoded.targets[n] = ReadWordLE(bytes + kPegasusTargetPayloadFixedSize + (n * 2));
    return true;
}

}}
