#pragma once

#include "SlayerSkillRuntime.h"

namespace rise { namespace slayer {

// The 5.2 renderer owns the concrete OBJECT/effect implementation.  This
// narrow sink keeps the skill port independent of those legacy types while
// making every runtime event bindable to the native effect pool.
class SlayerSkillEffectSink
{
public:
    virtual ~SlayerSkillEffectSink() {}
    virtual void SpawnSwordProjectile(int actorId, int targetId, int ordinal) = 0;
    virtual void SpawnBatFlockHit(int actorId, int targetId, int ordinal) = 0;
    virtual void StartBatFlockDot(int actorId, int targetId,
        unsigned durationMs) = 0;
    virtual void TickBatFlockDot(int actorId, int targetId) = 0;
    virtual void StartPierceDash(int actorId, int targetId) = 0;
    virtual void SpawnPierceHit(int actorId, int targetId, int ordinal) = 0;
    virtual void FinishPierceReturn(int actorId, int targetId) = 0;
    virtual void MarkDetection(int actorId) = 0;
};

// Dispatches one validated runtime event.  The sink must keep its own
// per-cast Sword Inertia collision ledger so a target touched by several
// returning swords is damaged once, as required by the guide.
bool DispatchSlayerSkillEffect(const RuntimeEvent& event,
    SlayerSkillEffectSink& sink);

}}
