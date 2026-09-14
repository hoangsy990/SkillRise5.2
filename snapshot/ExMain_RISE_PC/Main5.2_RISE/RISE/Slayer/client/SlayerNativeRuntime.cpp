#include "stdafx.h"
#include "SlayerNativeRuntime.h"

#ifdef RISE_SLAYER_PORT

#include "SlayerSkillResources.h"
#include "../shared/SlayerSkillContractData.h"
#include "../shared/SlayerClassContractData.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzInfomation.h"

#ifdef RISE_SLAYER_RUNTIME_QA
#include "../../SlayerRuntimeQA.h"
#endif

namespace rise { namespace slayer {
namespace {

short TargetIndex(OBJECT* target)
{
    if (!target || !CharactersClient)
        return -1;
    for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
        if (&CharactersClient[i].Object == target)
            return static_cast<short>(i);
    return -1;
}

void CastOrigin(OBJECT& owner, vec3_t position)
{
    VectorCopy(owner.Position, position);
    position[2] += 110.f;
}

void TargetOrigin(OBJECT& owner, OBJECT* target, vec3_t position)
{
    VectorCopy(target ? target->Position : owner.Position, position);
    position[2] += target ? 35.f : 110.f;
}

void SpawnModel(int type, vec3_t position, vec3_t angle, vec3_t light,
    int subtype, OBJECT& owner, int skillId, OBJECT* target, float scale)
{
    CreateEffect(type, position, angle, light, subtype, &owner, -1,
        static_cast<WORD>(skillId), 0, 0, scale, TargetIndex(target));
}

#ifdef RISE_SLAYER_RUNTIME_QA
void LogGraph(const char* skill, const char* graph)
{
    char line[384];
    sprintf_s(line, sizeof(line),
        "production-s21-graph skill=%s graph=%s", skill, graph);
    rise::slayerqa::AppendRuntimeQALog(line);
}
#else
void LogGraph(const char*, const char*) {}
#endif

#ifdef RISE_SLAYER_RUNTIME_QA
// The isolated QA client currently logs in with a legacy class-1 actor because
// the 5.2 class arrays/protocol still have MAX_CLASS == 7.  Keep that fixture
// bypass explicit and file-gated; a normal build must never render Slayer
// skills for a non-Slayer class.
bool IsQaFixtureBypassEnabled()
{
    const DWORD attributes = GetFileAttributesA("SlayerAutoQA.enable");
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}
#endif

void CreateSwordInertiaGraph(OBJECT& caster, OBJECT* target)
{
    vec3_t position, angle, light;
    CastOrigin(caster, position);
    VectorCopy(caster.Angle, angle);
    Vector(1.f, 1.f, 1.f, light);
    SpawnModel(kSwordInertiaController, position, angle, light, 0, caster,
        kSwordInertia, target, 1.f);
    PlayImpactSound(kSwordInertia, target ? target : &caster, 0);
    LogGraph("Sword Inertia",
        "roots=0x68A,0x689 handler=0x12D0F0F action=0xE0 life=99 sound=0x50D");
}

void CreateBatFlockGraph(OBJECT& caster, OBJECT& target)
{
    vec3_t casterPosition, targetPosition, angle, light;
    CastOrigin(caster, casterPosition);
    TargetOrigin(caster, &target, targetPosition);
    VectorCopy(caster.Angle, angle);
    Vector(0.72f, 0.16f, 0.35f, light);
    SpawnModel(kBatFlockController, casterPosition, angle, light, 0, caster,
        kBatFlock, &target, 1.f);
    SpawnModel(kBatFlockTargetController, targetPosition, angle, light, 0,
        caster, kBatFlock, &target, 1.f);
    SpawnModel(kBatFlockDotController, targetPosition, angle, light, 0,
        caster, kBatFlock, &target, 1.f);
    PlayImpactSound(kBatFlock, &target, 0);
    LogGraph("Bat Flock",
        "roots=0x682,0x683,0x684 handler=0x12D10A0 owner-target life=10 action=0xE3 sound=0x50B");
}

void CreatePierceAttackGraph(OBJECT& caster, OBJECT& target)
{
    vec3_t position, angle, light;
    TargetOrigin(caster, &target, position);
    VectorCopy(caster.Angle, angle);
    Vector(1.f, 1.f, 1.f, light);
    SpawnModel(kPierceController, position, angle, light, 0, caster,
        kPierceAttack, &target, 1.f);
    SpawnModel(kPierceMotionController, position, angle, light, 0, caster,
        kPierceAttack, &target, 1.f);
    PlayImpactSound(kPierceAttack, &target, 0);
    LogGraph("Pierce Attack",
        "receive-root=0x679 handler=0x12D1209; skill-path=0x68A->0x679 owner-target action=0xE4 sound=0x509");
}

// Detection's root is taken from the Slayer 295 handler; the initializer
// itself stays in the shared native effect allocator.  The dump's 295
// handler does not push a child id, so none is advertised here.
void CreateDetectionGraph(OBJECT& caster)
{
    vec3_t position, angle, light;
    CastOrigin(caster, position);
    VectorCopy(caster.Angle, angle);
    Vector(1.f, 1.f, 1.f, light);
    SpawnModel(kDetectionController, position, angle, light, 0, caster,
        kDetection, &caster, 1.f);
    LogGraph("Detection",
        "root=0x692 handler=0x12D139B self-target action=0xE8 sound=0x511");
}

void CreateDemolishGraph(OBJECT& caster)
{
    vec3_t position, angle, light;
    CastOrigin(caster, position);
    VectorCopy(caster.Angle, angle);
    Vector(1.f, 0.62f, 0.08f, light);
    SpawnModel(kDemolishController, position, angle, light, 0, caster,
        kDemolish, &caster, 1.f);
    LogGraph("Demolish",
        "root=0x695 handler=0x12D1443 self-buff action=0xE9 duration=60 sound=0x511");
}

}

bool DispatchNativeReceive(CHARACTER* source, CHARACTER* target, int skillId)
{
    if (!source || !source->Object.Live || !FindSkillSeed(skillId))
        return false;

    const bool isSlayerClass = slayer::IsSlayerClass(source->Class) ||
        slayer::IsSlayerClientClass(source->Class);
#ifdef RISE_SLAYER_RUNTIME_QA
    const bool fixtureBypass = !isSlayerClass && IsQaFixtureBypassEnabled();
    if (!isSlayerClass && !fixtureBypass)
    {
        rise::slayerqa::AppendRuntimeQALog(
            "class-gate rejected non-Slayer source");
        return false;
    }
    if (fixtureBypass)
    {
        char line[160];
        sprintf_s(line, sizeof(line),
            "class-gate fixture-bypass sourceClass=%d expected=%d",
            source->Class, slayer::kS21ClassSlayer);
        rise::slayerqa::AppendRuntimeQALog(line);
    }
#else
    if (!isSlayerClass)
        return false;
#endif

    if (skillId != kDetection && skillId != kDemolish &&
        (!target || !target->Object.Live || target->Dead != 0))
        return false;

    OBJECT* visualTarget = target ? &target->Object : &source->Object;
    source->Skill = static_cast<WORD>(skillId);
    source->TargetCharacter = TargetIndex(visualTarget);
    source->SkillSuccess = true;
    source->AttackTime = 1;
    const bool actionApplied = ApplyCastAction(source->Object, skillId);
#ifdef RISE_SLAYER_RUNTIME_QA
    char actionLine[192];
    sprintf_s(actionLine, sizeof(actionLine),
        "native-action skill=%d applied=%d currentAction=%d animationFrame=%.3f",
        skillId, actionApplied ? 1 : 0, source->Object.CurrentAction,
        static_cast<double>(source->Object.AnimationFrame));
    rise::slayerqa::AppendRuntimeQALog(actionLine);
#endif
    PlayCastSound(skillId, &source->Object);

    switch (skillId)
    {
    case kSwordInertia:
        CreateSwordInertiaGraph(source->Object, visualTarget);
        break;
    case kBatFlock:
        CreateBatFlockGraph(source->Object, *visualTarget);
        break;
    case kPierceAttack:
        CreatePierceAttackGraph(source->Object, *visualTarget);
        break;
    case kDetection:
        CreateDetectionGraph(source->Object);
        break;
    case kDemolish:
        CreateDemolishGraph(source->Object);
        break;
    default:
        return false;
    }
    return true;
}

void TickNativeRuntime(int, std::uint64_t)
{
    // Native effect objects own visual time. DOT is GameServer-authoritative;
    // the client must not manufacture it from a local QA clock.
}

void ResetNativeRuntime(int)
{
}

}}

#endif
