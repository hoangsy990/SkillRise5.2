#include "SlayerSkillDamageBridge.h"

namespace rise { namespace slayer {

bool DispatchSlayerSkillDamage(const RuntimeEvent& event, double baseDamage,
    int strength, int dexterity, SlayerSkillDamageSink& sink)
{
    switch (event.type)
    {
    case kSwordProjectileEvent:
        if (event.skillId != kSwordInertia)
            return false;
        sink.ApplyDirectDamage(event.skillId, event.actorId, event.targetId,
            event.ordinal, ApplyRegularDamageFormula(kSwordInertia,
                baseDamage, strength, dexterity));
        return true;
    case kBatFlockHitEvent:
        if (event.skillId != kBatFlock)
            return false;
        sink.ApplyDirectDamage(event.skillId, event.actorId, event.targetId,
            event.ordinal, ApplyRegularDamageFormula(kBatFlock, baseDamage,
                strength, dexterity));
        return true;
    case kPierceHitEvent:
        if (event.skillId != kPierceAttack)
            return false;
        sink.ApplyDirectDamage(event.skillId, event.actorId, event.targetId,
            event.ordinal, ApplyRegularDamageFormula(kPierceAttack,
                baseDamage, strength, dexterity));
        return true;
    case kBatFlockDotTickEvent:
        if (event.skillId != kBatFlock)
            return false;
        sink.ApplyBatFlockDotTick(event.actorId, event.targetId);
        return true;
    case kBatFlockDotAppliedEvent:
    case kPierceDashEvent:
    case kPierceReturnEvent:
    case kDetectionMarkEvent:
        return true;
    case kDemolishBuffEvent:
        return event.skillId == kDemolish &&
            event.targetId == event.actorId &&
            event.durationMs == static_cast<unsigned>(DemolishDurationSeconds() * 1000);
    default:
        return false;
    }
}

}}
