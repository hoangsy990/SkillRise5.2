#include "stdafx.h"
#include "SlayerRuntimeQA.h"

#ifdef RISE_SLAYER_RUNTIME_QA

#include "Slayer/client/SlayerSkillRuntime.h"
#include "Slayer/client/SlayerNativeRuntime.h"
#include "Slayer/client/SlayerSkillEffectBridge.h"
#include "Slayer/client/SlayerSkillResources.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzInfomation.h"
#include "NewUICommon.h"

#include <vector>

extern int SelectedCharacter;

namespace rise { namespace slayerqa {
namespace {

const int kRuntimeQASkills[] = { 292, 293, 294, 295 };
int gRuntimeQASkill = 0;
bool gRuntimeQAIconPanelVisible = false;
bool gCatalogProbeLogged = false;
unsigned gLastEventCount = 0;
char gLastResult[64] = "not-run";
std::uint64_t gVirtualNowMs = 0;
slayer::SlayerSkillRuntime gRuntime;
int gAutoSequenceStep = -1;
DWORD gAutoSequenceNextTick = 0;
bool gAutoSequenceComplete = false;

const char* SkillName(int id)
{
    switch (id)
    {
    case slayer::kSwordInertia: return "Sword Inertia";
    case slayer::kBatFlock: return "Bat Flock";
    case slayer::kPierceAttack: return "Pierce Attack";
    case slayer::kDetection: return "Detection";
    default: return "unknown";
    }
}

const char* EventName(slayer::EventType type)
{
    switch (type)
    {
    case slayer::kSwordProjectileEvent: return "sword-projectile";
    case slayer::kBatFlockHitEvent: return "bat-hit";
    case slayer::kBatFlockDotAppliedEvent: return "bat-dot-applied";
    case slayer::kBatFlockDotTickEvent: return "bat-dot-tick";
    case slayer::kPierceDashEvent: return "pierce-dash";
    case slayer::kPierceHitEvent: return "pierce-hit";
    case slayer::kPierceReturnEvent: return "pierce-return";
    case slayer::kDetectionMarkEvent: return "detection-mark";
    default: return "unknown";
    }
}

void AppendQALog(const char* message);

// Resolve only live objects already owned by the native client.  The QA
// bridge must never manufacture a second actor or change the live class
// identity; a missing target simply falls back to the caster for visual QA.
OBJECT* FindNativeActor(int key)
{
    if (Hero && Hero->Object.Live && Hero->Key == key)
        return &Hero->Object;
    if (CharactersClient)
    {
        CHARACTER* character = FindCharacterByKey(key);
        if (character && character->Object.Live)
            return &character->Object;
    }
    return Hero && Hero->Object.Live ? &Hero->Object : 0;
}

OBJECT* FindNativeTarget(int key, OBJECT* fallback)
{
    if (CharactersClient && key >= 0)
    {
        CHARACTER* character = FindCharacterByKey(key);
        if (character && character->Object.Live)
            return &character->Object;
    }
    return fallback;
}

void NativeCastOrigin(OBJECT* owner, OBJECT* target, vec3_t position)
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

void NativeCastAngle(OBJECT* owner, vec3_t angle)
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

void NativeCastLight(float red, float green, float blue, vec3_t light)
{
    Vector(red, green, blue, light);
}

void LogNativeSpawn(const char* skill, const char* primitive, int type,
    int subtype, OBJECT* owner, OBJECT* target, int s21Code = 0)
{
    char line[256];
    sprintf_s(line, sizeof(line),
        "native-render skill=%s primitive=%s type=%d subtype=%d s21Controller=0x%X ownerLive=%d targetLive=%d binding=native-5.2",
        skill, primitive, type, subtype, s21Code,
        owner && owner->Live ? 1 : 0, target && target->Live ? 1 : 0);
    AppendQALog(line);
}

// The first concrete native binding intentionally uses the existing 5.2
// effect pool and its stock lifetime/movement/render path.  This gives the
// Slayer bridge real root/child ownership and renderer coverage while the
// proprietary S21 BMDs remain isolated QA assets, not copied into the base
// client.  Each callback is a single event-to-native-pool boundary.
void SpawnNativeEffect(int type, vec3_t position, vec3_t angle, vec3_t light,
    int subtype, OBJECT* owner, int skillId, int targetId, float scale = 1.f)
{
    CreateEffect(type, position, angle, light, subtype, owner, -1,
        static_cast<WORD>(skillId), 0, 0, scale,
        static_cast<short>(targetId));
}

void SpawnNativeParticle(int type, vec3_t position, vec3_t angle, vec3_t light,
    int subtype, OBJECT* owner, float scale = 1.f)
{
    CreateParticle(type, position, angle, light, subtype, scale, owner);
}

short NativeTargetIndex(OBJECT* target)
{
    if (!target || !CharactersClient)
        return -1;
    for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
        if (&CharactersClient[i].Object == target)
            return static_cast<short>(i);
    return -1;
}

void LogS21Graph(const char* skill, int root, const char* children,
    int life, int action, int sound)
{
    char line[256];
    sprintf_s(line, sizeof(line),
        "s21-visual-graph skill=%s root=0x%X children=%s life=%d action=0x%X sound=0x%X source=dump-010E6830",
        skill, root, children, life, action, sound);
    AppendQALog(line);
}

// QA-only sink: dispatches bridge events into the existing native 5.2 effect
// pool and records the primitive/lifetime boundary.  It is deliberately
// isolated behind RISE_SLAYER_RUNTIME_QA and does not alter server damage.
class RuntimeQAEffectSink : public slayer::SlayerSkillEffectSink
{
public:
    virtual void SpawnSwordProjectile(int actorId, int targetId, int ordinal)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, owner, position);
        NativeCastAngle(owner, angle);
        angle[2] += -12.f + ordinal * 12.f;
        NativeCastLight(0.55f, 0.75f, 1.f, light);
        // S21 case 0x124 creates one 0x679 controller per sword.  The
        // controller owns the 99-frame out/return path; ordinal only fans the
        // three roots and is not substituted for the recovered code.
        SpawnNativeEffect(slayer::kSwordInertiaModel, position, angle, light,
            ordinal, owner, slayer::kSwordInertia, NativeTargetIndex(target),
            0.8f);
        LogS21Graph("Sword Inertia", slayer::kS21SwordInertiaController,
            "resource-0xE0", 99, 0xE0, 0x509);
        LogNativeSpawn("Sword Inertia", "controller-0x679/resource-0xE0",
            slayer::kSwordInertiaModel, ordinal, owner, target,
            slayer::kS21SwordInertiaController);
        Log("sword-projectile", actorId, targetId, ordinal, 0);
    }
    virtual void SpawnBatFlockHit(int actorId, int targetId, int ordinal)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, target, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(0.65f, 0.15f, 0.95f, light);
        const int controller = ordinal == 0 ?
            slayer::kS21BatFlockController :
            slayer::kS21BatFlockTargetController;
        SpawnNativeEffect(slayer::kBatFlockModel, position, angle, light,
            ordinal, owner, slayer::kBatFlock, NativeTargetIndex(target), 0.85f);
        // Handler 0x682 emits the registered 0x688 child.  Keep it as a
        // separate pool object so root/child lifetime and ownership match S21.
        SpawnNativeEffect(slayer::kBatFlockTrailModel, position, angle, light,
            ordinal, owner, slayer::kBatFlock, NativeTargetIndex(target), 0.7f);
        SpawnNativeParticle(BITMAP_LIGHT, position, angle, light, 3, owner, 2.f);
        LogS21Graph("Bat Flock", controller,
            ordinal == 0 ? "0x688" : "target-follow", 10, 0xE3, 0x50B);
        LogNativeSpawn("Bat Flock", "Bat_van01", slayer::kBatFlockModel,
            ordinal, owner, target, controller);
        LogNativeSpawn("Bat Flock", "van_object02_skill/child-0x688",
            slayer::kBatFlockTrailModel, ordinal, owner, target,
            slayer::kS21BatFlockTrailChild);
        Log("bat-hit", actorId, targetId, ordinal, 0);
    }
    virtual void StartBatFlockDot(int actorId, int targetId, unsigned durationMs)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, target, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(0.45f, 0.1f, 0.8f, light);
        SpawnNativeEffect(slayer::kBatFlockTrailModel, position, angle, light,
            2, owner, slayer::kBatFlock, NativeTargetIndex(target), 1.0f);
        LogS21Graph("Bat Flock", slayer::kS21BatFlockDotController,
            "0x688", 10, 0xE3, 0x50B);
        LogNativeSpawn("Bat Flock", "dot-controller-0x684",
            slayer::kBatFlockTrailModel, 2, owner, target,
            slayer::kS21BatFlockDotController);
        Log("bat-dot-applied", actorId, targetId, 0, durationMs);
    }
    virtual void TickBatFlockDot(int actorId, int targetId)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, target, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(0.7f, 0.2f, 1.f, light);
        SpawnNativeEffect(BITMAP_MAGIC, position, angle, light, 9, owner,
            slayer::kBatFlock, NativeTargetIndex(target), 1.f);
        LogNativeSpawn("Bat Flock", "dot-tick", BITMAP_MAGIC, 9, owner, target);
        Log("bat-dot-tick", actorId, targetId, 0, 0);
    }
    virtual void StartPierceDash(int actorId, int targetId)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, target, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(1.f, 0.65f, 0.15f, light);
        SpawnNativeEffect(slayer::kPierceImpactModel, position, angle, light,
            0, owner, slayer::kPierceAttack, NativeTargetIndex(target), 1.1f);
        LogS21Graph("Pierce Attack", slayer::kS21PierceController,
            "0x80BA,0x82F6,0x82F7,0x681,0x694", 30, 0xE4, 0x50D);
        LogNativeSpawn("Pierce Attack", "controller-0x68A/impact-0x694",
            slayer::kPierceImpactModel, 0, owner, target,
            slayer::kS21PierceController);
        Log("pierce-dash", actorId, targetId, 0, 0);
    }
    virtual void SpawnPierceHit(int actorId, int targetId, int ordinal)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, target, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(1.f, 0.35f, 0.05f, light);
        // The S21 receive branch has one 0x68A root.  Additional gameplay hit
        // events add contact light only; they must not duplicate the root.
        SpawnNativeParticle(BITMAP_LIGHT, position, angle, light, 3, owner,
            1.1f + ordinal * 0.05f);
        LogNativeSpawn("Pierce Attack", "0x68A-contact-child", BITMAP_LIGHT,
            ordinal, owner, target, slayer::kS21PierceController);
        Log("pierce-hit", actorId, targetId, ordinal, 0);
    }
    virtual void FinishPierceReturn(int actorId, int targetId)
    {
        OBJECT* owner = FindNativeActor(actorId);
        OBJECT* target = FindNativeTarget(targetId, owner);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, owner, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(0.85f, 0.4f, 0.08f, light);
        // 0x681 is the recovered mode-1 follow-up of controller 0x68A.  Its
        // BMD is a zero-mesh bone carrier, so use the native joint trail and
        // keep the renderable 0x694 impact on the root object.
        if (owner && target)
            CreateJoint(BITMAP_FORCEPILLAR, owner->Position, target->Position,
                angle, 2, target, 18.f);
        LogNativeSpawn("Pierce Attack", "van_swordline01/child-0x681",
            BITMAP_FORCEPILLAR, 2, owner, target,
            slayer::kS21PierceSwordLineChild);
        Log("pierce-return", actorId, targetId, 0, 0);
    }
    virtual void MarkDetection(int actorId)
    {
        OBJECT* owner = FindNativeActor(actorId);
        vec3_t position, angle, light;
        NativeCastOrigin(owner, owner, position);
        NativeCastAngle(owner, angle);
        NativeCastLight(0.2f, 0.8f, 1.f, light);
        SpawnNativeEffect(slayer::kDetectionMarkModel, position, angle, light,
            0, owner, slayer::kDetection, -1, 1.4f);
        SpawnNativeEffect(slayer::kDetectionImpactModel, position, angle, light,
            1, owner, slayer::kDetection, -1, 1.1f);
        SpawnNativeEffect(BITMAP_LIGHT, position, angle, light, 1, owner,
            slayer::kDetection, -1, 1.5f);
        LogS21Graph("Detection", slayer::kS21DetectionController,
            "0x693,0x694,0x81CF,0x7FDD,0x82F6", 30, 0xE8, 0x511);
        LogNativeSpawn("Detection", "van_object03_skill/child-0x693",
            slayer::kDetectionMarkModel, 0, owner, 0,
            slayer::kS21DetectionMarkChild);
        LogNativeSpawn("Detection", "Van_object04_skill/child-0x694",
            slayer::kDetectionImpactModel, 1, owner, 0,
            slayer::kS21DetectionImpactChild);
        Log("detection-mark", actorId, -1, 0, 0);
    }

private:
    static void Log(const char* type, int actorId, int targetId, int ordinal,
        unsigned durationMs)
    {
        char line[256];
        sprintf_s(line, sizeof(line),
            "effect-dispatch type=%s actor=%d target=%d ordinal=%d durationMs=%u "
            "binding=native-5.2",
            type, actorId, targetId, ordinal, durationMs);
        AppendQALog(line);
    }
};

RuntimeQAEffectSink gRuntimeQAEffectSink;

void AppendQALog(const char* message)
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    char line[640];
    sprintf_s(line, sizeof(line),
        "%04u-%02u-%02uT%02u:%02u:%02u.%03uZ pid=%lu SlayerQA %s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds, GetCurrentProcessId(), message);
    OutputDebugStringA(line);
    FILE* file = 0;
    if (fopen_s(&file, "SlayerRuntimeQA.log", "a") == 0 && file)
    {
        fputs(line, file);
        fclose(file);
    }
}

OBJECT* SelectedTarget()
{
	if (SelectedCharacter >= 0 && SelectedCharacter < MAX_CHARACTERS_CLIENT)
	{
		OBJECT* target = &CharactersClient[SelectedCharacter].Object;
		if (target->Live)
			return target;
	}
	// The character scene selection is not guaranteed to remain a live target
	// after StartGame().  Keep the isolated event QA deterministic by falling
	// back to the joined Slayer actor; this never changes game state or sends a
	// damage packet.
	return Hero && Hero->Object.Live ? &Hero->Object : 0;
}

int SelectedTargetId()
{
	if (!SelectedTarget())
		return -1;
	if (SelectedCharacter >= 0 && SelectedCharacter < MAX_CHARACTERS_CLIENT)
		return SelectedCharacter;
	return Hero ? Hero->Key : -1;
}

void SetResult(const char* text)
{
    strcpy_s(gLastResult, sizeof(gLastResult), text);
}

void LogCastEvents(int skillId, bool accepted,
    const std::vector<slayer::RuntimeEvent>& events)
{
    gLastEventCount = static_cast<unsigned>(events.size());
    char line[256];
    sprintf_s(line, sizeof(line),
        "cast skill=%d name=%s accepted=%d target=%d events=%u nowMs=%llu",
        skillId, SkillName(skillId), accepted ? 1 : 0,
        SelectedTarget() ? SelectedCharacter : -1, gLastEventCount,
        static_cast<unsigned long long>(gVirtualNowMs));
    AppendQALog(line);
    for (unsigned i = 0; i < events.size(); ++i)
    {
        const slayer::RuntimeEvent& event = events[i];
        sprintf_s(line, sizeof(line),
            "event skill=%d name=%s type=%s ordinal=%d actor=%d target=%d durationMs=%u castId=%llu",
            event.skillId, SkillName(event.skillId), EventName(event.type),
            event.ordinal, event.actorId, event.targetId, event.durationMs,
            static_cast<unsigned long long>(event.castId));
        AppendQALog(line);
        const bool dispatched = slayer::DispatchSlayerSkillEffect(event,
            gRuntimeQAEffectSink);
        sprintf_s(line, sizeof(line),
            "effect-dispatch-result skill=%d type=%s accepted=%d binding=native-5.2",
            event.skillId, EventName(event.type), dispatched ? 1 : 0);
        AppendQALog(line);
    }
}

void CastCurrentSkill()
{
    const int skillId = kRuntimeQASkills[gRuntimeQASkill];
    std::vector<slayer::RuntimeEvent> events;
    slayer::CastContext context;
    context.actorId = Hero ? Hero->Key : 1;
    // QA intentionally uses the Slayer identity while the temporary actor is
    // still the existing logged-in character.  This does not mutate Hero->Class.
    context.actorClassId = slayer::kS21ClassSlayer;
    context.slayerClassEnabled = true;
    context.targetId = SelectedTargetId();
    context.targetAlive = SelectedTarget() != 0;
    // The four test rows are exercised with an explicit QA stat fixture so a
    // DK owner can inspect the event chain without changing character data.
    context.level = 400;
    context.strength = 1200;
    context.dexterity = 1200;
    context.hasBatFlock = true;
    context.targetHasBatFlock = context.targetAlive;
    context.batFlockMasteryPoints = 10;
    context.nowMs = gVirtualNowMs;
    const bool accepted = gRuntime.Cast(skillId, context, events);
    const bool actionApplied = Hero &&
        slayer::ApplyCastAction(Hero->Object, skillId);
    char actionLine[160];
    sprintf_s(actionLine, sizeof(actionLine),
        "slayer-cast-action skill=%d applied=%d action=%d modelActions=%d",
        skillId, actionApplied ? 1 : 0,
        skillId == 292 ? 284 : skillId == 293 ? 285 :
        skillId == 294 ? 286 : 287,
        Models ? Models[MODEL_PLAYER].NumActions : -1);
    AppendQALog(actionLine);
    SetResult(accepted ? "accepted" : "rejected");
    LogCastEvents(skillId, accepted, events);
}

void AdvanceDotTick(unsigned deltaMs)
{
    gVirtualNowMs += deltaMs;
    std::vector<slayer::RuntimeEvent> events;
    const int actorId = Hero ? Hero->Key : 1;
    gRuntime.Tick(actorId, gVirtualNowMs, events);
    gLastEventCount = static_cast<unsigned>(events.size());
    char line[192];
    sprintf_s(line, sizeof(line), "tick nowMs=%llu events=%u",
        static_cast<unsigned long long>(gVirtualNowMs), gLastEventCount);
    AppendQALog(line);
    for (unsigned i = 0; i < events.size(); ++i)
    {
        sprintf_s(line, sizeof(line),
            "event skill=%d name=%s type=%s ordinal=%d actor=%d target=%d durationMs=%u castId=%llu",
            events[i].skillId, SkillName(events[i].skillId),
            EventName(events[i].type), events[i].ordinal, events[i].actorId,
            events[i].targetId, events[i].durationMs,
            static_cast<unsigned long long>(events[i].castId));
        AppendQALog(line);
        const bool dispatched = slayer::DispatchSlayerSkillEffect(events[i],
            gRuntimeQAEffectSink);
        sprintf_s(line, sizeof(line),
            "effect-dispatch-result skill=%d type=%s accepted=%d binding=native-5.2",
            events[i].skillId, EventName(events[i].type), dispatched ? 1 : 0);
        AppendQALog(line);
    }
}

void TogglePanel()
{
    gRuntimeQAIconPanelVisible = !gRuntimeQAIconPanelVisible;
    AppendQALog(gRuntimeQAIconPanelVisible ? "panel=on" : "panel=off");
}

bool IsSlayerAutoQaEnabled()
{
    const DWORD attributes = GetFileAttributesA("SlayerAutoQA.enable");
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void ProbeNativeSkillCatalog()
{
    if (gCatalogProbeLogged || !SkillAttribute)
        return;

    const int skillIds[] = { slayer::kSwordInertia, slayer::kBatFlock,
        slayer::kPierceAttack, slayer::kDetection };
    const char* names[] = { "Sword Inertia", "Bat Flock", "Pierce Attack",
        "Detection" };
    bool ready = true;
    for (unsigned i = 0; i < sizeof(skillIds) / sizeof(skillIds[0]); ++i)
    {
        ready = ready && _stricmp(SkillAttribute[skillIds[i]].Name, names[i]) == 0;
        ready = ready && SkillAttribute[skillIds[i]].Magic_Icon == 86 + i;
    }
    char line[160];
    sprintf_s(line, sizeof(line), "native-skill-catalog ready=%d rows=292,293,294,295 "
        "class-column=fail-closed", ready ? 1 : 0);
    AppendQALog(line);

    int learnedSlots[4] = { -1, -1, -1, -1 };
    if (CharacterAttribute)
    {
        for (int slot = 0; slot < MAX_SKILLS; ++slot)
        {
            for (unsigned i = 0; i < sizeof(skillIds) / sizeof(skillIds[0]); ++i)
            {
                if (CharacterAttribute->Skill[slot] == skillIds[i])
                    learnedSlots[i] = slot;
            }
        }
    }
    sprintf_s(line, sizeof(line),
        "native-learned-skills class=%d slots=292:%d,293:%d,294:%d,295:%d",
        Hero ? Hero->Class : -1, learnedSlots[0], learnedSlots[1],
        learnedSlots[2], learnedSlots[3]);
    AppendQALog(line);
    gCatalogProbeLogged = true;
}

}

void AppendRuntimeQALog(const char* message)
{
    AppendQALog(message);
}

int RuntimeQASelectedSkillId()
{
    return kRuntimeQASkills[gRuntimeQASkill];
}

const char* RuntimeQASelectedSkillName()
{
    return SkillName(RuntimeQASelectedSkillId());
}

int RuntimeQASelectedTargetIndex()
{
	return SelectedTargetId();
}

bool RuntimeQASelectedTargetLive()
{
    return SelectedTarget() != 0;
}

unsigned RuntimeQALastEventCount()
{
    return gLastEventCount;
}

const char* RuntimeQALastResult()
{
    return gLastResult;
}

bool DispatchSlayerNativeReceive(CHARACTER* source, CHARACTER* target,
    int skillId)
{
    // Exercise the production Slayer packet/render bridge.  The remainder of
    // this function is retained as diagnostic fallback evidence, but the QA
    // entry point must use the same code that the isolated port ships.
    return slayer::DispatchNativeReceive(source, target, skillId);

    const slayer::SkillSeed* seed = slayer::FindSkillSeed(skillId);
    if (!source || !target || !source->Object.Live || !target->Object.Live ||
        !seed)
        return false;

    // ReceiveMagic has already resolved the packet's source/target through
    // the native 5.2 character table.  Keep those native keys in the runtime
    // event ledger so the effect sink resolves the same live objects.
    slayer::CastContext context;
    context.actorId = source->Key;
    context.actorClassId = slayer::kS21ClassSlayer;
    context.slayerClassEnabled = true;
    context.targetId = target->Key;
    context.targetAlive = target->Object.Live && target->Dead == 0;
    // An accepted ReceiveMagic packet is authoritative.  Do not reject the
    // render path because the local attribute snapshot is one frame behind
    // the server; satisfy only the deterministic runtime adapter's guard.
    context.level = source->Level > seed->skillListLevel
        ? source->Level : seed->skillListLevel;
    context.strength = seed->requiredStrength;
    context.dexterity = seed->requiredDexterity;
    context.hasBatFlock = true;
    context.targetHasBatFlock = context.targetAlive;
    context.batFlockMasteryPoints = 10;
    context.nowMs = static_cast<std::uint64_t>(GetTickCount64());

    std::vector<slayer::RuntimeEvent> events;
    const bool accepted = gRuntime.Cast(skillId, context, events);
    char line[256];
    sprintf_s(line, sizeof(line),
        "native-receive-dispatch skill=%d source=%d target=%d accepted=%d events=%u",
        skillId, source->Key, target->Key, accepted ? 1 : 0,
        static_cast<unsigned>(events.size()));
    AppendQALog(line);
    if (!accepted)
        return false;

    source->Skill = static_cast<WORD>(skillId);
    source->TargetCharacter = static_cast<short>(target - CharactersClient);
    source->SkillSuccess = true;
    source->AttackTime = 1;
    const bool actionApplied = slayer::ApplyCastAction(source->Object, skillId);
    sprintf_s(line, sizeof(line),
        "native-receive-action skill=%d applied=%d action=%d",
        skillId, actionApplied ? 1 : 0,
        skillId == 292 ? 284 : skillId == 293 ? 285 :
        skillId == 294 ? 286 : 287);
    AppendQALog(line);
    for (unsigned i = 0; i < events.size(); ++i)
    {
        const bool dispatched = slayer::DispatchSlayerSkillEffect(events[i],
            gRuntimeQAEffectSink);
        sprintf_s(line, sizeof(line),
            "native-receive-effect skill=%d type=%s accepted=%d binding=native-5.2",
            events[i].skillId, EventName(events[i].type), dispatched ? 1 : 0);
        AppendQALog(line);
    }
    return true;
}

bool HandleRuntimeQAHotKey()
{
    ProbeNativeSkillCatalog();
    if (SEASON3B::IsPress(VK_F10))
    {
        TogglePanel();
        return true;
    }
    if (SEASON3B::IsPress(VK_F11))
    {
        AdvanceDotTick(5000);
        return true;
    }
    if (SEASON3B::IsPress(VK_F6))
    {
        gRuntimeQASkill = (gRuntimeQASkill + 1) %
            (sizeof(kRuntimeQASkills) / sizeof(kRuntimeQASkills[0]));
        char line[128];
        sprintf_s(line, sizeof(line), "select skill=%d name=%s",
            RuntimeQASelectedSkillId(), RuntimeQASelectedSkillName());
        AppendQALog(line);
        return true;
    }
    if (SEASON3B::IsPress(VK_F7))
    {
        CastCurrentSkill();
        return true;
    }
    if (SEASON3B::IsPress(VK_F9))
    {
        // F9 mirrors Grow Lancer's explicit target/contact checkpoint.  For
        // Slayer it advances the DOT clock and records the selected target.
        char line[160];
        sprintf_s(line, sizeof(line), "contact-check skill=%d target=%d live=%d",
            RuntimeQASelectedSkillId(), RuntimeQASelectedTargetIndex(),
            RuntimeQASelectedTargetLive() ? 1 : 0);
        AppendQALog(line);
        AdvanceDotTick(1000);
        return true;
    }
    return false;
}

void RunRuntimeQAAutoSequence()
{
    if (!IsSlayerAutoQaEnabled() || gAutoSequenceComplete ||
        Hero == 0 || !Hero->Object.Live)
        return;

    const DWORD now = GetTickCount();
    if (gAutoSequenceStep < 0)
    {
        gAutoSequenceStep = 0;
        gAutoSequenceNextTick = now + 2000;
        char line[192];
        sprintf_s(line, sizeof(line),
            "auto-sequence armed scene=MAIN actor=%d target=%d targetLive=%d",
            Hero->Key, RuntimeQASelectedTargetIndex(),
            RuntimeQASelectedTargetLive() ? 1 : 0);
        AppendQALog(line);
        return;
    }

    if (static_cast<LONG>(now - gAutoSequenceNextTick) < 0)
        return;

    if (gAutoSequenceStep < static_cast<int>(
        sizeof(kRuntimeQASkills) / sizeof(kRuntimeQASkills[0])))
    {
        gRuntimeQASkill = gAutoSequenceStep;
        gVirtualNowMs += 1500;
        char line[192];
        sprintf_s(line, sizeof(line), "auto-sequence cast step=%d skill=%d name=%s",
            gAutoSequenceStep, RuntimeQASelectedSkillId(),
            RuntimeQASelectedSkillName());
        AppendQALog(line);
        // Exercise the same packet-to-runtime bridge used by ReceiveMagic.
        // The probe is local and is labeled as such; only a later server
        // packet can establish authoritative gameplay acceptance.
        const bool nativeAccepted = DispatchSlayerNativeReceive(Hero, Hero,
            RuntimeQASelectedSkillId());
        SetResult(nativeAccepted ? "native-receive-accepted" :
            "native-receive-rejected");
        sprintf_s(line, sizeof(line),
            "auto-sequence native-receive-probe skill=%d accepted=%d",
            RuntimeQASelectedSkillId(), nativeAccepted ? 1 : 0);
        AppendQALog(line);

        // Keep the DOT clock in the same QA run so Bat Flock's delayed ticks
        // are exercised without requiring a second manual hotkey.
        if (RuntimeQASelectedSkillId() == slayer::kBatFlock)
            AdvanceDotTick(5000);

        ++gAutoSequenceStep;
        gAutoSequenceNextTick = now + 1500;
        return;
    }

    gAutoSequenceComplete = true;
    AppendQALog("auto-sequence complete skills=292,293,294,295");
}

bool IsRuntimeQAIconPanelVisible()
{
    return gRuntimeQAIconPanelVisible;
}

} }

#endif
