#pragma once

#include "../client/SlayerSkillRuntime.h"

namespace rise { namespace slayer {

// The GameServer supplies the normal weapon/target damage boundary.  This
// sink only receives the skill-specific multiplier and keeps integer rounding,
// PvP/PvM and resistance handling in the native authoritative path.
class SlayerSkillDamageSink
{
public:
    virtual ~SlayerSkillDamageSink() {}
    virtual void ApplyDirectDamage(int skillId, int actorId, int targetId,
        int ordinal, double damage) = 0;
    virtual void ApplyBatFlockDotTick(int actorId, int targetId) = 0;
};

// `baseDamage` is the value already resolved by the native attack pipeline.
// Returns false only for a malformed event/skill pairing.  DOT ticks are
// deliberately callback-only because the supplied S21 calculation script
// does not expose a separate DOT formula.
bool DispatchSlayerSkillDamage(const RuntimeEvent& event, double baseDamage,
    int strength, int dexterity, SlayerSkillDamageSink& sink);

}}
