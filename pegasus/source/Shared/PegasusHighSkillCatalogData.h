#pragma once

#include <cstddef>

namespace rise { namespace pegasus {

static constexpr int kDeathsideSkill = 288;
static constexpr int kDarknessSkill = 289;
static constexpr int kRainingArrowSkill = 2023;
static constexpr int kDexBoosterSkill = 2024;
static constexpr int kChaosBladeSkill = 2012;
static constexpr int kHavocSpearSkill = 2013;
static constexpr int kSpiralChargeSkill = 2014;
static constexpr int kCrusherChargeSkill = 2015;
static constexpr int kElementalChargeSkill = 2016;
static constexpr int kSwordWrathSkill = 1500;
static constexpr int kShiningBirdSkill = 241;
static constexpr int kDragonViolentSkill = 242;
static constexpr int kSpearstormSkill = 243;
static constexpr int kShiningBirdBaseDamageInstances = 1;
static constexpr int kDragonViolentBaseDamageInstances = 2;
static constexpr int kSpearstormBaseDamageInstances = 2;
static constexpr int kSpearstormAdditionalDamageRate = 5;

// Provenance is deliberately part of the compiled/tested contract.  The item
// mapping comes from the bounded 20-byte Pegasus .rdata table; skill fields
// come from the hash-pinned Pegasus Skill.xml.  Neither source is modified.
static constexpr const char* kPegasusExecutableSha256 =
    "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA";
static constexpr const char* kPegasusSkillXmlSha256 =
    "4CD10CFA5BB310C2D89DE2305BEBB259A94BD3814FB64805669689A31657D032";
static constexpr const char* kPegasusBuffEffectXmlSha256 =
    "1B12B27C6D56EAA721943AD0E15AAAD30F4D376B1F66B83947C17EA2E3BBBF76";

struct HighSkillBuffEvidence {
    int skillId;
    int pegasusEffectId;
    int pegasusGroup;
    const char* name;
    const char* description;
};

static constexpr HighSkillBuffEvidence kHighSkillBuffEvidence[] = {
    {289,228,132,"Darkness","Increases Curse damage and Defense"},
    {2014,214,129,"Spiral Charge","Two-handed MG; attack/defense; Chaos Blade range 6"},
    {2015,215,130,"Crusher Charge","One-handed MG; attack speed/defense; Chaos Blade range 5"},
    {2016,227,131,"Elemental Charge","Wizard MG; Havoc Spear range 7; third cast Nova"},
    {2024,229,133,"Dex Booster","Attack speed and attack success rate"},
};
static constexpr std::size_t kHighSkillBuffEvidenceCount =
    sizeof(kHighSkillBuffEvidence) / sizeof(kHighSkillBuffEvidence[0]);

// 168 is unassigned in the native RISE client/server effect enums and remains
// below the one-byte native effect limit. Pegasus used numeric 213 in a newer
// enum; reusing that number would be out of range in RISE 5.2.
static constexpr int kSwordWrathNativeEffect = 168;
static constexpr int kSwordWrathDefaultDurationSeconds = 60;
static constexpr int kSpiralChargeNativeEffect = 169;
static constexpr int kCrusherChargeNativeEffect = 170;
static constexpr int kElementalChargeNativeEffect = 171;
static constexpr int kDexBoosterNativeEffect = 172;
static constexpr int kDarknessNativeEffect = 173;
static constexpr int kDexBoosterDurationSeconds = 300;
// Count zero is the native RISE persistent-effect sentinel: MainProc neither
// decrements nor expires it. The three MG charge buffs end on death/logout.
static constexpr int kPersistentChargeEffectCount = 0;

static constexpr bool IsPegasusNativeBuffEffect(int effect)
{
    return effect == kSwordWrathNativeEffect ||
        effect == kSpiralChargeNativeEffect ||
        effect == kCrusherChargeNativeEffect ||
        effect == kElementalChargeNativeEffect ||
        effect == kDexBoosterNativeEffect ||
        effect == kDarknessNativeEffect;
}

static constexpr int HighSkillBuffDurationSeconds(int skillId)
{
    return skillId == kDexBoosterSkill ? kDexBoosterDurationSeconds :
        kPersistentChargeEffectCount;
}

static constexpr int NativeEffectForHighSkillBuff(int skillId)
{
    return skillId == kDarknessSkill ? kDarknessNativeEffect :
        skillId == kSpiralChargeSkill ? kSpiralChargeNativeEffect :
        skillId == kCrusherChargeSkill ? kCrusherChargeNativeEffect :
        skillId == kElementalChargeSkill ? kElementalChargeNativeEffect :
        skillId == kDexBoosterSkill ? kDexBoosterNativeEffect : -1;
}

struct HighSkillSeed {
    int itemSection;
    int itemIndex;
    int skillId;
    const char* name;
    int level;
    int damage;
    int mana;
    int abilityGauge;
    int distance;
    int delay;
    int strength;
    int dexterity;
    int energy;
    int leadership;
    unsigned char requiredClass[7];
    unsigned char typeSkill;
    unsigned char isDamage;
    unsigned short icon;
};

static constexpr HighSkillSeed kHighSkillSeeds[] = {
    {15,100,288,"Deathside",0,75,120,20,6,0,0,0,930,0,{0,0,0,0,0,1,0},0,1,0},
    {15,101,289,"Darkness",0,0,100,50,5,5000,0,0,300,0,{0,0,0,0,0,1,0},1,0,0},
    {15,102,2023,"Raining Arrow",160,65,15,7,7,0,0,0,0,0,{0,0,3,0,0,0,0},0,1,0},
    {15,103,2024,"Dex Booster",0,0,30,0,0,0,0,0,0,0,{0,0,3,0,0,0,0},1,0,229},
    {15,104,2012,"Chaos Blade",160,350,19,14,3,0,0,0,0,0,{0,0,0,3,0,0,0},0,1,0},
    {15,105,2013,"Havoc Spear",160,150,92,20,6,0,0,0,1073,0,{0,0,0,3,0,0,0},0,1,0},
    {15,106,2014,"Spiral Charge",120,0,21,17,0,0,0,0,0,0,{0,0,0,3,0,0,0},1,0,214},
    {15,107,2015,"Crusher Charge",120,0,21,17,0,0,0,0,0,0,{0,0,0,3,0,0,0},1,0,366},
    {15,108,2016,"Elemental Charge",120,0,65,20,0,0,0,0,0,0,{0,0,0,3,0,0,0},1,0,367},
    {15,109,1500,"Sword Wrath",0,0,21,17,0,0,0,0,0,0,{0,1,0,0,0,0,0},1,0,213},
    {15,110,1501,"Sword Blow",0,30,19,17,5,0,0,0,0,0,{0,1,0,0,0,0,0},0,1,0},
    {15,111,241,"Shining Bird",80,120,45,5,6,0,0,0,500,0,{1,0,0,0,0,0,0},0,1,0},
    {15,112,242,"Dragon Violent",50,140,30,5,6,500,0,0,680,0,{0,0,0,0,0,0,1},0,1,0},
    {15,113,243,"Spearstorm",160,160,105,5,6,300,0,0,1160,0,{0,0,0,0,1,0,0},0,1,0},
};

static constexpr std::size_t kHighSkillSeedCount =
    sizeof(kHighSkillSeeds) / sizeof(kHighSkillSeeds[0]);

static constexpr const HighSkillSeed* FindHighSkillByItem(int section, int index)
{
    for (const HighSkillSeed& seed : kHighSkillSeeds)
        if (seed.itemSection == section && seed.itemIndex == index) return &seed;
    return nullptr;
}

static constexpr const HighSkillSeed* FindHighSkillById(int skillId)
{
    for (const HighSkillSeed& seed : kHighSkillSeeds)
        if (seed.skillId == skillId) return &seed;
    return nullptr;
}

static constexpr const HighSkillBuffEvidence* FindHighSkillBuffEvidence(int skillId)
{
    for (const HighSkillBuffEvidence& evidence : kHighSkillBuffEvidence)
        if (evidence.skillId == skillId) return &evidence;
    return nullptr;
}

}}
