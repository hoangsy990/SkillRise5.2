#include <cmath>
#include <cstdlib>
#include <iostream>
#include "../../Shared/GrowLancerSkillContractData.h"

namespace gl = rise::growlancer;

static void Require(bool value, const char* message)
{
    if (!value)
    {
        std::cerr << "FAIL: " << message << std::endl;
        std::exit(1);
    }
}

static void RequireNear(double actual, double expected, const char* message)
{
    Require(std::fabs(actual - expected) < 0.000001, message);
}

int main()
{
    Require(gl::SkillSeedCount() == 11, "eleven base/internal skill rows");
    Require(gl::ScrollMappingCount() == 7, "seven evidenced scroll mappings");
    Require(gl::ActiveMasterSkillSeedCount() == 13,
        "thirteen active third-master rows");
    Require(gl::FindActiveMasterSkillSeed(699)->brand == 277,
        "Shining Peak master brand remains base 277");
    Require(gl::FindActiveMasterSkillSeed(699)->treeParent == 698,
        "Shining Peak anomalous tree parent remains exact");
    Require(gl::FindActiveMasterSkillSeed(895)->buffEffect == 425,
        "active Wrath strengthener uses buff 425");
    Require(gl::FindActiveMasterSkillSeed(708) == 0,
        "legacy Wrath row is not in active third-master tree");
    Require(gl::ClientPacketSeedCount() == 9,
        "all nine base client packet families are classified");
    Require(gl::FindClientPacketSeed(271)->targetSource == gl::kPacketSelectedTarget,
        "Spin Step uses selected target at S21 client edge");
    Require(gl::FindClientPacketSeed(272)->targetSource == gl::kPacketLocalActor,
        "Circle Shield is self-targeted at S21 client edge");
    Require(gl::FindClientPacketSeed(277)->s21Family == gl::kPacketExtendedPositionC157,
        "Shining Peak uses extended positional S21 packet");
    Require(gl::FindClientPacketSeed(275)->s21Family == gl::kPacketClashMovementC159,
        "Clash uses movement S21 packet");
    for (int skillId = gl::kSpinStep; skillId <= gl::kBreche; ++skillId)
        Require(!gl::HasEnabledRise52PacketMapping(skillId),
            "RISE 5.2 packet translation remains disabled");
    Require(gl::IsGrowLancerBaseSkill(271), "first base skill classification");
    Require(gl::IsGrowLancerBaseSkill(279), "last base skill classification");
    Require(!gl::IsGrowLancerBaseSkill(280), "internal skill is not learnable base");
    Require(gl::IsGrowLancerInternalSkill(280), "Spin Step explosion classification");
    Require(gl::IsGrowLancerInternalSkill(281), "Magic Pin explosion classification");
    Require(!gl::HasProvenServerRuntimeHandler(271), "base dispatch remains fail closed");
    Require(!gl::HasProvenServerRuntimeHandler(281), "internal dispatch remains fail closed");
    Require(!gl::HasProvenServerRuntimeHandler(895), "active Wrath master remains fail closed");
    Require(!gl::HasProvenServerRuntimeHandler(708), "legacy Wrath chain remains fail closed");
    Require(!gl::HasProvenServerRuntimeHandler(710), "legacy Wrath tail remains fail closed");
    Require(gl::HasProvenServerRuntimeHandler(270), "unrelated SS6 skill is untouched");
    for (int skillId = 0; skillId < 2048; ++skillId)
        Require(gl::HasProvenServerRuntimeHandler(skillId) ==
            !(gl::IsGrowLancerSkill(skillId) ||
              gl::IsActiveGrowLancerMasterSkill(skillId) ||
              gl::IsLegacyGrowLancerMasterSkill(skillId)),
            "runtime gate rejects base/internal/active/legacy GL IDs only");
    Require(gl::FindSkillForScroll(12, 271) == 271, "Spin Step scroll");
    Require(gl::FindSkillForScroll(12, 272) == 273, "Obsidian scroll is non-identity mapping");
    Require(gl::FindSkillForScroll(12, 277) == 279, "Breche scroll");
    Require(gl::FindSkillForScroll(12, 278) == -1, "no inferred adjacent scroll");
    Require(!gl::MeetsLearningStats(274, 199, 200, 0), "Magic Pin strength gate");
    Require(gl::MeetsLearningStats(274, 200, 200, 0), "Magic Pin exact stat gate");
    RequireNear(gl::ApplyRegularDamageFormula(271, 0, 100.0, 0, 150, 0, 0), 123.2,
        "Spin Step primary formula");
    RequireNear(gl::ApplyRegularDamageFormula(271, 1, 100.0, 0, 150, 0, 0), 78.4,
        "Spin Step explosion formula");
    RequireNear(gl::ApplyRegularDamageFormula(276, 1, 100.0, 150, 0, 0, 0), 123.2,
        "Harsh Strike second formula");
    RequireNear(gl::ApplyRegularDamageFormula(274, 2, 80.0, 0, 200, 0, 0), 102.96,
        "Magic Pin third formula");
    RequireNear(gl::ApplyRegularDamageFormula(279, 0, 230.0, 300, 0, 0, 0), 146.05,
        "Breche formula");
    RequireNear(gl::ApplyRegularDamageFormula(277, 0, 50.0, 600, 0, 0, 0), 33.8666666666667,
        "Shining Peak divided three-part formula");
    std::cout << "PASS: Grow Lancer identities, scrolls, master rows, disabled packet mapping, stat gates and proven formulas" << std::endl;
    return 0;
}
