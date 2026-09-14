#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

#include "../shared/SlayerSkillContractData.h"

namespace rise { namespace slayer {

enum EventType
{
    kSwordProjectileEvent,
    kBatFlockHitEvent,
    kBatFlockDotAppliedEvent,
    kBatFlockDotTickEvent,
    kPierceDashEvent,
    kPierceHitEvent,
    kPierceReturnEvent,
    kDetectionMarkEvent
};

struct RuntimeEvent
{
    EventType type;
    int skillId;
    int actorId;
    int targetId;
    int ordinal;
    unsigned durationMs;
    std::uint64_t castId;
};

struct CastContext
{
    int actorId;
    int actorClassId;
    bool slayerClassEnabled;
    int targetId;
    bool targetAlive;
    int level;
    int strength;
    int dexterity;
    // The caster must know Bat Flock to unlock Pierce Attack.  The target's
    // active Bat Flock debuff is a separate fact that grants the two bonus
    // Pierce hits.
    bool hasBatFlock;
    bool targetHasBatFlock;
    int batFlockMasteryPoints;
    std::uint64_t nowMs;

    CastContext()
        : actorId(-1), actorClassId(-1), slayerClassEnabled(false),
          targetId(-1), targetAlive(false), level(0), strength(0),
          dexterity(0), hasBatFlock(false), targetHasBatFlock(false),
          batFlockMasteryPoints(0), nowMs(0)
    {
    }
};

// A deterministic, engine-independent event adapter.  The native client and
// GameServer bind these events to their existing effect, movement, damage and
// minimap systems.  No S21 packet number is assumed here.
class SlayerSkillRuntime
{
public:
    SlayerSkillRuntime() : m_nextCastId(1) {}

    bool Cast(int skillId, const CastContext& context,
        std::vector<RuntimeEvent>& events);
    void Tick(int actorId, std::uint64_t nowMs, std::vector<RuntimeEvent>& events);
    void ResetActor(int actorId);

private:
    struct DotState
    {
        int actorId;
        int targetId;
        std::uint64_t nextTickMs;
        std::uint64_t expiresMs;
        std::uint64_t castId;
    };

    typedef std::pair<int, int> ActorSkillKey;
    typedef std::pair<int, int> ActorTargetKey;
    std::uint64_t m_nextCastId;
    std::map<ActorSkillKey, std::uint64_t> m_cooldowns;
    std::map<ActorTargetKey, DotState> m_dots;
};

} }
