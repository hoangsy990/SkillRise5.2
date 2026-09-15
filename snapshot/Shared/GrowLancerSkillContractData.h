#pragma once

namespace rise { namespace growlancer {

enum SkillId
{
    kSpinStep = 271,
    kCircleShield = 272,
    kObsidian = 273,
    kMagicPin = 274,
    kClash = 275,
    kHarshStrike = 276,
    kShiningPeak = 277,
    kWrath = 278,
    kBreche = 279,
    kSpinStepExplosion = 280,
    kMagicPinExplosion = 281
};

struct SkillSeed
{
    int id;
    const char* name;
    int requiredLevel;
    int damage;
    int mana;
    int bp;
    int range;
    int delayMs;
    int type;
    int buffEffect;
    int requiredEnergy;
    int requiredStrength;
    int requiredDexterity;
};

struct ScrollMapping
{
    int itemGroup;
    int itemIndex;
    int skillId;
    const char* name;
};

struct MasterSkillSeed
{
    int id;
    const char* name;
    int brand;
    int treeParent;
    int requiredMinPoint;
    int maxPoint;
    int mana;
    int bp;
    int range;
    int delayMs;
    int buffEffect;
    bool hasPinnedLuaFormula;
};

enum ClientPacketFamily
{
    kPacketCommonTargetC135,
    kPacketExtendedPositionC157,
    kPacketClashMovementC159
};

enum ClientPacketTargetSource
{
    kPacketSelectedTarget,
    kPacketLocalActor,
    kPacketSelectedTargetAndPosition
};

struct ClientPacketSeed
{
    int skillId;
    ClientPacketFamily s21Family;
    ClientPacketTargetSource targetSource;
    int s21Opcode;
    int rise52Opcode;
};

static const SkillSeed kSkillSeeds[] =
{
    {271, "Spin Step",           83, 100,  12,  0, 2,   500, 0,   0,   0,   0, 150},
    {272, "Circle Shield",      220,   0, 100, 50, 0,     0, 0, 216,   0,   0,   0},
    {273, "Obsidian",            74,   0,  50, 50, 0,     0, 0, 217, 200,   0,   0},
    {274, "Magic Pin",           30,  80,   5,  3, 2,   500, 0,   0,   0, 200, 200},
    {275, "Clash",                0,  50,  50, 50, 6, 10000, 0,   0,   0,   0,   0},
    {276, "Harsh Strike",        74, 100,  12,  0, 3,   500, 0,   0,   0, 150,   0},
    {277, "Shining Peak",        92,  50,   8,  5, 4,     0, 0,   0,   0, 600,   0},
    {278, "Wrath",               66,   0,  40, 30, 0,     0, 0, 424,   0, 200, 200},
    {279, "Breche",             300, 230,  15,  6, 5,     0, 0,   0,   0, 300, 300},
    {280, "Explosion",            0,  50,   0,  0, 2,     0, 1,   0,   0,   0,   0},
    {281, "Magic Pin Explosion",  0,   0,   0,  0, 0,     0, 1,   0,   0,   0,   0}
};

static const ScrollMapping kScrollMappings[] =
{
    {12, 271, 271, "Spin Step Scroll"},
    {12, 272, 273, "Obsidian Scroll"},
    {12, 273, 274, "Magic Pin Scroll"},
    {12, 274, 276, "Harsh Strike Scroll"},
    {12, 275, 277, "Shining Peak Scroll"},
    {12, 276, 278, "Wrath Scroll"},
    {12, 277, 279, "Breche Scroll"}
};

// Active third-master rows are copied from the pinned S21 SkillList.xml and
// SkillTreeData_3rd.xml.  Brand and treeParent deliberately remain separate:
// Shining Peak 699 is branded to base 277 while its tree parent is 698.
static const MasterSkillSeed kActiveMasterSkillSeeds[] =
{
    {687, "Spin Step Strengthener",    271,   0,  1, 20,  14,  0, 2,   500,   0, true},
    {690, "Spin Step Mastery",         687, 687,  1, 20,  16,  0, 2,   500,   0, true},
    {688, "Harsh Strike Strengthener", 276,   0,  1, 20,  14,  0, 3,   500,   0, true},
    {691, "Harsh Strike Mastery",      688, 688, 10, 10,  16,  0, 3,   500,   0, true},
    {692, "Magic Pin Strengthener",    274,   0,  1, 20,  17, 10, 2,   500,   0, true},
    {695, "Magic Pin Mastery",         692, 692, 10, 10,  20, 13, 3,   500,   0, true},
    {693, "Obsidian Strengthener",     273,   0,  1, 20,  50, 50, 0,     0, 220, true},
    {696, "Breche Strengthener",       279,   0,  1, 20,  16, 10, 5,     0,   0, true},
    {698, "Breche Mastery",            696, 696, 10, 10,  16, 10, 6,     0,   0, true},
    {699, "Shining Peak Strengthener", 277, 698,  1, 20,  10,  7, 4,     0,   0, true},
    {703, "Circle Shield PowUp",       272,   0,  1, 20, 100, 50, 0,     0, 221, false},
    {706, "Circle Shield Mastery",     703, 703,  1, 20, 100, 50, 0,     0, 222, false},
    {895, "Wrath Strengthener",        278,   0,  1, 10,  50, 40, 0,     0, 425, false}
};

// S21 opcode evidence is descriptive only.  rise52Opcode stays -1 so no S21
// numeric opcode can be registered into the older protocol by accident.
static const ClientPacketSeed kClientPacketSeeds[] =
{
    {271, kPacketCommonTargetC135,     kPacketSelectedTarget,            0x35, -1},
    {272, kPacketCommonTargetC135,     kPacketLocalActor,                0x35, -1},
    {273, kPacketCommonTargetC135,     kPacketLocalActor,                0x35, -1},
    {274, kPacketCommonTargetC135,     kPacketSelectedTarget,            0x35, -1},
    {275, kPacketClashMovementC159,    kPacketSelectedTargetAndPosition, 0x59, -1},
    {276, kPacketCommonTargetC135,     kPacketSelectedTarget,            0x35, -1},
    {277, kPacketExtendedPositionC157, kPacketSelectedTargetAndPosition, 0x57, -1},
    {278, kPacketCommonTargetC135,     kPacketLocalActor,                0x35, -1},
    {279, kPacketExtendedPositionC157, kPacketSelectedTargetAndPosition, 0x57, -1}
};

inline unsigned SkillSeedCount()
{
    return static_cast<unsigned>(sizeof(kSkillSeeds) / sizeof(kSkillSeeds[0]));
}

inline unsigned ScrollMappingCount()
{
    return static_cast<unsigned>(sizeof(kScrollMappings) / sizeof(kScrollMappings[0]));
}

inline unsigned ActiveMasterSkillSeedCount()
{
    return static_cast<unsigned>(sizeof(kActiveMasterSkillSeeds) /
        sizeof(kActiveMasterSkillSeeds[0]));
}

inline const MasterSkillSeed* FindActiveMasterSkillSeed(int id)
{
    for (unsigned i = 0; i < ActiveMasterSkillSeedCount(); ++i)
        if (kActiveMasterSkillSeeds[i].id == id)
            return &kActiveMasterSkillSeeds[i];
    return 0;
}

inline unsigned ClientPacketSeedCount()
{
    return static_cast<unsigned>(sizeof(kClientPacketSeeds) /
        sizeof(kClientPacketSeeds[0]));
}

inline const ClientPacketSeed* FindClientPacketSeed(int skillId)
{
    for (unsigned i = 0; i < ClientPacketSeedCount(); ++i)
        if (kClientPacketSeeds[i].skillId == skillId)
            return &kClientPacketSeeds[i];
    return 0;
}

inline bool HasEnabledRise52PacketMapping(int skillId)
{
    const ClientPacketSeed* seed = FindClientPacketSeed(skillId);
    return seed != 0 && seed->rise52Opcode >= 0;
}

inline const SkillSeed* FindSkillSeed(int id)
{
    for (unsigned i = 0; i < SkillSeedCount(); ++i)
        if (kSkillSeeds[i].id == id)
            return &kSkillSeeds[i];
    return 0;
}

inline bool IsGrowLancerSkill(int id)
{
    return id >= kSpinStep && id <= kMagicPinExplosion;
}

inline bool IsGrowLancerBaseSkill(int id)
{
    return id >= kSpinStep && id <= kBreche;
}

inline bool IsGrowLancerInternalSkill(int id)
{
    return id == kSpinStepExplosion || id == kMagicPinExplosion;
}

inline bool IsActiveGrowLancerMasterSkill(int id)
{
    return FindActiveMasterSkillSeed(id) != 0;
}

// Catalog-only S21 Wrath chain, absent from the pinned active third tree.
// Identity is preserved for rejection, not silently activated as a variant.
inline bool IsLegacyGrowLancerMasterSkill(int id)
{
    return id == 708 || id == 709 || id == 710;
}

// The supplied S21 tables prove catalog values and formulas, but not the
// hardcoded target selection, barrage scheduling or recipient-class ABI.
// Runtime dispatch must stay closed until those contracts are recovered.
inline bool HasProvenServerRuntimeHandler(int id)
{
    return !IsGrowLancerSkill(id) && !IsActiveGrowLancerMasterSkill(id) &&
        !IsLegacyGrowLancerMasterSkill(id);
}

inline int FindSkillForScroll(int group, int index)
{
    for (unsigned i = 0; i < ScrollMappingCount(); ++i)
        if (kScrollMappings[i].itemGroup == group &&
            kScrollMappings[i].itemIndex == index)
            return kScrollMappings[i].skillId;
    return -1;
}

inline bool MeetsLearningStats(int id, int strength, int dexterity, int energy)
{
    const SkillSeed* seed = FindSkillSeed(id);
    return seed != 0 && strength >= seed->requiredStrength &&
        dexterity >= seed->requiredDexterity && energy >= seed->requiredEnergy;
}

struct ObsidianRegularFormulaResult
{
    double skillEffect;
    double skillTime;
};

// S21 RegularSkillCalc.lua::GrowLancerObsidian returns Strength/20 and 240.
// Recipient, refresh/stack behavior and native duration units still require
// authoritative handler evidence; this is only the Lua numeric output.
constexpr ObsidianRegularFormulaResult ApplyObsidianRegularFormula(double strength)
{
    return { strength / 20.0, 240.0 };
}

// Hash-pinned S21 Data\Skills\RegularSkillCalc.lua SHA-256
// F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847.
// Native hitIndex is zero-based: Spin returns strike/explosion, while Harsh
// and Magic Lua BarrageCount are 1/2 and 1/2/3. Invalid indices return Lua's
// zero/default rather than accidentally repeating the final damage branch.
// This numeric helper does not authorize targets, packet emission or rounding.
inline double ApplyRegularDamageFormula(int id, int hitIndex, double inDamage,
    int strength, int dexterity, double strengthTreeBonus,
    double dexterityTreeBonus)
{
    switch (id)
    {
    case kSpinStep:
        if (hitIndex < 0 || hitIndex > 1) return 0.0;
        return (inDamage * (hitIndex == 0 ? 1.1 : 0.7)) *
            (dexterity / 10.0 + 97.0 + dexterityTreeBonus) / 100.0;
    case kHarshStrike:
        if (hitIndex < 0 || hitIndex > 1) return 0.0;
        return (inDamage * (hitIndex == 0 ? 1.0 : 1.1)) *
            (strength / 10.0 + 97.0 + strengthTreeBonus) / 100.0;
    case kMagicPin:
    {
        if (hitIndex < 0 || hitIndex > 2) return 0.0;
        const double multiplier = hitIndex <= 0 ? 0.8 :
            (hitIndex == 1 ? 1.0 : 1.1);
        return (inDamage * multiplier) *
            (dexterity / 10.0 + 97.0 + dexterityTreeBonus) / 100.0;
    }
    case kBreche:
        return (inDamage * 0.5) *
            (strength / 10.0 + 97.0 + strengthTreeBonus) / 100.0;
    case kShiningPeak:
        return inDamage * (((strength / 10.0 + 97.0 + strengthTreeBonus) +
            (dexterity / 10.0 + 97.0 + dexterityTreeBonus)) * 0.8) /
            100.0 / 3.0;
    default:
        return inDamage;
    }
}

} }
