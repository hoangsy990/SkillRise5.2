#pragma once

// Slayer is intentionally kept in an overlay until the legacy 5.2 class
// arrays, character encoding and persistence ABI are migrated together.
// This header is engine-independent and is the single source of truth for
// the five S21 skills being ported.

namespace rise { namespace slayer {

enum SkillId
{
    kSwordInertia = 292,
    kBatFlock = 293,
    kPierceAttack = 294,
    kDetection = 295,
    kDemolish = 297
};

enum ClassId
{
    // The 5.2 tree currently owns slots 0..6.  Slot 7 is the next reserved
    // native slot; S21's script/table class number remains 9.
    kRise52ReservedClassSlot = 7,
    kS21ClassSlayer = 9,
    // The legacy DB convention stores base class 9 as 9*16 and increments
    // the low nibble for the three class stages.  These values are evidence
    // for the future ABI adapter; they are not written into legacy packets.
    kS21SlayerDbClass = 144,
    kS21RoyalSlayerDbClass = 145,
    kS21MasterSlayerDbClass = 146
};

enum Element
{
    kNoElement = 0,
    kDarkness = 6
};

enum SkillFlags
{
    kRequiresTarget = 1 << 0,
    kSelfTarget = 1 << 1,
    kProjectile = 1 << 2,
    kDash = 1 << 3,
    kAppliesDamageOverTime = 1 << 4,
    kMarksMiniMap = 1 << 5
};

struct SkillSeed
{
    int id;
    const char* name;

    // Values from the supplied 5.2 SkillList.xml.  guideLevel is kept
    // separately because the public Webzen guide has two different level
    // values for Bat Flock and Detection.
    int skillListLevel;
    int guideLevel;
    int damage;
    int mana;
    int bp;
    int distance;
    int delayMs;
    int element;
    int type;
    int useType;
    int classRequirement;
    int icon;
    int buffIndex;
    int flags;
    int requiredStrength;
    int requiredDexterity;
};

static const SkillSeed kSkillSeeds[] =
{
    { kSwordInertia, "Sword Inertia", 30,  30,  10,   5,   0, 6, 0,    kDarkness, 0, 0, 1, 86, 0,
      kRequiresTarget | kProjectile, 50,  100 },
    { kBatFlock,     "Bat Flock",     150, 270,  90,  20,   5, 6, 0,    kDarkness, 0, 0, 2, 87, 0,
      kRequiresTarget | kProjectile | kAppliesDamageOverTime, 100, 380 },
    { kPierceAttack, "Pierce Attack", 160,  -1, 170,  30,  10, 6, 0,    kDarkness, 0, 0, 3, 88, 0,
      kRequiresTarget | kDash, 300, 1100 },
    { kDetection,    "Detection",     350, 400,   0, 100, 100, 0, 5000, kNoElement, 1, 0, 3, 89, 316,
      kSelfTarget | kMarksMiniMap, 0, 800 },
    { kDemolish,     "Demolish",      400, 400,   0,  50,   0, 0, 60000, kNoElement, 1, 0, 3, 90, 317,
      kSelfTarget, 0, 1450 }
};

struct RequirementOverride
{
    int skillId;
    int beadLevel;
    int guideLevel;
    int requiredSkillId;
    int requiredMasteryPoints;
};

// The guide's bead table says Detection level 300 and Pierce Attack requires
// Bat Flock + 10 mastery points.  The SkillList row says Detection level 350
// and Pierce Attack level 160; preserving both is deliberate.
static const RequirementOverride kRequirementOverrides[] =
{
    { kSwordInertia, 30,  30,  0,           0  },
    { kBatFlock,     270, 270, 0,           0  },
    { kPierceAttack, -1,  -1, kBatFlock,   10  },
    { kDetection,    300, 400, 0,           0  },
    { kDemolish,     400, 400, 0,           0  }
};

struct ScrollMapping
{
    int itemGroup;
    int itemIndex;
    int skillId;
    const char* note;
};

// Only the Pierce Attack bead mapping is present in the supplied
// SkillRequire.xml (ItemGroup 12 / ItemIndex 479).  The other three item
// indices must be read from the owner's production item catalog before they
// are enabled; no adjacent index is inferred.
static const ScrollMapping kKnownScrollMappings[] =
{
    { 12, 479, kPierceAttack, "requires Bat Flock and 10 Bat Flock mastery points" }
};

struct MasterySeed
{
    int id;
    const char* name;
    int baseSkillId;
    int parentId;
    int requiredMinPoint;
    int maxPoint;
};

static const MasterySeed kMasterySeeds[] =
{
    { 779, "Sword Inertia Strengthener", kSwordInertia, 0,             1, 20 },
    { 780, "Sword Inertia Mastery",      779,          779,          10, 10 },
    { 781, "Bat Flock Strengthener",     kBatFlock,     0,             1, 20 },
    { 782, "Bat Flock Mastery",          781,          781,          10, 10 },
    { 787, "Demolish Strengthener",      kDemolish,     0,             1, 20 },
    { 788, "Demolish Mastery",           787,          787,           1, 20 },
    { 794, "Detection Strengthener",     kDetection,    0,            10, 10 }
};

inline unsigned SkillSeedCount()
{
    return static_cast<unsigned>(sizeof(kSkillSeeds) / sizeof(kSkillSeeds[0]));
}

inline unsigned RequirementOverrideCount()
{
    return static_cast<unsigned>(sizeof(kRequirementOverrides) /
        sizeof(kRequirementOverrides[0]));
}

inline unsigned KnownScrollMappingCount()
{
    return static_cast<unsigned>(sizeof(kKnownScrollMappings) /
        sizeof(kKnownScrollMappings[0]));
}

inline unsigned MasterySeedCount()
{
    return static_cast<unsigned>(sizeof(kMasterySeeds) /
        sizeof(kMasterySeeds[0]));
}

inline const SkillSeed* FindSkillSeed(int id)
{
    for (unsigned i = 0; i < SkillSeedCount(); ++i)
        if (kSkillSeeds[i].id == id)
            return &kSkillSeeds[i];
    return 0;
}

inline const RequirementOverride* FindRequirementOverride(int id)
{
    for (unsigned i = 0; i < RequirementOverrideCount(); ++i)
        if (kRequirementOverrides[i].skillId == id)
            return &kRequirementOverrides[i];
    return 0;
}

inline const MasterySeed* FindMasterySeed(int id)
{
    for (unsigned i = 0; i < MasterySeedCount(); ++i)
        if (kMasterySeeds[i].id == id)
            return &kMasterySeeds[i];
    return 0;
}

inline int FindSkillForScroll(int itemGroup, int itemIndex)
{
    for (unsigned i = 0; i < KnownScrollMappingCount(); ++i)
        if (kKnownScrollMappings[i].itemGroup == itemGroup &&
            kKnownScrollMappings[i].itemIndex == itemIndex)
            return kKnownScrollMappings[i].skillId;
    return -1;
}

inline bool IsSlayerSkill(int id)
{
    return id == kSwordInertia || id == kBatFlock || id == kPierceAttack ||
        id == kDetection || id == kDemolish;
}

inline bool IsSlayerDbClass(int dbClass)
{
    return dbClass == kS21SlayerDbClass ||
        dbClass == kS21RoyalSlayerDbClass ||
        dbClass == kS21MasterSlayerDbClass;
}

inline int SlayerBaseClassFromDb(int dbClass)
{
    return IsSlayerDbClass(dbClass) ? kS21ClassSlayer : -1;
}

inline bool IsDarknessSkill(int id)
{
    const SkillSeed* seed = FindSkillSeed(id);
    return seed != 0 && seed->element == kDarkness;
}

inline bool MeetsStats(int id, int level, int strength, int dexterity)
{
    const SkillSeed* seed = FindSkillSeed(id);
    return seed != 0 && level >= seed->skillListLevel &&
        strength >= seed->requiredStrength &&
        dexterity >= seed->requiredDexterity;
}

inline int RequiredBeadLevel(int id)
{
    const SkillSeed* seed = FindSkillSeed(id);
    const RequirementOverride* overrideData = FindRequirementOverride(id);
    if (seed == 0)
        return -1;
    return overrideData != 0 ? overrideData->beadLevel : seed->skillListLevel;
}

inline int RequiredGuideLevel(int id)
{
    const SkillSeed* seed = FindSkillSeed(id);
    return seed == 0 ? -1 :
        (seed->guideLevel >= 0 ? seed->guideLevel : seed->skillListLevel);
}

inline bool MeetsBeadStats(int id, int level, int strength, int dexterity)
{
    const SkillSeed* seed = FindSkillSeed(id);
    const int beadLevel = RequiredBeadLevel(id);
    return seed != 0 && (beadLevel < 0 || level >= beadLevel) &&
        strength >= seed->requiredStrength &&
        dexterity >= seed->requiredDexterity;
}

inline bool MeetsGuideStats(int id, int level, int strength, int dexterity)
{
    const SkillSeed* seed = FindSkillSeed(id);
    const int guideLevel = RequiredGuideLevel(id);
    return seed != 0 && (guideLevel < 0 || level >= guideLevel) &&
        strength >= seed->requiredStrength &&
        dexterity >= seed->requiredDexterity;
}

inline int SwordInertiaProjectileCount()
{
    return 3;
}

inline int SwordInertiaMaxHitsPerTarget()
{
    return 1;
}

inline int BatFlockHitCount()
{
    return 2;
}

inline int BatFlockDotDurationSeconds()
{
    return 5;
}

inline int PierceAttackHitCount(bool targetHasBatFlock)
{
    return targetHasBatFlock ? 4 : 2;
}

inline int DetectionCooldownMs()
{
    return 5000;
}

inline int DemolishCooldownMs()
{
    return 60000;
}

inline int DemolishDurationSeconds()
{
    // MasterSkillCalc_3rd.lua sets SkillTime=60 for both Slayer Demolish
    // mastery levels.  This is independent of the 60000 ms recast delay.
    return 60;
}

// Exact formulas from RegularSkillCalc.lua.  The incoming damage is kept as
// double here; the native server performs its normal integer conversion at
// the final damage boundary.
inline double ApplyRegularDamageFormula(int id, double inDamage,
    int strength, int dexterity)
{
    const double multiplier = (strength / 8.0) + (dexterity / 28.0) + 120.0;
    switch (id)
    {
    case kSwordInertia:
        return inDamage * multiplier / 100.0;
    case kBatFlock:
        return inDamage * multiplier / 100.0 * 0.5;
    case kPierceAttack:
        return inDamage * multiplier / 100.0;
    default:
        return inDamage;
    }
}

// Detection's mark lifetime is not specified in the public guide or the
// supplied SkillList row.  Keep it unresolved instead of inventing a timer.
inline bool HasAuthoritativeDetectionDuration()
{
    return false;
}

// 5.2 has no class-9 array slot. Callers must reject activation until the
// class migration explicitly raises capacity and maps persistence/network
// class bytes; this prevents an out-of-bounds RequireClass write.
inline bool HasNativeClassSlot(int maxClass)
{
    return kS21ClassSlayer >= 0 && kS21ClassSlayer < maxClass;
}

inline bool HasReservedClassSlot(int maxClass)
{
    return kRise52ReservedClassSlot >= 0 &&
        kRise52ReservedClassSlot < maxClass;
}

} }
