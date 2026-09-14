#pragma once

#include <array>
#include <cstddef>

#include "../shared/SlayerSkillContractData.h"

namespace rise { namespace slayer {

struct ServerSkillRecord
{
    int id;
    const char* name;
    int reqLevel;
    int damage;
    int mana;
    int bp;
    int distance;
    int delayMs;
    int type;
    int element;
    int classColumn;
    int icon;
    int buffIndex;
    int flags;
    int reqStrength;
    int reqDexterity;
};

static const ServerSkillRecord kServerSkillRecords[] =
{
    { kSwordInertia, "Sword Inertia", 30,  10,   5,   0, 6,    0, 0, kDarkness, kS21ClassSlayer, 86,   0,
      kRequiresTarget | kProjectile, 50,  100 },
    { kBatFlock,     "Bat Flock",     150,  90,  20,   5, 6,    0, 0, kDarkness, kS21ClassSlayer, 87,   0,
      kRequiresTarget | kProjectile | kAppliesDamageOverTime, 100, 380 },
    { kPierceAttack, "Pierce Attack", 160, 170,  30,  10, 6,    0, 0, kDarkness, kS21ClassSlayer, 88,   0,
      kRequiresTarget | kDash, 300, 1100 },
    { kDetection,    "Detection",     350,   0, 100, 100, 0, 5000, 1, kNoElement, kS21ClassSlayer, 89, 316,
      kSelfTarget | kMarksMiniMap, 0, 800 }
};

inline std::size_t ServerSkillRecordCount()
{
    return sizeof(kServerSkillRecords) / sizeof(kServerSkillRecords[0]);
}

inline const ServerSkillRecord* FindServerSkillRecord(int id)
{
    for (std::size_t i = 0; i < ServerSkillRecordCount(); ++i)
        if (kServerSkillRecords[i].id == id)
            return &kServerSkillRecords[i];
    return 0;
}

// SkillManager's legacy RequireClass array is MAX_CLASS == 7.  Refuse to
// write S21 class column 9 (or the reserved slot 7) until the owner migrates
// every class-sized server/client/DB array as one change.
inline bool CanInstallClassColumn(int maxClass, bool persistenceMigrated,
    bool protocolMigrated)
{
    return persistenceMigrated && protocolMigrated &&
        kS21ClassSlayer < maxClass;
}

inline bool IsServerSkillEnabled(int id, int maxClass,
    bool persistenceMigrated, bool protocolMigrated)
{
    return FindServerSkillRecord(id) != 0 &&
        CanInstallClassColumn(maxClass, persistenceMigrated, protocolMigrated);
}

} }
