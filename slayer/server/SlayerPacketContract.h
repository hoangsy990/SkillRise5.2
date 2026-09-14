#pragma once

#include "../shared/SlayerSkillContractData.h"

namespace rise { namespace slayer {

enum PacketFamily
{
    kLegacyTargetPacket,
    kLegacySelfPacket,
    kLegacyDashPacket
};

enum TargetSource
{
    kSelectedTarget,
    kLocalActor
};

struct PacketSeed
{
    int skillId;
    PacketFamily family;
    TargetSource source;
    int rise52Opcode;
};

// Numeric opcodes are intentionally unresolved.  The 5.2 packet ABI must be
// recovered from this checkout's send/receive pair before enabling casts.
static const PacketSeed kPacketSeeds[] =
{
    { kSwordInertia, kLegacyTargetPacket, kSelectedTarget, -1 },
    { kBatFlock,     kLegacyTargetPacket, kSelectedTarget, -1 },
    { kPierceAttack, kLegacyDashPacket,   kSelectedTarget, -1 },
    { kDetection,    kLegacySelfPacket,   kLocalActor,     -1 }
};

inline const PacketSeed* FindPacketSeed(int skillId)
{
    for (unsigned i = 0; i < sizeof(kPacketSeeds) / sizeof(kPacketSeeds[0]); ++i)
        if (kPacketSeeds[i].skillId == skillId)
            return &kPacketSeeds[i];
    return 0;
}

inline bool HasEnabledRise52Opcode(int skillId)
{
    const PacketSeed* seed = FindPacketSeed(skillId);
    return seed != 0 && seed->rise52Opcode >= 0;
}

} }
