#pragma once

#include <map>
#include "../SkillManager.h"

// Server-side catalog overlay for the five Slayer ids.  The legacy 5.2
// RequireClass array remains untouched; Slayer rows use an explicit persisted
// DB-class gate and a safe compatibility array slot until full class creation,
// body assets, and persistence migration are completed.

namespace rise { namespace slayerserver {

enum SkillId
{
    kSwordInertia = 292,
    kBatFlock = 293,
    kPierceAttack = 294,
    kDetection = 295,
    kDemolish = 297,
    // Master Slayer SkillTreeData_3rd.xml: 781 -> 782.
    kBatFlockStrengthener = 781,
    // SkillRequire.xml row 294: ReqMasterSkillIndex=782, ReqMasterPoint=10.
    kBatFlockMastery = 782
};

enum EffectId
{
    kBatFlockEffect = 221,
    kDemolishEffect = 222,
    // S21 BuffEffectManager row 316 (Detection) is a one-minute self result.
    // Keep the runtime slot private to the Slayer overlay; it is not written
    // into the legacy Effect.txt index space.
    kDetectionEffect = 223
};

// S21 class identity.  The legacy server still has MAX_CLASS == 7.  Keep the
// persisted DB class values explicit, while the compatibility adapter maps
// their internal array slot to the existing male sword class.  No legacy
// class array is ever indexed with 9.
static const int kS21ClassSlayer = 9;
static const int kS21SlayerDbClass = 144;
static const int kS21RoyalSlayerDbClass = 145;
static const int kS21MasterSlayerDbClass = 146;
static const int kSlayerLegacyArrayClass = 1; // CLASS_DK slot, 0..MAX_CLASS-1
static const unsigned char kSlayerClientClassByte = 0xE0;

inline bool IsSlayerClass(int classId)
{
	return classId == kS21ClassSlayer;
}

inline bool IsSlayerDbClass(int dbClass)
{
	return dbClass == kS21SlayerDbClass ||
		dbClass == kS21RoyalSlayerDbClass ||
		dbClass == kS21MasterSlayerDbClass;
}

inline int LegacyArrayClassForDbClass(int dbClass)
{
	return IsSlayerDbClass(dbClass) ? kSlayerLegacyArrayClass : dbClass / 16;
}

inline unsigned char ClientClassByteForDbClass(int dbClass)
{
	return IsSlayerDbClass(dbClass) ? kSlayerClientClassByte :
		static_cast<unsigned char>((dbClass % 16) * 16 - ((dbClass % 16) * 16 / 32) +
			(dbClass / 16) * 32);
}

inline bool IsSlayerSkill(int id)
{
    return id == kSwordInertia || id == kBatFlock || id == kPierceAttack ||
        id == kDetection || id == kDemolish;
}

inline bool IsSlayerBatMasterySkill(int id)
{
    return id == kBatFlockStrengthener || id == kBatFlockMastery;
}

inline bool IsS21SlayerExclusiveMasterSkill(int id)
{
    // Hash-pinned S21 SkillTreeData_3rd.xml class 512 owns 779..794.
    // S21 Rush 631 is class-512-only there, but the existing 5.2
    // MasterSkillTree.txt already has an unrelated 631 row. Do not gate
    // that collision by ID until the full per-class tree is imported.
    // Other class-512 nodes (e.g. 300/325/347) are shared too.
    return id >= 779 && id <= 794;
}

inline bool IsSlayerDamageSkill(int id)
{
    return id == kSwordInertia || id == kBatFlock ||
        IsSlayerBatMasterySkill(id) || id == kPierceAttack;
}

inline int ScaleSlayerDamage(int damage, int strength, int dexterity,
    bool batFlockHalfStrike)
{
    // RegularSkillCalc.lua evaluates STR/8 and DEX/28 as fractional Lua
    // numbers, then applies ((STR/8)+(DEX/28)+120)/100. In common-denominator
    // integer form that is (7*STR+2*DEX+6720)/5600. Truncate only after the
    // entire rate (and Bat Flock's 0.5 multiplier) has been applied.
    if (damage <= 0)
        return 0;
    const __int64 numerator = static_cast<__int64>(damage) *
        (7 * static_cast<__int64>(strength) +
         2 * static_cast<__int64>(dexterity) + 6720);
    const __int64 scaled = numerator / (batFlockHalfStrike ? 11200 : 5600);
    return static_cast<int>(scaled > 0x7fffffff ? 0x7fffffff :
        (scaled < 0 ? 0 : scaled));
}

inline int BatFlockDotDamage(int energy)
{
    // Data/Scripts/Skills/RegularSkillCalc.lua::BatFlockDotDamage receives
    // DOT from FormulaData.xml::Character formula 9 and returns DOT / 100
    // for both player and monster targets. That formula substitutes Energy
    // four times; it is independent of Bat Flock's initial half-strike.
    const double stat = energy > 0 ? static_cast<double>(energy) : 0.0;
    const double formula9 =
        ((((stat * stat) * 0.6 / 24.0) +
          ((stat - 800.0) * (stat - 500.0) / 200.0)) / 24.0) + 2000.0;
    const double result = formula9 / 100.0;
    return result >= 2147483647.0 ? 0x7fffffff :
        (result <= 0.0 ? 0 : static_cast<int>(result));
}

inline int DemolishDurationSeconds()
{
    // MasterSkillCalc_3rd.lua: SkillTime=60 for Slayer Demolish.
    return 60;
}

inline int DetectionDurationSeconds()
{
    // BuffEffectManager row 316: Detection result remains for one minute.
    return 60;
}

inline void ApplyServerCatalog(std::map<int, SKILL_INFO>& catalog)
{
    const struct Seed
    {
        int id;
        const char* name;
        int damage;
        int mana;
        int bp;
        int range;
        int delay;
        int type;
        int effect;
        int level;
    } seeds[] =
    {
        // S21 SkillList.xml has no server status effect for Sword Inertia;
        // keep Effect zero instead of reusing the Darkness element number.
        { kSwordInertia, "Sword Inertia", 10, 5, 0, 6, 0, 0, 0, 30 },
        { kBatFlock, "Bat Flock", 90, 20, 5, 6, 0, 0, kBatFlockEffect, 150 },
        // Hash-pinned S21 SkillList.xml rows 781/782. These are distinct
        // castable mastery IDs, not aliases guessed from the base effect.
        { kBatFlockStrengthener, "Bat Flock Strengthener", 22, 25, 9, 6, 0, 0, 0, 160 },
        { kBatFlockMastery, "Bat Flock Mastery", 23, 30, 12, 6, 0, 0, 0, 160 },
        // Pierce Attack is also a damage row with no BuffEffectManager slot.
        { kPierceAttack, "Pierce Attack", 170, 30, 10, 6, 0, 0, 0, 160 },
        { kDetection, "Detection", 0, 100, 100, 0, 5000, 1, 0, 350 },
        { kDemolish, "Demolish", 0, 50, 0, 0, 60000, 1, kDemolishEffect, 400 }
    };
    for (unsigned i = 0; i < sizeof(seeds) / sizeof(seeds[0]); ++i)
    {
        SKILL_INFO info = {};
        info.Index = seeds[i].id;
        strncpy_s(info.Name, sizeof(info.Name), seeds[i].name, _TRUNCATE);
        info.Damage = seeds[i].damage;
        info.Mana = seeds[i].mana;
        info.BP = seeds[i].bp;
        info.Range = seeds[i].range;
        info.Delay = seeds[i].delay;
        info.Type = seeds[i].type;
        info.Effect = seeds[i].effect;
        info.RequireLevel = seeds[i].level;
        // No legacy class slot is populated here.  This prevents an out of
        // bounds write; CheckSkillRequireClass/RunningSkill use the explicit
        // persisted Slayer DB-class gate above instead of this legacy array.
        catalog[info.Index] = info;
    }
}

} }
