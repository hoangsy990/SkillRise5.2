#pragma once

#include "PegasusHighSkillCatalogData.h"
#include "PegasusSwordBlowProtocol.h"

#include <cstddef>
#include <cstdint>

namespace rise { namespace pegasus {

// The Pegasus item catalog contains fourteen rows (15:100..113). Wind Soul
// is a separate recovered skill and is deliberately the fifteenth/last QA
// step instead of being counted as another group-15 item.
constexpr int kRuntimeQaWindSoulSkill = 737;
constexpr std::size_t kRuntimeQaSkillCount = kHighSkillSeedCount + 1;
constexpr std::uint32_t kRuntimeQaInitialDelayMs = 1500;
constexpr std::uint32_t kRuntimeQaStepDelayMs = 4000;
// Framebuffer phases occur at roughly +250/+650/+1050 ms, and the caster
// action sampler reports at +900 ms. Keep the next native cast beyond both so
// the last image and action verdict belong to one cast rather than an overlap.
constexpr std::uint32_t kRuntimeQaRepeatDelayMs = 1250;
// Native RISE applies non-success monster shock through rand_fps_check(2).
// Ten ordinary casts leave the runtime gate a representative native sample
// without forcing the combat flag or patching the client's reaction branch.
constexpr unsigned kRuntimeQaReactionCastCount = 10;

static_assert(kHighSkillSeedCount == 14,
    "Pegasus group-15 QA catalog must contain item rows 100..113");
static_assert(kRuntimeQaSkillCount == 15,
    "Runtime QA must cover fourteen Pegasus items plus Wind Soul");

constexpr int RuntimeQaSkillId(std::size_t step)
{
    return step < kHighSkillSeedCount
        ? kHighSkillSeeds[step].skillId
        : step == kHighSkillSeedCount ? kRuntimeQaWindSoulSkill : -1;
}

constexpr const char* RuntimeQaSkillName(std::size_t step)
{
    return step < kHighSkillSeedCount
        ? kHighSkillSeeds[step].name
        : step == kHighSkillSeedCount ? "Wind Soul" : "invalid";
}

constexpr bool RuntimeQaSkillIsSelfBuff(std::size_t step)
{
    return step < kHighSkillSeedCount && kHighSkillSeeds[step].typeSkill == 1;
}

constexpr bool RuntimeQaSkillRequiresTargetReaction(int skill)
{
    return skill == kRainingArrowSkill ||
        skill == kChaosBladeSkill ||
        skill == kHavocSpearSkill ||
        skill == kSwordBlowSkill ||
        skill == kShiningBirdSkill ||
        skill == kDragonViolentSkill ||
        skill == kSpearstormSkill;
}

// Deathside's first cast summons its persistent Reaper; only a later cast
// selects attack action 3/4/8 and creates the matching scythe child. The
// one-run harness therefore needs two real casts for step 1.
constexpr unsigned RuntimeQaCastCount(std::size_t step)
{
    const int skill = RuntimeQaSkillId(step);
    return skill == kDeathsideSkill ? 2u :
        RuntimeQaSkillRequiresTargetReaction(skill) ?
        kRuntimeQaReactionCastCount : 1u;
}

// QA-only cleanup list. Persistent charge effects must not contaminate the
// following visual sample. Sword Wrath is the single deliberate exception:
// keep it for the immediately following Sword Blow sample so the enhanced
// range/radius path is exercised in the same one-run recording.
static constexpr int kRuntimeQaBuffEffects[] = {
    kSwordWrathNativeEffect,
    kSpiralChargeNativeEffect,
    kCrusherChargeNativeEffect,
    kElementalChargeNativeEffect,
    kDexBoosterNativeEffect,
    kDarknessNativeEffect,
};
constexpr std::size_t kRuntimeQaBuffEffectCount =
    sizeof(kRuntimeQaBuffEffects) / sizeof(kRuntimeQaBuffEffects[0]);

constexpr bool RuntimeQaRetainsBuffForStep(std::size_t step, int effect)
{
    return RuntimeQaSkillId(step) == 1501 &&
        effect == kSwordWrathNativeEffect;
}

}} // namespace rise::pegasus
