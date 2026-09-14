#pragma once

#include <cstdint>
#include <set>

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
    virtual void MarkDetection(int actorId, unsigned durationMs) = 0;
    virtual void StartDemolishBuff(int actorId, unsigned durationMs) = 0;
};

// Collision callbacks use this ledger when a Sword Inertia projectile touches
// a target. A target may be touched by all three boomerangs but is accepted
// only once within the same cast; another target or another cast is allowed.
class SwordInertiaHitLedger
{
public:
    bool TryAccept(std::uint64_t castId, int actorId, int targetId)
    {
        return m_hits.insert(HitKey(castId, actorId, targetId)).second;
    }

    void ResetCast(std::uint64_t castId)
    {
        for (std::set<HitKey>::iterator it = m_hits.begin(); it != m_hits.end(); )
        {
            if (it->castId == castId)
                it = m_hits.erase(it);
            else
                ++it;
        }
    }

    void ResetActor(int actorId)
    {
        for (std::set<HitKey>::iterator it = m_hits.begin(); it != m_hits.end(); )
        {
            if (it->actorId == actorId)
                it = m_hits.erase(it);
            else
                ++it;
        }
    }

private:
    struct HitKey
    {
        std::uint64_t castId;
        int actorId;
        int targetId;

        HitKey(std::uint64_t cast, int actor, int target)
            : castId(cast), actorId(actor), targetId(target) {}

        bool operator<(const HitKey& other) const
        {
            if (castId != other.castId) return castId < other.castId;
            if (actorId != other.actorId) return actorId < other.actorId;
            return targetId < other.targetId;
        }
    };

    std::set<HitKey> m_hits;
};

// Dispatches one validated runtime event.  The sink must keep its own
// per-cast Sword Inertia collision ledger so a target touched by several
// returning swords is damaged once, as required by the guide.
bool DispatchSlayerSkillEffect(const RuntimeEvent& event,
    SlayerSkillEffectSink& sink);

}}
