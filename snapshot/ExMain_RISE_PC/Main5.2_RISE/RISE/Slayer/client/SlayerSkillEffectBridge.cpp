#include "stdafx.h"
#include "SlayerSkillEffectBridge.h"

namespace rise { namespace slayer {

bool DispatchSlayerSkillEffect(const RuntimeEvent& event,
    SlayerSkillEffectSink& sink)
{
    switch (event.type)
    {
    case kSwordProjectileEvent:
        if (event.skillId != kSwordInertia)
            return false;
        sink.SpawnSwordProjectile(event.actorId, event.targetId, event.ordinal);
        return true;
    case kBatFlockHitEvent:
        if (event.skillId != kBatFlock)
            return false;
        sink.SpawnBatFlockHit(event.actorId, event.targetId, event.ordinal);
        return true;
    case kBatFlockDotAppliedEvent:
        if (event.skillId != kBatFlock)
            return false;
        sink.StartBatFlockDot(event.actorId, event.targetId, event.durationMs);
        return true;
    case kBatFlockDotTickEvent:
        if (event.skillId != kBatFlock)
            return false;
        sink.TickBatFlockDot(event.actorId, event.targetId);
        return true;
    case kPierceDashEvent:
        if (event.skillId != kPierceAttack)
            return false;
        sink.StartPierceDash(event.actorId, event.targetId);
        return true;
    case kPierceHitEvent:
        if (event.skillId != kPierceAttack)
            return false;
        sink.SpawnPierceHit(event.actorId, event.targetId, event.ordinal);
        return true;
    case kPierceReturnEvent:
        if (event.skillId != kPierceAttack)
            return false;
        sink.FinishPierceReturn(event.actorId, event.targetId);
        return true;
    case kDetectionMarkEvent:
        if (event.skillId != kDetection || event.targetId != -1)
            return false;
        sink.MarkDetection(event.actorId);
        return true;
    default:
        return false;
    }
}

}}
