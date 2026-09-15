#pragma once

namespace rise { namespace growlancer {

// S21 Data/Scripts/Character/CalcCharacter.lua::CalcAbilityGauge GL branch.
// Inputs are the *effective* Lua arguments, not guessed base/add mapping.
// Return the Lua numeric expression without choosing an SS6 MaxBP rounding
// or packet conversion; that bridge is a separate acceptance gate.
constexpr double CalcGrowLancerAbilityGauge(double strength,
    double dexterity, double vitality, double energy)
{
    return strength * .15 + dexterity * .2 + vitality * .3 + energy * 1.0;
}

} }
