#include "../../Shared/PegasusHighSkillCatalogData.h"
#include "../../Shared/PegasusHighSkillSoundCatalog.h"
#include <cstring>
#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    static_assert(kShiningBirdBaseDamageInstances == 1 &&
        kDragonViolentBaseDamageInstances == 2 &&
        kSpearstormBaseDamageInstances == 2 &&
        kSpearstormAdditionalDamageRate == 5,
        "Pegasus high-skill damage contract drifted");
    static_assert(HighSkillBuffDurationSeconds(kDexBoosterSkill) == 300 &&
        HighSkillBuffDurationSeconds(kSpiralChargeSkill) == 0 &&
        HighSkillBuffDurationSeconds(kCrusherChargeSkill) == 0 &&
        HighSkillBuffDurationSeconds(kElementalChargeSkill) == 0,
        "Pegasus high-skill buff lifetime contract drifted");
    static_assert(kHighSkillSeedCount == 14, "all Pegasus changelog group-15 items required");
    static_assert(kHighSkillSoundSeedCount == 23, "all recovered high-skill sounds required");
    static_assert(kHighSkillBuffEvidenceCount == 5, "all recovered self-buff contracts required");
    static_assert(FindHighSkillByItem(15, 100)->skillId == 288 &&
        FindHighSkillByItem(15, 101)->skillId == 289,
        "Deathside/Darkness prefix mappings changed");
    static_assert(FindHighSkillByItem(15, 110)->skillId == 1501,
        "owner-confirmed Sword Blow mapping changed");
    static_assert(FindHighSkillById(2024)->itemIndex == 103,
        "reverse lookup contract changed");
    static_assert(kSwordWrathNativeEffect == 168 &&
        kSwordWrathNativeEffect >= 0 && kSwordWrathNativeEffect < 210,
        "Sword Wrath must fit the native one-byte RISE effect domain");
    static_assert(NativeEffectForHighSkillBuff(289) == 173 &&
        NativeEffectForHighSkillBuff(2014) == 169 &&
        NativeEffectForHighSkillBuff(2015) == 170 &&
        NativeEffectForHighSkillBuff(2016) == 171 &&
        NativeEffectForHighSkillBuff(2024) == 172 &&
        NativeEffectForHighSkillBuff(1501) == -1,
        "native-safe self-buff effect mapping changed");
    static_assert(IsPegasusNativeBuffEffect(168) &&
        IsPegasusNativeBuffEffect(169) &&
        IsPegasusNativeBuffEffect(170) &&
        IsPegasusNativeBuffEffect(171) &&
        IsPegasusNativeBuffEffect(172) &&
        IsPegasusNativeBuffEffect(173) &&
        !IsPegasusNativeBuffEffect(167) &&
        !IsPegasusNativeBuffEffect(174),
        "native-safe Pegasus buff marker classification changed");

    const int expectedIds[] = {288,289,2023,2024,2012,2013,2014,2015,2016,1500,1501,241,242,243};
    const char* expectedNames[] = {
        "Deathside","Darkness","Raining Arrow","Dex Booster","Chaos Blade","Havoc Spear",
        "Spiral Charge","Crusher Charge","Elemental Charge","Sword Wrath",
        "Sword Blow","Shining Bird","Dragon Violent","Spearstorm"
    };
    for (std::size_t i = 0; i < kHighSkillSeedCount; ++i) {
        const HighSkillSeed& seed = kHighSkillSeeds[i];
        if (seed.itemSection != 15 || seed.itemIndex != 100 + static_cast<int>(i)) return 1;
        if (seed.skillId != expectedIds[i] || std::strcmp(seed.name, expectedNames[i]) != 0) return 2;
        if (FindHighSkillByItem(15, seed.itemIndex) != &seed || FindHighSkillById(seed.skillId) != &seed) return 3;
    }
    if (std::strcmp(kPegasusExecutableSha256,
        "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA") != 0) return 4;
    if (std::strcmp(kPegasusSkillXmlSha256,
        "4CD10CFA5BB310C2D89DE2305BEBB259A94BD3814FB64805669689A31657D032") != 0) return 5;
    if (std::strcmp(kPegasusBuffEffectXmlSha256,
        "1B12B27C6D56EAA721943AD0E15AAAD30F4D376B1F66B83947C17EA2E3BBBF76") != 0) return 9;

    const HighSkillSeed* swordBlow = FindHighSkillById(1501);
    if (!swordBlow || swordBlow->damage != 30 || swordBlow->mana != 19 ||
        swordBlow->abilityGauge != 17 || swordBlow->distance != 5 ||
        swordBlow->requiredClass[1] != 1 || !swordBlow->isDamage) return 6;
    for (std::size_t i = 0; i < kHighSkillSoundSeedCount; ++i) {
        if (!FindHighSkillById(kHighSkillSoundSeeds[i].skillId) ||
            std::strlen(kHighSkillSoundSeeds[i].sha256) != 64) return 7;
        for (std::size_t j = i + 1; j < kHighSkillSoundSeedCount; ++j)
            if (std::strcmp(kHighSkillSoundSeeds[i].relativePath,
                kHighSkillSoundSeeds[j].relativePath) == 0) return 8;
    }
    for (std::size_t i = 0; i < kHighSkillBuffEvidenceCount; ++i) {
        const HighSkillBuffEvidence& evidence = kHighSkillBuffEvidence[i];
        if (!FindHighSkillById(evidence.skillId) ||
            FindHighSkillBuffEvidence(evidence.skillId) != &evidence ||
            evidence.pegasusEffectId < 210) return 10;
    }
    std::puts("PASS: 14 mappings, 23 sounds and 5 self-buff provenance rows");
}
