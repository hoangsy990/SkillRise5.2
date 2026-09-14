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
    kDemolish = 297
};

enum EffectId
{
    kBatFlockEffect = 221,
    kDemolishEffect = 222
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

inline bool IsSlayerDamageSkill(int id)
{
    return id == kSwordInertia || id == kBatFlock || id == kPierceAttack;
}

inline int DemolishDurationSeconds()
{
    // MasterSkillCalc_3rd.lua: SkillTime=60 for Slayer Demolish.
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
