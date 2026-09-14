#pragma once

#include <map>

// Server-side catalog overlay for the five Slayer ids.  The legacy 5.2
// RequireClass array is deliberately left untouched: class-column 9 still
// needs the owner's full class/persistence migration before these rows can be
// learned by ordinary characters.

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

// S21 class identity.  The legacy server still has MAX_CLASS == 7, so this
// value is used only as a guard; no legacy class array is indexed with it.
static const int kS21ClassSlayer = 9;

inline bool IsSlayerClass(int classId)
{
    return classId == kS21ClassSlayer;
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
        { kSwordInertia, "Sword Inertia", 10, 5, 0, 6, 0, 0, 6, 30 },
        { kBatFlock, "Bat Flock", 90, 20, 5, 6, 0, 0, kBatFlockEffect, 150 },
        { kPierceAttack, "Pierce Attack", 170, 30, 10, 6, 0, 0, 6, 160 },
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
        // bounds write and keeps activation blocked until class ABI work is
        // completed in the same change.
        catalog[info.Index] = info;
    }
}

} }
