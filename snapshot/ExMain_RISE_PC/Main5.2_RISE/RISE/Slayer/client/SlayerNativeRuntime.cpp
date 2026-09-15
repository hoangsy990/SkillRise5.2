#include "stdafx.h"
#include "SlayerNativeRuntime.h"

#ifdef RISE_SLAYER_PORT

#include "SlayerSkillResources.h"
#include "../shared/SlayerSkillContractData.h"
#include "../shared/SlayerClassContractData.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzInfomation.h"
#include "NewUIMiniMap.h"
#include "NewUISystem.h"

#include <algorithm>

#ifdef RISE_SLAYER_RUNTIME_QA
#include "../../SlayerRuntimeQA.h"
#endif

namespace rise { namespace slayer {
namespace {

int gPendingLocalGraphs[5] = {};

int LocalGraphSlot(int skillId)
{
    switch (skillId)
    {
    case kSwordInertia: return 0;
    case kBatFlock: return 1;
    case kPierceAttack: return 2;
    case kDetection: return 3;
    case kDemolish: return 4;
    default: return -1;
    }
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

void SpawnModel(int type, vec3_t position, vec3_t angle, vec3_t light,
    int subtype, OBJECT& owner, int skillId, OBJECT* target, float scale,
    OBJECT** createdEffect = 0)
{
    CreateEffect(type, position, angle, light, subtype, &owner, -1,
        static_cast<WORD>(skillId), 0, 0, scale, TargetIndex(target),
        createdEffect);
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
    (void)target;
    vec3_t position, angle, light;
    VectorCopy(caster.Position, position);
    VectorCopy(caster.Angle, angle);
    Vector(1.f, 1.f, 1.f, light);
    // Base 0x124 takes the 0x10E68A3 action branch: 0x679 is unconditional,
    // but the 0x68A/0x689 pair and 0x50D sound are guarded by skill 0x818.
    // The ReceiveMagic handler at 0x12D0F0F has the same upgrade guard.
    // Rebuilding those upgraded roots for base Sword produced the wrong
    // effect/texture family instead of the S21 base three-lane sword graph.
    SpawnModel(kPierceController, position, angle, light, 0, caster,
        kSwordInertia, 0, 0.f);
    LogGraph("Sword Inertia",
        "base-action-root=0x679 branch=0x10E68A3 action=0xE0 sound=0x509; 0x68A/0x689 gated by upgrade=0x818");
}

void CreateBatFlockGraph(OBJECT& caster, OBJECT& target)
{
    vec3_t position, angle, light;
    VectorCopy(caster.Position, position);
    VectorCopy(caster.Angle, angle);
    VectorCopy(caster.Light, light);
    // The S21 cast handler passes no target-list/count into CreateEffect for
    // these roots. The packet target still drives the attack action/sound;
    // it must not silently become a 0x682/0x683/0x684 visual child target.
    SpawnModel(kBatFlockController, position, angle, light, 0, caster,
        kBatFlock, 0, 0.f);
    SpawnModel(kBatFlockTargetController, position, angle, light, 0,
        caster, kBatFlock, 0, 0.f);
    SpawnModel(kBatFlockDotController, position, angle, light, 0,
        caster, kBatFlock, 0, 0.f);
    PlayImpactSound(kBatFlock, &target, 0);
    LogGraph("Bat Flock",
        "roots=0x682,0x683,0x684 handler=0x12D10A0 owner=caster target-list=empty life=10 action=0xE3 sound=0x50B");
}

void CreatePierceAttackGraph(OBJECT& caster, OBJECT& target)
{
    (void)target;
    vec3_t position, angle, light;
    VectorCopy(caster.Position, position);
    VectorCopy(caster.Angle, angle);
    VectorCopy(caster.Light, light);
    // The base 0x126 action branch at 0x10E6D1B creates only 0x68A.
    // ReceiveMagic 0x12D1209 and the later 0x679 action child both require
    // the S21 upgraded skill 0x816. Never attach that upgrade-only graph to
    // a plain Pierce Attack cast merely because its dispatch reaches here.
    SpawnModel(kSwordInertiaController, position, angle, light, 0, caster,
        kPierceAttack, 0, 0.f);
    LogGraph("Pierce Attack",
        "base-action-root=0x68A branch=0x10E6D1B action=0xE4 sound=0x50D; 0x679 gated by upgrade=0x816");
}

// Detection's root is taken from the Slayer 295 handler; the initializer
// itself stays in the shared native effect allocator.  The dump's 295
// handler does not push a child id, so none is advertised here.
void CreateDetectionGraph(OBJECT& caster)
{
    vec3_t position, angle, light;
    VectorCopy(caster.Position, position);
    VectorCopy(caster.Angle, angle);
    VectorCopy(caster.Light, light);
    SpawnModel(kDetectionController, position, angle, light, 0, caster,
        kDetection, &caster, 0.f);
    LogGraph("Detection",
        "root=0x692 handler=0x12D139B self-target action=0xE8 sound=0x511");
}

void CreateDemolishGraph(OBJECT& caster)
{
    vec3_t position, angle, light;
    VectorCopy(caster.Position, position);
    VectorCopy(caster.Angle, angle);
    VectorCopy(caster.Light, light);
    SpawnModel(kDemolishController, position, angle, light, 0, caster,
        kDemolish, &caster, 0.f);
    LogGraph("Demolish",
        "root=0x695 handler=0x12D1443 self-buff action=0xE9 duration=60 sound=0x511");
}

bool CreateCastGraph(CHARACTER& source, CHARACTER* target, int skillId)
{
    OBJECT* visualTarget = target ? &target->Object : &source.Object;
    switch (skillId)
    {
    case kSwordInertia:
        CreateSwordInertiaGraph(source.Object, visualTarget);
        return true;
    case kBatFlock:
        CreateBatFlockGraph(source.Object, *visualTarget);
        return true;
    case kPierceAttack:
        CreatePierceAttackGraph(source.Object, *visualTarget);
        return true;
    case kDetection:
        CreateDetectionGraph(source.Object);
        return true;
    case kDemolish:
        CreateDemolishGraph(source.Object);
        return true;
    default:
        return false;
    }
}

}

bool DispatchNativeLocalCast(CHARACTER* source, CHARACTER* target, int skillId)
{
    if (!source || source != Hero || !source->Object.Live ||
        !FindSkillSeed(skillId))
        return false;
    const bool isSlayerClass = slayer::IsSlayerClass(source->Class) ||
        slayer::IsSlayerClientClass(source->Class);
#ifdef RISE_SLAYER_RUNTIME_QA
    if (!isSlayerClass && !IsQaFixtureBypassEnabled())
        return false;
#else
    if (!isSlayerClass)
        return false;
#endif
    if (skillId != kDetection && skillId != kDemolish &&
        (!target || !target->Object.Live))
        return false;
    source->AttackTime = 1;
    PlayCastSound(skillId, &source->Object);
    if (!CreateCastGraph(*source, target, skillId))
        return false;
    const int slot = LocalGraphSlot(skillId);
    if (slot >= 0 && gPendingLocalGraphs[slot] < 32)
        ++gPendingLocalGraphs[slot];
    return true;
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

    // The S21 receive handler resolves an actor pointer, not a living-only
    // monster filter.  Damage can kill a target before the 5.2 cast packet
    // reaches this bridge; the authored cast visuals must still be emitted.
    if (skillId != kDetection && skillId != kDemolish &&
        (!target || !target->Object.Live))
        return false;

    const int localSlot = source == Hero ? LocalGraphSlot(skillId) : -1;
    if (localSlot >= 0 && gPendingLocalGraphs[localSlot] > 0)
    {
        --gPendingLocalGraphs[localSlot];
        // S21's 292/293/295/297 ReceiveMagic handlers skip or upgrade-gate
        // the local actor. Its action branch already seeded the local graph.
        // The canonical 5.2 0x19 acknowledgment confirms the server cast,
        // but must not reset that animation or duplicate its root children.
        source->SkillSuccess = true;
        return true;
    }

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

    return CreateCastGraph(*source, target, skillId);
}

bool DispatchBatFanout(CHARACTER* source, const short* targetIndexes,
    int targetCount)
{
    if (!source || !source->Object.Live || !targetIndexes ||
        targetCount <= 0 || targetCount > 10)
        return false;
    const bool isSlayerClass = slayer::IsSlayerClass(source->Class) ||
        slayer::IsSlayerClientClass(source->Class);
#ifdef RISE_SLAYER_RUNTIME_QA
    if (!isSlayerClass && !IsQaFixtureBypassEnabled())
        return false;
#else
    if (!isSlayerClass)
        return false;
#endif

    vec3_t position, angle, light;
    VectorCopy(source->Object.Position, position);
    VectorCopy(source->Object.Angle, angle);
    VectorCopy(source->Object.Light, light);
    OBJECT* flockRoot = 0;
    OBJECT* markRoot = 0;
    // The S21 list-bearing receive branch creates subtype-1 0x682 and
    // subtype-0 0x683. These are supplemental to the canonical cast roots.
    SpawnModel(kBatFlockController, position, angle, light, 1,
        source->Object, kBatFlock, 0, 0.f, &flockRoot);
    SpawnModel(kBatFlockTargetController, position, angle, light, 0,
        source->Object, kBatFlock, 0, 0.f, &markRoot);
    if (!flockRoot || !markRoot)
        return false;
    SetBatFlockTargets(*flockRoot, targetIndexes, targetCount);
    SetBatFlockTargets(*markRoot, targetIndexes, targetCount);
    LogGraph("Bat Flock",
        "supplemental roots=0x682 subtype1,0x683 subtype0 target-list<=10 handler=0x12A76B5");
    return true;
}

bool DispatchPierceFanout(CHARACTER* source, const short* targetIndexes,
    int targetCount, std::uint8_t castSerial)
{
    if (!source || !source->Object.Live || !targetIndexes ||
        targetCount <= 0 || targetCount > 10)
        return false;
    const bool isSlayerClass = slayer::IsSlayerClass(source->Class) ||
        slayer::IsSlayerClientClass(source->Class);
#ifdef RISE_SLAYER_RUNTIME_QA
    if (!isSlayerClass && !IsQaFixtureBypassEnabled())
        return false;
#else
    if (!isSlayerClass)
        return false;
#endif

    vec3_t position, angle, light;
    VectorCopy(source->Object.Position, position);
    VectorCopy(source->Object.Angle, angle);
    VectorCopy(source->Object.Light, light);
    OBJECT* listRoot = 0;
    // S21's 0x12A7945 supplemental receive always creates 0x689 and
    // appends the packet's resolved target keys. Its 0x68A is only created
    // for remote actors, already covered by 5.2's ordinary cast bridge.
    SpawnModel(kSwordSecondaryController, position, angle, light, 0,
        source->Object, kPierceAttack, 0, 0.f, &listRoot);
    if (!listRoot)
        return false;
    SetPierceTargets(*listRoot, targetIndexes, targetCount, castSerial);
    LogGraph("Pierce Attack",
        "supplemental root=0x689 target-list<=10 handler=0x12A7945");
    return true;
}

bool DispatchDetectionReveal(CHARACTER* source, int map,
    int durationSeconds)
{
    if (!source || source != Hero || !source->Object.Live ||
        durationSeconds != 60 || !g_pNewUIMiniMap)
        return false;
    const bool isSlayerClass = slayer::IsSlayerClass(source->Class) ||
        slayer::IsSlayerClientClass(source->Class);
#ifdef RISE_SLAYER_RUNTIME_QA
    if (!isSlayerClass && !IsQaFixtureBypassEnabled())
        return false;
#else
    if (!isSlayerClass)
        return false;
#endif
    g_pNewUIMiniMap->BeginSlayerDetection(map, durationSeconds);
    LogGraph("Detection",
        "server-buff-start minimap life-form reveal duration=60s");
    return true;
}

void TickNativeRuntime(int, std::uint64_t)
{
    // Native effect objects own visual time. DOT is GameServer-authoritative;
    // the client must not manufacture it from a local QA clock.
}

void ResetNativeRuntime(int)
{
    std::fill(gPendingLocalGraphs, gPendingLocalGraphs + 5, 0);
}

}}

#endif
