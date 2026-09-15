#pragma once

namespace rise { namespace growlancer {

// Exact S21 CalcCharacter.lua expression values before native OBJECT field
// conversion. Lua '/' produces fractions; the supplied protected GS does not
// expose whether it truncates each term or the sum. Keep that bridge explicit.
struct SourceDamage
{
    double physicalMinLeft;
    double physicalMinRight;
    double physicalMaxLeft;
    double physicalMaxRight;
    double magicMin;
    double magicMax;
};

constexpr SourceDamage ComputeSourceDamage(int strength, int dexterity, int energy)
{
    return {
        strength / 8.0 + dexterity / 10.0,
        strength / 8.0 + dexterity / 10.0,
        strength / 4.0 + dexterity / 6.0,
        strength / 4.0 + dexterity / 6.0,
        energy / 9.0,
        energy / 4.0
    };
}

} }
