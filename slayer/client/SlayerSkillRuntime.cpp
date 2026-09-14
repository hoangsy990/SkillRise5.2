#include "SlayerSkillRuntime.h"

namespace rise { namespace slayer {

namespace {

static RuntimeEvent MakeEvent(EventType type, int skillId, int actorId,
    int targetId, int ordinal, unsigned durationMs = 0,
    std::uint64_t castId = 0)
{
    RuntimeEvent event = {};
    event.type = type;
    event.skillId = skillId;
    event.actorId = actorId;
    event.targetId = targetId;
    event.ordinal = ordinal;
    event.durationMs = durationMs;
    event.castId = castId;
    return event;
}

static bool IsTargetCast(const CastContext& context)
{
    return context.targetAlive && context.targetId >= 0;
}

}

bool SlayerSkillRuntime::Cast(int skillId, const CastContext& context,
    std::vector<RuntimeEvent>& events)
{
    const SkillSeed* seed = FindSkillSeed(skillId);
    if (seed == 0 || context.actorId < 0 ||
        context.actorClassId != kS21ClassSlayer ||
        !context.slayerClassEnabled)
        return false;
    if (!MeetsStats(skillId, context.level, context.strength,
        context.dexterity))
        return false;

    const ActorSkillKey cooldownKey(context.actorId, skillId);
    std::map<ActorSkillKey, std::uint64_t>::const_iterator cooldown =
        m_cooldowns.find(cooldownKey);
    if (cooldown != m_cooldowns.end() && context.nowMs < cooldown->second)
        return false;

    if ((seed->flags & kRequiresTarget) != 0 && !IsTargetCast(context))
        return false;
    if (skillId == kPierceAttack &&
        (!context.hasBatFlock || context.batFlockMasteryPoints < 10))
        return false;

    const std::uint64_t castId = m_nextCastId++;

    switch (skillId)
    {
    case kSwordInertia:
        // Three boomerang projectiles share a target ledger in the native
        // adapter; each target can receive damage only once.
        for (int i = 0; i < SwordInertiaProjectileCount(); ++i)
            events.push_back(MakeEvent(kSwordProjectileEvent, skillId,
                context.actorId, context.targetId, i, 0, castId));
        break;
    case kBatFlock:
        for (int i = 0; i < BatFlockHitCount(); ++i)
            events.push_back(MakeEvent(kBatFlockHitEvent, skillId,
                context.actorId, context.targetId, i, 0, castId));
        events.push_back(MakeEvent(kBatFlockDotAppliedEvent, skillId,
            context.actorId, context.targetId, 0,
            static_cast<unsigned>(BatFlockDotDurationSeconds() * 1000), castId));
        m_dots[ActorTargetKey(context.actorId, context.targetId)] =
            DotState{context.actorId, context.targetId,
                context.nowMs + 1000,
                context.nowMs + static_cast<std::uint64_t>(BatFlockDotDurationSeconds()) * 1000,
                castId};
        break;
    case kPierceAttack:
        events.push_back(MakeEvent(kPierceDashEvent, skillId,
            context.actorId, context.targetId, 0, 0, castId));
        for (int i = 0; i < PierceAttackHitCount(context.targetHasBatFlock); ++i)
            events.push_back(MakeEvent(kPierceHitEvent, skillId,
                context.actorId, context.targetId, i, 0, castId));
        events.push_back(MakeEvent(kPierceReturnEvent, skillId,
            context.actorId, context.targetId, 0, 0, castId));
        break;
    case kDetection:
        events.push_back(MakeEvent(kDetectionMarkEvent, skillId,
            context.actorId, -1, 0,
            static_cast<unsigned>(DetectionDurationMs()), castId));
        m_cooldowns[cooldownKey] = context.nowMs + DetectionCooldownMs();
        break;
    case kDemolish:
        // Demolish is a self-buff. The authoritative server computes the
        // ignore-defense percentage; the runtime event carries its recovered
        // 60-second lifetime and ownership.
        events.push_back(MakeEvent(kDemolishBuffEvent, skillId,
            context.actorId, context.actorId, 0,
            static_cast<unsigned>(DemolishDurationSeconds() * 1000), castId));
        m_cooldowns[cooldownKey] = context.nowMs + DemolishCooldownMs();
        break;
    default:
        return false;
    }
    return true;
}

void SlayerSkillRuntime::Tick(int actorId, std::uint64_t nowMs,
    std::vector<RuntimeEvent>& events)
{
    for (std::map<ActorTargetKey, DotState>::iterator it = m_dots.begin();
        it != m_dots.end(); )
    {
        DotState& dot = it->second;
        if (dot.actorId != actorId)
        {
            ++it;
            continue;
        }
        while (dot.nextTickMs <= nowMs && dot.nextTickMs < dot.expiresMs)
        {
            events.push_back(MakeEvent(kBatFlockDotTickEvent, kBatFlock,
                dot.actorId, dot.targetId, 0, 0, dot.castId));
            dot.nextTickMs += 1000;
        }
        if (nowMs >= dot.expiresMs)
            it = m_dots.erase(it);
        else
            ++it;
    }
}

void SlayerSkillRuntime::ResetActor(int actorId)
{
    for (std::map<ActorSkillKey, std::uint64_t>::iterator it = m_cooldowns.begin();
        it != m_cooldowns.end(); )
    {
        if (it->first.first == actorId)
            it = m_cooldowns.erase(it);
        else
            ++it;
    }
    for (std::map<ActorTargetKey, DotState>::iterator it = m_dots.begin();
        it != m_dots.end(); )
    {
        if (it->first.first == actorId)
            it = m_dots.erase(it);
        else
            ++it;
    }
}

} }
