#pragma once

#include <map>

// Server-side catalog overlay for the four Slayer ids.  The legacy 5.2
// RequireClass array is deliberately left untouched: class-column 9 still
// needs the owner's full class/persistence migration before these rows can be
// learned by ordinary characters.

namespace rise { namespace slayerserver {

enum SkillId
{
    kSwordInertia = 292,
    kBatFlock = 293,
    kPierceAttack = 294,
    kDetection = 295
};

enum EffectId
{
    kBatFlockEffect = 221
};

inline bool IsSlayerSkill(int id)
{
    return id >= kSwordInertia && id <= kDetection;
}

inline bool IsSlayerDamageSkill(int id)
{
    return id >= kSwordInertia && id <= kPierceAttack;
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
        { kDetection, "Detection", 0, 100, 100, 0, 5000, 1, 0, 350 }
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
