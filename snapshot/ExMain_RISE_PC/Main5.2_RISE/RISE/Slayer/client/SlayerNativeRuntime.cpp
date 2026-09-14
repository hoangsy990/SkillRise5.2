#include "stdafx.h"
#include "SlayerNativeRuntime.h"

#ifdef RISE_SLAYER_PORT

#include "SlayerSkillEffectBridge.h"
#include "SlayerSkillResources.h"
#include "SlayerSkillRuntime.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzInfomation.h"

#ifdef RISE_SLAYER_RUNTIME_QA
#include "../../SlayerRuntimeQA.h"
#endif

#include <vector>

namespace rise { namespace slayer {
namespace {

SlayerSkillRuntime gRuntime;

OBJECT* FindActor(int key)
{
    if (Hero && Hero->Object.Live && Hero->Key == key)
        return &Hero->Object;
    CHARACTER* character = FindCharacterByKey(key);
    return character && character->Object.Live ? &character->Object : 0;
}

OBJECT* FindTarget(int key, OBJECT* fallback)
{
    CHARACTER* character = key >= 0 ? FindCharacterByKey(key) : 0;
    return character && character->Object.Live ? &character->Object : fallback;
}

short TargetIndex(OBJECT* target)
{
    if (!target || !CharactersClient)
        return -1;
    for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
        if (&CharactersClient[i].Object == target)
            return static_cast<short>(i);
    return -1;
}

void Origin(OBJECT* owner, OBJECT* target, vec3_t position)
{
    if (target)
    {
        VectorCopy(target->Position, position);
    }
    else if (owner)
    {
        VectorCopy(owner->Position, position);
    }
    else
    {
        Vector(0.f, 0.f, 0.f, position);
    }
    position[2] += target && target != owner ? 35.f : 110.f;
}

void Angle(OBJECT* owner, vec3_t angle)
{
    if (owner)
    {
        VectorCopy(owner->Angle, angle);
    }
    else
    {
        Vector(0.f, 0.f, 0.f, angle);
    }
}

void SpawnEffect(int type, vec3_t position, vec3_t angle, vec3_t light,
    int subtype, OBJECT* owner, int skillId, OBJECT* target, float scale)
{
    CreateEffect(type, position, angle, light, subtype, owner, -1,
        static_cast<WORD>(skillId), 0, 0, scale, TargetIndex(target));
}

#ifdef RISE_SLAYER_RUNTIME_QA
void LogEvent(const char* text, int skillId, int actorId, int targetId,
    int ordinal)
{
    char line[256];
    sprintf_s(line, sizeof(line),
        "native-port %s skill=%d actor=%d target=%d ordinal=%d",
        text, skillId, actorId, targetId, ordinal);
    rise::slayerqa::AppendRuntimeQALog(line);
}
#else
void LogEvent(const char*, int, int, int, int) {}
#endif

class NativeEffectSink : public SlayerSkillEffectSink
{
public:
    void SpawnSwordProjectile(int actorId, int targetId, int ordinal) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, owner, position); Angle(owner, angle);
        angle[2] += -14.f + ordinal * 14.f;
        Vector(0.45f, 0.75f, 1.f, light);
        SpawnEffect(kSwordInertiaModel, position, angle, light, ordinal,
            owner, kSwordInertia, target, 0.72f);
        SpawnEffect(kSwordInertiaAuraModel, position, angle, light, ordinal,
            owner, kSwordInertia, target, 1.05f);
        LogEvent("root=0x679 child=resource-0xE0 life=99", kSwordInertia,
            actorId, targetId, ordinal);
    }

    void SpawnBatFlockHit(int actorId, int targetId, int ordinal) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, target, position); Angle(owner, angle);
        Vector(0.72f, 0.16f, 0.35f, light);
        SpawnEffect(kBatFlockTrailModel, position, angle, light, ordinal,
            owner, kBatFlock, target, 0.68f + ordinal * 0.08f);
        CreateParticle(BITMAP_LIGHT, position, angle, light, 3,
            0.8f + ordinal * 0.15f, owner);
        PlayImpactSound(kBatFlock, target, ordinal);
        LogEvent(ordinal == 0 ? "root=0x682 child=0x688" :
            "root=0x683 target-follow", kBatFlock, actorId, targetId, ordinal);
    }

    void StartBatFlockDot(int actorId, int targetId, unsigned) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, target, position); Angle(owner, angle);
        Vector(0.62f, 0.08f, 0.28f, light);
        SpawnEffect(kBatFlockModel, position, angle, light, 2, owner,
            kBatFlock, target, 0.82f);
        SpawnEffect(kBatFlockTrailModel, position, angle, light, 2, owner,
            kBatFlock, target, 0.72f);
        LogEvent("root=0x684 children=0x678,0x688 life=60", kBatFlock,
            actorId, targetId, 0);
    }

    void TickBatFlockDot(int actorId, int targetId) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, target, position); Angle(owner, angle);
        Vector(0.8f, 0.1f, 0.35f, light);
        SpawnEffect(kBatFlockModel, position, angle, light, 3, owner,
            kBatFlock, target, 0.58f);
        CreateParticle(BITMAP_MAGIC, position, angle, light, 9, 0.75f, owner);
        LogEvent("dot-tick child=0x678", kBatFlock, actorId, targetId, 0);
    }

    void StartPierceDash(int actorId, int targetId) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, target, position); Angle(owner, angle);
        Vector(1.f, 0.48f, 0.08f, light);
        SpawnEffect(kPierceImpactModel, position, angle, light, 0, owner,
            kPierceAttack, target, 0.62f);
        if (owner && target)
            CreateJoint(BITMAP_FORCEPILLAR, owner->Position, target->Position,
                angle, 2, target, 12.f);
        LogEvent("root=0x68A children=0x80BA,0x82F6,0x82F7",
            kPierceAttack, actorId, targetId, 0);
    }

    void SpawnPierceHit(int actorId, int targetId, int ordinal) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        vec3_t position, angle, light;
        Origin(owner, target, position); Angle(owner, angle);
        Vector(1.f, 0.3f, 0.04f, light);
        CreateParticle(BITMAP_LIGHT, position, angle, light, 3,
            0.75f + ordinal * 0.12f, owner);
        CreateParticle(BITMAP_SPARK + 1, position, angle, light, 7,
            0.65f + ordinal * 0.08f, owner);
        PlayImpactSound(kPierceAttack, target, ordinal);
        LogEvent("contact children=0x694,0x696", kPierceAttack,
            actorId, targetId, ordinal);
    }

    void FinishPierceReturn(int actorId, int targetId) override
    {
        OBJECT* owner = FindActor(actorId);
        OBJECT* target = FindTarget(targetId, owner);
        if (owner && target)
        {
            vec3_t angle; Angle(owner, angle);
            CreateJoint(BITMAP_FORCEPILLAR, target->Position, owner->Position,
                angle, 2, owner, 10.f);
        }
        LogEvent("return child=0x681", kPierceAttack,
            actorId, targetId, 0);
    }

    void MarkDetection(int actorId) override
    {
        OBJECT* owner = FindActor(actorId);
        vec3_t position, angle, light;
        Origin(owner, owner, position); Angle(owner, angle);
        Vector(0.16f, 0.7f, 1.f, light);
        SpawnEffect(kDetectionMarkModel, position, angle, light, 0, owner,
            kDetection, 0, 0.42f);
        SpawnEffect(kDetectionImpactModel, position, angle, light, 1, owner,
            kDetection, 0, 0.36f);
        CreateParticle(BITMAP_LIGHT, position, angle, light, 1, 0.7f, owner);
        LogEvent("root=0x692 children=0x693,0x694,0x81CF,0x7FDD,0x82F6",
            kDetection, actorId, -1, 0);
    }
};

NativeEffectSink gSink;

void DispatchEvents(const std::vector<RuntimeEvent>& events)
{
    for (std::vector<RuntimeEvent>::const_iterator it = events.begin();
        it != events.end(); ++it)
        DispatchSlayerSkillEffect(*it, gSink);
}

}

bool DispatchNativeReceive(CHARACTER* source, CHARACTER* target, int skillId)
{
    const SkillSeed* seed = FindSkillSeed(skillId);
    if (!source || !target || !source->Object.Live || !target->Object.Live || !seed)
        return false;

    CastContext context;
    context.actorId = source->Key;
    context.actorClassId = kS21ClassSlayer;
    context.slayerClassEnabled = true;
    context.targetId = target->Key;
    context.targetAlive = target->Object.Live && target->Dead == 0;
    context.level = max(source->Level, seed->skillListLevel);
    context.strength = seed->requiredStrength;
    context.dexterity = seed->requiredDexterity;
    context.hasBatFlock = true;
    context.targetHasBatFlock = context.targetAlive;
    context.batFlockMasteryPoints = 10;
    context.nowMs = static_cast<std::uint64_t>(GetTickCount64());

    std::vector<RuntimeEvent> events;
    if (!gRuntime.Cast(skillId, context, events))
        return false;

    source->Skill = static_cast<WORD>(skillId);
    source->TargetCharacter = static_cast<short>(target - CharactersClient);
    source->SkillSuccess = true;
    source->AttackTime = 1;
    ApplyCastAction(source->Object, skillId);
    PlayCastSound(skillId, &source->Object);
    DispatchEvents(events);
    return true;
}

void TickNativeRuntime(int actorKey, std::uint64_t nowMs)
{
    std::vector<RuntimeEvent> events;
    gRuntime.Tick(actorKey, nowMs, events);
    DispatchEvents(events);
}

void ResetNativeRuntime(int actorKey)
{
    gRuntime.ResetActor(actorKey);
}

}}

#endif
