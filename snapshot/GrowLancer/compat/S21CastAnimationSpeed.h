#pragma once
namespace rise { namespace growlancer {
// Pinned source class7 profile constructor 0x9A23EC -> 0x997699.
// Default upper limit, NOT a default input speed or an SS6/DK global rule.
constexpr float S21GrowLancerInitialSpeedLimit = 273.0f;
// Pure tail of S21 0x1408580, AFTER optional manager/skill bonuses.
// These are source-domain inputs, not native object fields or buff IDs.
// A null source object means tornadoPresent must be false. Cap is optional;
// zero is a valid cap. Keep the source ordering: flags -> Tornado -> cap.
constexpr float S21AdjustCastSpeedTail(float speedAfterBonuses,
    unsigned char sourceFlags, bool tornadoPresent, bool hasCap, float cap)
{
    float speed = speedAfterBonuses;
    // S21 bit1 true jumps over the bit8 test at 14086DF. Both set add
    // only +20; this is a priority OR, not two independent bonuses.
    if ((sourceFlags & 0x09u) != 0)
        speed += 20.0f;
    if (tornadoPresent)
        speed *= 0.5f;
    if (hasCap && speed > cap)
        speed = cap;
    return speed;
}

// Input is AFTER the source caller's bonuses, status adjustment and cap.
// Not raw AttackSpeed; not an implicit choice of remote/local speed policy.
constexpr float S21FixedCastAnimationSpeed(unsigned short sourceAction)
{
    return sourceAction == 187 ? 0.5f :
        (sourceAction >= 191 && sourceAction <= 194 ? 0.35f : -1.0f);
}

constexpr float S21CastAnimationSpeed(unsigned short sourceAction, float adjustedSpeed)
{
    const float term = adjustedSpeed * 0.002f;
    switch (sourceAction)
    {
    case 185: return 0.13f + term;
    case 186: case 188: return 0.16f + term;
    case 187: return 0.5f;
    case 189: return 0.2f + term;
    case 190: return 0.3f + term;
    case 191: case 192: case 193: case 194: return 0.35f;
    default: return -1.0f;
    }
}
}}
