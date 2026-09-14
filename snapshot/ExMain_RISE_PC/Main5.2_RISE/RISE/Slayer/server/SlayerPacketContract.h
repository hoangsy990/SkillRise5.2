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
    // Existing 5.2 envelope from wsclientinline.h/Protocol.cpp. This is not
    // enough to enable a skill: the native handler still needs an explicit
    // skill case and authoritative fan-out/validation.
    int rise52EnvelopeOpcode;
    int rise52Opcode;
};

// The envelope opcodes are recovered from the 5.2 send/receive pair:
// 0x19 = PMSG_SKILL_ATTACK, 0x1E = PMSG_DURATION_SKILL_ATTACK.  The
// skill-specific opcode remains disabled (-1) until its handler is wired.
static const PacketSeed kPacketSeeds[] =
{
    { kSwordInertia, kLegacyTargetPacket, kSelectedTarget, 0x19, -1 },
    { kBatFlock,     kLegacyTargetPacket, kSelectedTarget, 0x19, -1 },
    { kPierceAttack, kLegacyDashPacket,   kSelectedTarget, 0x1E, -1 },
    { kDetection,    kLegacySelfPacket,   kLocalActor,     0x19, -1 }
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
