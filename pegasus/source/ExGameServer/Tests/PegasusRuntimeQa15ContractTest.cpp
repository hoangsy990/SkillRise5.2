#include "../../Shared/PegasusRuntimeQa15.h"

#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    static_assert(kRuntimeQaSkillCount == 15, "QA denominator drifted");
    static_assert(RuntimeQaSkillId(0) == 288, "first QA skill drifted");
    static_assert(RuntimeQaSkillId(13) == 243, "last item skill drifted");
    static_assert(RuntimeQaSkillId(14) == 737, "Wind Soul must be step 15");
    static_assert(RuntimeQaSkillIsSelfBuff(1), "Darkness must self-cast");
    static_assert(RuntimeQaSkillIsSelfBuff(3), "Dex Booster must self-cast");
    static_assert(!RuntimeQaSkillIsSelfBuff(14), "Wind Soul needs a target");
    static_assert(RuntimeQaCastCount(0) == 2,
        "Deathside QA must summon then attack with the persistent Reaper");
    static_assert(kRuntimeQaReactionCastCount == 10,
        "reaction sampling count drifted");
    static_assert(RuntimeQaCastCount(2) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(4) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(5) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(10) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(11) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(12) == kRuntimeQaReactionCastCount &&
        RuntimeQaCastCount(13) == kRuntimeQaReactionCastCount,
        "all seven target-reaction samples must repeat through native RNG");
    static_assert(RuntimeQaCastCount(1) == 1 && RuntimeQaCastCount(14) == 1,
        "buff and Wind Soul samples must remain single-cast");
    static_assert(kRuntimeQaRepeatDelayMs >= 1200 &&
        kRuntimeQaRepeatDelayMs < kRuntimeQaStepDelayMs,
        "repeat cast must follow framebuffer/action sampling inside its step");
    static_assert(kRuntimeQaBuffEffectCount == 6,
        "QA cleanup must cover every isolated Pegasus buff marker");
    static_assert(RuntimeQaRetainsBuffForStep(10, kSwordWrathNativeEffect),
        "Sword Wrath must survive into the Sword Blow sample");
    static_assert(!RuntimeQaRetainsBuffForStep(11, kSwordWrathNativeEffect),
        "Sword Wrath must be removed after the Sword Blow sample");
    static_assert(!RuntimeQaRetainsBuffForStep(10, kDarknessNativeEffect),
        "unrelated buffs must not contaminate Sword Blow");
    static_assert(kRuntimeQaInitialDelayMs >= 1000,
        "auto-spawned targets need a viewport preparation delay");

    for (std::size_t left = 0; left < kRuntimeQaSkillCount; ++left)
    {
        if (RuntimeQaSkillId(left) < 0 || RuntimeQaSkillName(left)[0] == '\0')
            return 1;
        for (std::size_t right = left + 1; right < kRuntimeQaSkillCount; ++right)
            if (RuntimeQaSkillId(left) == RuntimeQaSkillId(right))
                return 2;
    }
    for (std::size_t left = 0; left < kRuntimeQaBuffEffectCount; ++left)
    {
        if (!IsPegasusNativeBuffEffect(kRuntimeQaBuffEffects[left]))
            return 3;
        for (std::size_t right = left + 1; right < kRuntimeQaBuffEffectCount; ++right)
            if (kRuntimeQaBuffEffects[left] == kRuntimeQaBuffEffects[right])
                return 4;
    }
    std::puts("PASS: one-run QA sequence covers 14 Pegasus items + Wind Soul");
    return 0;
}
