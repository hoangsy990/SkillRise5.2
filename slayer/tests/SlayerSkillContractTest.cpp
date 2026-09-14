#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "../shared/SlayerSkillContractData.h"
#include "../shared/SlayerClassContractData.h"
#include "../server/SlayerPacketContract.h"
#include "../server/SlayerServerCatalog.h"
#include "../client/SlayerSkillRuntime.h"
#include "../client/SlayerSkillEffectBridge.h"
#include "../server/SlayerSkillDamageBridge.h"

namespace sl = rise::slayer;

static void Require(bool value, const char* message)
{
    if (!value)
    {
        std::cerr << "FAIL: " << message << std::endl;
        std::exit(1);
    }
}

static void RequireNear(double actual, double expected, const char* message)
{
    Require(std::fabs(actual - expected) < 0.000001, message);
}

class RecordingEffectSink : public sl::SlayerSkillEffectSink
{
public:
    int swordProjectiles;
    int batHits;
    int batDots;
    int batTicks;
    int pierceDashes;
    int pierceHits;
    int pierceReturns;
    int detections;
    int demolishBuffs;
    unsigned lastDotDuration;
    unsigned lastDemolishDuration;

    RecordingEffectSink()
        : swordProjectiles(0), batHits(0), batDots(0), batTicks(0),
          pierceDashes(0), pierceHits(0), pierceReturns(0), detections(0),
          demolishBuffs(0), lastDotDuration(0), lastDemolishDuration(0)
    {
    }

    virtual void SpawnSwordProjectile(int, int, int) { ++swordProjectiles; }
    virtual void SpawnBatFlockHit(int, int, int) { ++batHits; }
    virtual void StartBatFlockDot(int, int, unsigned durationMs)
    {
        ++batDots;
        lastDotDuration = durationMs;
    }
    virtual void TickBatFlockDot(int, int) { ++batTicks; }
    virtual void StartPierceDash(int, int) { ++pierceDashes; }
    virtual void SpawnPierceHit(int, int, int) { ++pierceHits; }
    virtual void FinishPierceReturn(int, int) { ++pierceReturns; }
    virtual void MarkDetection(int) { ++detections; }
    virtual void StartDemolishBuff(int, unsigned durationMs)
    {
        ++demolishBuffs;
        lastDemolishDuration = durationMs;
    }
};

class RecordingDamageSink : public sl::SlayerSkillDamageSink
{
public:
    int directHits;
    int dotTicks;
    double lastDamage;

    RecordingDamageSink() : directHits(0), dotTicks(0), lastDamage(0.0) {}

    virtual void ApplyDirectDamage(int, int, int, int, double damage)
    {
        ++directHits;
        lastDamage = damage;
    }
    virtual void ApplyBatFlockDotTick(int, int) { ++dotTicks; }
};

int main()
{
    Require(sl::SkillSeedCount() == 5, "five Slayer base skill rows");
    Require(sl::kSlayerClassProfile.classId == sl::kS21ClassSlayer &&
        sl::kSlayerClassProfile.createdByDefault &&
        sl::IsSlayerClass(sl::kS21ClassSlayer),
        "Slayer class profile is present");
    Require(sl::kSlayerClassProfile.startingStrength == 28 &&
        sl::kSlayerClassProfile.startingDexterity == 30 &&
        sl::kSlayerClassProfile.startingVitality == 15 &&
        sl::kSlayerClassProfile.startingEnergy == 10,
        "Slayer starting attributes match the guide");
    Require(sl::FindSkillSeed(292)->id == sl::kSwordInertia,
        "Sword Inertia uses native SkillList ID 292");
    Require(sl::FindSkillSeed(293)->id == sl::kBatFlock,
        "Bat Flock uses native SkillList ID 293");
    Require(sl::FindSkillSeed(294)->id == sl::kPierceAttack,
        "Pierce Attack uses native SkillList ID 294");
    Require(sl::FindSkillSeed(295)->id == sl::kDetection,
        "Detection uses native SkillList ID 295");
    Require(sl::FindSkillSeed(297)->id == sl::kDemolish,
        "Demolish uses native SkillList ID 297");
    Require(sl::FindSkillSeed(293)->skillListLevel == 150,
        "server profile preserves supplied SkillList Bat Flock level");
    Require(sl::FindSkillSeed(293)->guideLevel == 270,
        "Webzen guide Bat Flock level is kept separately");
    Require(sl::FindSkillSeed(295)->skillListLevel == 350,
        "server profile preserves supplied SkillList Detection level");
    Require(sl::FindSkillSeed(295)->guideLevel == 400,
        "Webzen guide Detection level is kept separately");
    Require(sl::FindRequirementOverride(295)->beadLevel == 300,
        "Webzen bead table Detection level is retained");
    Require(sl::RequiredBeadLevel(sl::kBatFlock) == 270 &&
        sl::RequiredGuideLevel(sl::kBatFlock) == 270,
        "Bat Flock bead/guide level profiles");
    Require(sl::MeetsStats(sl::kBatFlock, 150, 100, 380) &&
        !sl::MeetsBeadStats(sl::kBatFlock, 150, 100, 380),
        "Bat Flock config and bead gates remain explicit");
    Require(sl::MeetsStats(sl::kDetection, 350, 0, 800) &&
        !sl::MeetsGuideStats(sl::kDetection, 350, 0, 800) &&
        sl::MeetsBeadStats(sl::kDetection, 300, 0, 800),
        "Detection config, guide and bead gates remain explicit");
    Require(sl::FindRequirementOverride(294)->requiredSkillId == sl::kBatFlock,
        "Pierce Attack requires Bat Flock");
    Require(sl::FindRequirementOverride(294)->requiredMasteryPoints == 10,
        "Pierce Attack requires ten Bat Flock mastery points");
    Require(sl::FindSkillForScroll(12, 479) == sl::kPierceAttack,
        "known Pierce Attack bead mapping");
    Require(sl::FindSkillForScroll(12, 478) == -1,
        "unproven adjacent bead index is rejected");
    Require(sl::SwordInertiaProjectileCount() == 3,
        "Sword Inertia emits three swords");
    Require(sl::SwordInertiaMaxHitsPerTarget() == 1,
        "Sword Inertia target receives damage once");
    Require(sl::BatFlockHitCount() == 2, "Bat Flock has two direct hits");
    Require(sl::BatFlockDotDurationSeconds() == 5,
        "Bat Flock DOT duration follows SkillSettings.ini");
    Require(sl::PierceAttackHitCount(false) == 2,
        "Pierce Attack base hit count");
    Require(sl::PierceAttackHitCount(true) == 4,
        "Pierce Attack adds two hits to a Bat Flock target");
    Require(sl::DetectionCooldownMs() == 5000,
        "Detection cooldown is five seconds");
    Require(sl::DemolishCooldownMs() == 60000 &&
        sl::DemolishDurationSeconds() == 60,
        "Demolish cooldown and duration follow S21");
    Require(!sl::HasAuthoritativeDetectionDuration(),
        "Detection mark duration is not invented");
    Require(!sl::HasNativeClassSlot(7),
        "legacy 5.2 class capacity rejects Slayer slot");
    Require(sl::HasNativeClassSlot(10) && sl::HasNativeClassSlot(11),
        "Slayer class 9 fits only after ten class slots are allocated");
    Require(sl::HasReservedClassSlot(8),
        "reserved slot 7 remains distinct from Slayer class 9");
    Require(!sl::CanInstallClassColumn(10, false, true),
        "class column cannot install before persistence migration");
    Require(sl::CanInstallClassColumn(10, true, true),
        "class column installs only after both migrations");
    RequireNear(sl::ApplyRegularDamageFormula(sl::kSwordInertia, 100.0, 50, 100),
        129.8214285714286, "Sword Inertia formula");
    RequireNear(sl::ApplyRegularDamageFormula(sl::kBatFlock, 100.0, 100, 380),
        73.0357142857143, "Bat Flock formula");
    RequireNear(sl::ApplyRegularDamageFormula(sl::kPierceAttack, 100.0, 300, 1100),
        196.7857142857143, "Pierce Attack formula");

    const int skillIds[] = { sl::kSwordInertia, sl::kBatFlock,
        sl::kPierceAttack, sl::kDetection, sl::kDemolish };
    for (unsigned i = 0; i < sizeof(skillIds) / sizeof(skillIds[0]); ++i)
    {
        const int skillId = skillIds[i];
        Require(sl::FindServerSkillRecord(skillId) != 0,
            "server catalog contains every Slayer skill");
        Require(!sl::HasEnabledRise52Opcode(skillId),
            "no guessed 5.2 opcode is enabled");
    }
    Require(sl::FindPacketSeed(sl::kSwordInertia)->rise52EnvelopeOpcode == 0x19,
        "Sword Inertia uses the recovered target envelope");
    Require(sl::FindPacketSeed(sl::kBatFlock)->rise52EnvelopeOpcode == 0x19,
        "Bat Flock uses the recovered target envelope");
    Require(sl::FindPacketSeed(sl::kPierceAttack)->rise52EnvelopeOpcode == 0x19,
        "Pierce Attack uses the recovered targeted envelope");
    Require(sl::FindPacketSeed(sl::kDetection)->rise52EnvelopeOpcode == 0x19,
        "Detection uses the recovered self envelope");
    Require(sl::FindPacketSeed(sl::kDemolish)->rise52EnvelopeOpcode == 0x19,
        "Demolish uses the recovered self envelope");

    sl::SlayerSkillRuntime runtime;
    sl::CastContext context;
    context.actorId = 7;
    context.actorClassId = sl::kS21ClassSlayer;
    context.slayerClassEnabled = true;
    context.targetId = 99;
    context.targetAlive = true;
    context.level = 30;
    context.strength = 50;
    context.dexterity = 100;
    context.nowMs = 0;
    std::vector<sl::RuntimeEvent> events;
    Require(runtime.Cast(sl::kSwordInertia, context, events),
        "Sword Inertia cast accepted");
    Require(events.size() == 3, "Sword Inertia emits three projectile events");
    Require(events[0].type == sl::kSwordProjectileEvent &&
        events[1].ordinal == 1 && events[2].ordinal == 2,
        "Sword Inertia projectile ordering");
    Require(events[0].castId != 0 && events[1].castId == events[0].castId &&
        events[2].castId == events[0].castId,
        "Sword Inertia projectiles share one cast identity");
    sl::SwordInertiaHitLedger hitLedger;
    Require(hitLedger.TryAccept(events[0].castId, context.actorId, 200),
        "Sword Inertia accepts first contact");
    Require(!hitLedger.TryAccept(events[0].castId, context.actorId, 200),
        "Sword Inertia rejects duplicate target contact in one cast");
    Require(hitLedger.TryAccept(events[0].castId, context.actorId, 201),
        "Sword Inertia accepts a second target in one cast");
    Require(hitLedger.TryAccept(events[0].castId + 1, context.actorId, 200),
        "Sword Inertia accepts the target on a later cast");
    RecordingEffectSink sink;
    RecordingDamageSink damageSink;
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillEffect(events[i], sink),
            "Sword Inertia effect events dispatch");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillDamage(events[i], 100.0, 50, 100,
            damageSink), "Sword Inertia damage events dispatch");
    Require(sink.swordProjectiles == 3,
        "Sword Inertia bridge binds all three projectiles");
    Require(damageSink.directHits == 3 &&
        std::fabs(damageSink.lastDamage - 129.8214285714286) < 0.000001,
        "Sword Inertia bridge applies recovered formula");
    events.clear();

    context.level = 150;
    context.strength = 100;
    context.dexterity = 380;
    Require(runtime.Cast(sl::kBatFlock, context, events),
        "Bat Flock cast accepted");
    Require(events.size() == 3, "Bat Flock emits two hits and DOT application");
    Require(events[0].type == sl::kBatFlockHitEvent &&
        events[1].type == sl::kBatFlockHitEvent &&
        events[2].type == sl::kBatFlockDotAppliedEvent,
        "Bat Flock hit-before-DOT ordering");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillEffect(events[i], sink),
            "Bat Flock effect events dispatch");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillDamage(events[i], 100.0, 100, 380,
            damageSink), "Bat Flock damage events dispatch");
    Require(sink.batHits == 2 && sink.batDots == 1 &&
        sink.lastDotDuration == 5000,
        "Bat Flock bridge binds hits and five-second DOT");
    events.clear();
    runtime.Tick(context.actorId, 5000, events);
    Require(events.size() == 4, "Bat Flock emits four one-second DOT ticks");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillEffect(events[i], sink),
            "Bat Flock DOT events dispatch");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillDamage(events[i], 100.0, 100, 380,
            damageSink), "Bat Flock DOT damage events dispatch");
    Require(sink.batTicks == 4, "Bat Flock bridge binds all DOT ticks");
    Require(damageSink.dotTicks == 4, "Bat Flock damage bridge binds all DOT ticks");
    events.clear();

    context.level = 160;
    context.strength = 300;
    context.dexterity = 1100;
    context.hasBatFlock = true;
    context.targetHasBatFlock = true;
    context.batFlockMasteryPoints = 9;
    Require(!runtime.Cast(sl::kPierceAttack, context, events),
        "Pierce Attack rejects fewer than ten Bat Flock mastery points");
    context.batFlockMasteryPoints = 10;
    Require(runtime.Cast(sl::kPierceAttack, context, events),
        "Pierce Attack cast accepted with prerequisite");
    Require(events.size() == 6, "Pierce Attack emits dash, four hits and return");
    Require(events[0].type == sl::kPierceDashEvent &&
        events[1].type == sl::kPierceHitEvent &&
        events[4].type == sl::kPierceHitEvent &&
        events[5].type == sl::kPierceReturnEvent,
        "Pierce Attack dash-hit-return ordering");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillEffect(events[i], sink),
            "Pierce Attack effect events dispatch");
    for (std::size_t i = 0; i < events.size(); ++i)
        Require(sl::DispatchSlayerSkillDamage(events[i], 100.0, 300, 1100,
            damageSink), "Pierce Attack damage events dispatch");
    Require(sink.pierceDashes == 1 && sink.pierceHits == 4 &&
        sink.pierceReturns == 1,
        "Pierce Attack bridge binds dash, hits and return");
    Require(damageSink.directHits == 9 &&
        std::fabs(damageSink.lastDamage - 196.7857142857143) < 0.000001,
        "Pierce Attack bridge applies recovered formula");
    events.clear();
    context.targetHasBatFlock = false;
    Require(runtime.Cast(sl::kPierceAttack, context, events),
        "Pierce Attack remains castable without a target debuff");
    Require(events.size() == 4, "Pierce Attack emits two hits without target debuff");
    events.clear();

    context.level = 350;
    context.strength = 0;
    context.dexterity = 800;
    context.targetAlive = false;
    context.targetId = -1;
    context.nowMs = 0;
    Require(runtime.Cast(sl::kDetection, context, events),
        "Detection cast accepted without a selected target");
    Require(events.size() == 1 && events[0].type == sl::kDetectionMarkEvent,
        "Detection emits one minimap mark event");
    Require(sl::DispatchSlayerSkillEffect(events[0], sink),
        "Detection mark event dispatch");
    Require(sink.detections == 1, "Detection bridge binds minimap mark");
    events.clear();
    context.nowMs = 1000;
    Require(!runtime.Cast(sl::kDetection, context, events),
        "Detection cooldown rejects an early recast");
    context.nowMs = 5000;
    Require(runtime.Cast(sl::kDetection, context, events),
        "Detection recast accepted after five seconds");
    events.clear();

    context.level = 400;
    context.dexterity = 1450;
    context.targetAlive = false;
    context.targetId = -1;
    context.nowMs = 60000;
    Require(runtime.Cast(sl::kDemolish, context, events),
        "Demolish self-buff accepted");
    Require(events.size() == 1 && events[0].type == sl::kDemolishBuffEvent &&
        events[0].targetId == context.actorId &&
        events[0].durationMs == 60000,
        "Demolish emits a 60-second self-buff event");
    Require(sl::DispatchSlayerSkillEffect(events[0], sink),
        "Demolish buff event dispatch");
    Require(sink.demolishBuffs == 1 && sink.lastDemolishDuration == 60000,
        "Demolish effect bridge binds the recovered duration");
    Require(sl::DispatchSlayerSkillDamage(events[0], 100.0, 0, 1450,
        damageSink), "Demolish buff damage boundary dispatch");

    std::cout << "PASS: Slayer IDs, guide/config profiles, prerequisites, formulas, runtime event ordering and fail-closed class/packet gates" << std::endl;
    return 0;
}
