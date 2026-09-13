#pragma once

namespace rise { namespace pegasus {
// Exact default/fallback action numbers recovered at bounded caster handlers.
// Pegasus has model-specific variants for some skills; those stay open until
// their model predicates are translated to native RISE identifiers.
constexpr int kPegasusRainingArrowDefaultAction = 286;
constexpr int kPegasusPlayerActionCount = 287;
constexpr int kPegasusDexBoosterDefaultAction = 67;
constexpr int kPegasusChaosBladeDefaultAction = 183;
// 0x654B40..0x654B56 prefers action 285 when the model contains it.
constexpr int kPegasusChaosBladeExtendedAction = 285;
constexpr int kPegasusHavocSpearDefaultAction = 183;
// 0x666CF5..0x666D0B prefers action 284 when present in the model.
constexpr int kPegasusHavocSpearExtendedAction = 284;
constexpr int kPegasusSpiralChargeDefaultAction = 67;
constexpr int kPegasusCrusherChargeDefaultAction = 67;
constexpr int kPegasusElementalChargeDefaultAction = 67;
constexpr int kPegasusDragonViolentDefaultAction = 253;
constexpr int kPegasusSpearstormDefaultAction = 80;
// Pegasus 0x65B148 input/cast handler selects action 0x91 for Darkness 0x121.
// RISE 5.2 may carry an older player BMD, so runtime dispatch verifies the
// action count and falls back to its native self-buff pose when unavailable.
constexpr int kPegasusDarknessAction = 145;

// Proven semantic mapping: Pegasus action 67 is the common self-buff pose.
// Other numeric actions are evidence only until their native names are proven.
constexpr int kNativePegasusBuffAction = PLAYER_SKILL_VITALITY;
// 676F46..676F80: only Spiral has this mount-aware branch; Crusher and
// Elemental explicitly select67. Use native identifiers, never source layouts.
constexpr int SpiralChargeCasterAction(int helperType, bool safeZone)
{
    return safeZone ? PLAYER_SKILL_VITALITY :
        helperType == MODEL_HELPER + 2 ? PLAYER_SKILL_SLEEP_UNI :
        helperType == MODEL_HELPER + 3 ? PLAYER_SKILL_SLEEP_DINO :
        helperType == MODEL_HELPER + 37 ? PLAYER_SKILL_SLEEP_FENRIR :
        PLAYER_SKILL_VITALITY;
}
static_assert(PLAYER_SKILL_SLEEP_UNI == 157 && PLAYER_SKILL_SLEEP_DINO == 158 &&
    PLAYER_SKILL_SLEEP_FENRIR == 159, "Spiral mounted action mapping drift");
static_assert(SpiralChargeCasterAction(MODEL_HELPER + 2, false) == 157 &&
    SpiralChargeCasterAction(MODEL_HELPER + 3, false) == 158 &&
    SpiralChargeCasterAction(MODEL_HELPER + 37, false) == 159 &&
    SpiralChargeCasterAction(MODEL_HELPER + 4, false) == 67 &&
    SpiralChargeCasterAction(-1, false) == 67 &&
    SpiralChargeCasterAction(MODEL_HELPER + 2, true) == 67 &&
    SpiralChargeCasterAction(MODEL_HELPER + 3, true) == 67 &&
    SpiralChargeCasterAction(MODEL_HELPER + 37, true) == 67,
    "Spiral mount/safe-zone branch drift");
constexpr int kNativeChaosBladeAction = PLAYER_SKILL_GIGANTICSTORM;
constexpr int kNativeHavocSpearAction = PLAYER_SKILL_GIGANTICSTORM;
constexpr int kNativeDragonViolentAction = PLAYER_SKILL_DRAGONLORE;
constexpr int kNativeSpearstormAction = PLAYER_ATTACK_STRIKE;
}}
