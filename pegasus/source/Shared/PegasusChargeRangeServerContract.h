#pragma once

namespace rise { namespace pegasus {

// Hash-pinned Pegasus Skill.xml and BuffEffect.xml prove these absolute cast
// ranges. They do not provide the attack/defense/speed formulas for the same
// stance effects, so this contract intentionally covers range only.
static constexpr int kSpiralChaosBladeRange = 6;
static constexpr int kCrusherChaosBladeRange = 5;
static constexpr int kElementalHavocSpearRange = 7;

enum class PegasusChargeWeaponFamily
{
    None,
    Sword,
    Staff,
    Other,
};

// Pegasus BuffEffect.xml identifies the three charge stances by weapon type;
// Webzen's Season 16 MG renewal guide independently confirms the same split.
// Keep this as a pure contract so the server-side equipment adapter can be
// tested without copying a newer server's item layout.
static constexpr bool ChargeBuffWeaponAllowed(int skillId,
    PegasusChargeWeaponFamily family, bool twoHanded)
{
    return skillId == 2014 ? (family == PegasusChargeWeaponFamily::Sword && twoHanded) :
        skillId == 2015 ? (family == PegasusChargeWeaponFamily::Sword && !twoHanded) :
        skillId == 2016 ? (family == PegasusChargeWeaponFamily::Staff) : true;
}

static constexpr int ChargeEffectiveCastRange(int skillId, int baseRange,
    bool spiralActive, bool crusherActive, bool elementalActive)
{
    if (skillId == 2012) // Chaos Blade
    {
        // A valid MG stance has only one charge marker. max-range precedence
        // keeps a corrupt dual-marker state deterministic without weakening
        // either XML-proven range.
        if (spiralActive)
            return kSpiralChaosBladeRange;
        if (crusherActive)
            return kCrusherChaosBladeRange;
    }
    if (skillId == 2013 && elementalActive) // Havoc Spear
        return kElementalHavocSpearRange;
    return baseRange;
}

}} // namespace rise::pegasus
