#pragma once

namespace rise { namespace pegasus {

// Hash-pinned Pegasus Data/Local/xml/BuffEffect.xml, effect 213/group 128:
// "Increase +1 Range and +1 Damage Radius for Death Stab and Sword Blow."
// RISE carries that state through the collision-safe native marker 168; this
// header keeps the gameplay distances independent from either enum value.
static constexpr int kSwordWrathRangeBonus = 1;
static constexpr int kSwordWrathDamageRadiusBonus = 1;

static constexpr int SwordWrathEffectiveRange(int baseRange, bool active)
{
    return baseRange + (active ? kSwordWrathRangeBonus : 0);
}

static constexpr int SwordWrathEffectiveDamageRadius(int baseRadius, bool active)
{
    return baseRadius + (active ? kSwordWrathDamageRadiusBonus : 0);
}

static constexpr bool IsWithinPegasusDistance(int x, int y, int tx, int ty, int distance)
{
    return distance > 0 &&
        (((x - tx) * (x - tx)) + ((y - ty) * (y - ty))) <= (distance * distance);
}

}} // namespace rise::pegasus
