#include "../Main5.2_RISE/RISE/PegasusSwordBlowMath.h"
#include "../../Shared/PegasusSwordBlowProtocol.h"
#include <cmath>
#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    constexpr SwordBlowLight tail = SwordBlowSmallSwordTailLight(0.2f);
    static_assert(tail.red == 0.2f * 0.394f && tail.green == 0.2f * 0.476f,
        "Tail material must use background-subtracted candidate, not raw screenshot RGB");
    static_assert(tail.red < tail.green && tail.green < tail.blue &&
        tail.blue == 0.2f, "Tail material must be blue-violet without increasing blue energy");
    constexpr SwordBlowLight darkTail = SwordBlowSmallSwordTailLight(0.0f);
    static_assert(darkTail.red == 0.0f && darkTail.green == 0.0f && darkTail.blue == 0.0f,
        "Tail transfer must not add light to a zero-intensity material");
    static_assert(kSwordBlowModelPlayDivisor == 6.0f,
        "0x67962A Sword Blow BMD play divisor drift");
    // Guarded SS21 object/visual trace provenance. These exact constants keep
    // later ports from reproducing the V1-style mistake of guessing a visible
    // tint from screenshots instead of separating object light from material.
    static_assert(kSwordBlowNeutralModelLight.red == 0.2f &&
        kSwordBlowNeutralModelLight.green == 0.2f &&
        kSwordBlowNeutralModelLight.blue == 0.2f,
        "SS21 root/blade/plane object light drifted");
    static_assert(kSwordBlowSmallSwordBrightPassTransfer == 0.65f,
        "Original Sword Blow material transfer must stay unchanged");
    static_assert(kSwordBlowFlySmallSwordBrightPassTransfer == 0.32f,
        "Sword Blow Fly material transfer drifted");
    static_assert(kSwordBlowFlySmallSwordImageLight.red == 143.0f / 255.0f &&
        kSwordBlowFlySmallSwordImageLight.green == 155.0f / 255.0f &&
        kSwordBlowFlySmallSwordImageLight.blue == 233.0f / 255.0f,
        "Sword Blow Fly owner-frame sword-tail sample drifted");
    static_assert(kSwordBlowSkill == 1501 && kSwordBlowFlySkill == 1502 &&
        kSwordBlowSkill != kSwordBlowFlySkill,
        "Sword Blow Fly must not replace original Sword Blow");
    static_assert(kSwordBlowPlanLight.red == 0.7f &&
        kSwordBlowPlanLight.green == 0.7f &&
        kSwordBlowPlanLight.blue == 1.0f,
        "SS21 Plan/glint light drifted");
    static_assert(kSwordBlowMultishotLight.red == 0.4f &&
        kSwordBlowMultishotLight.green == 0.5f &&
        kSwordBlowMultishotLight.blue == 1.2f,
        "SS21 multishot 1/2 light drifted");
    static_assert(kSwordBlowMultishot3Light.red == 0.35f &&
        kSwordBlowMultishot3Light.green == 0.4f &&
        kSwordBlowMultishot3Light.blue == 0.48f,
        "SS21 multishot 3 light drifted");
    static_assert(kSwordBlowLightningLight.red == 0.3f &&
        kSwordBlowLightningLight.green == 0.6f &&
        kSwordBlowLightningLight.blue == 1.0f,
        "SS21 target lightning light drifted");
    static_assert(kSwordBlowFlareLight.red == 0.65f &&
        kSwordBlowFlareLight.green == 0.7f &&
        kSwordBlowFlareLight.blue == 0.9f,
        "SS21 target flare light drifted");
    static_assert(kSwordBlowFlightDurationMs == 1500,
        "Sword Blow flight choreography duration drifted");
    static_assert(kSwordBlowWingModelBone == 45 &&
        kSwordBlowWingRootX == 2.85384768f &&
        kSwordBlowWingRootY == 8.55563068f &&
        kSwordBlowWingRootZ == -24.29822159f,
        "BloodCastle01 mesh-3 skinned root drifted");
    static_assert(kSwordBlowFlyThrustSpeedMultiplier == 1.35f &&
        kSwordBlowFlyThrustStartFrame == 4.0f,
        "Sword Blow Fly repeated-thrust timing drifted");
    if (std::fabs(SwordBlowFlightPlaySpeed(2.5f, 7, 14) -
        7.3125f) > 0.0001f ||
        SwordBlowFlightPlaySpeed(2.5f, 1, 14) != 2.5f)
        return 17;
    if (SwordBlowFlightHeightAt(0.0f) != 0.0f ||
        SwordBlowFlightHeightAt(1.0f) != 0.0f ||
        SwordBlowFlightHeightAt(kSwordBlowFlightCrouchEnd) != 0.0f ||
        SwordBlowFlightHeightAt(kSwordBlowFlightTakeoffEnd) < 149.9f ||
        SwordBlowFlightHeightAt(0.45f) != kSwordBlowFlightHeight ||
        SwordBlowFlightHeightAt(0.70f) >= kSwordBlowFlightHeight ||
        SwordBlowFlightWingAlphaAt(0.0f) != 0.0f ||
        SwordBlowFlightWingAlphaAt(1.0f) != 0.0f ||
        std::fabs(SwordBlowFlightWingAlphaAt(0.5f) -
            kSwordBlowFlightWingOpacity) > 0.000001f)
        return 16;
    // Captured arithmetic expectations, including both out-of-life ends.
    const float samples[][2] = {
        {-1, 0}, {0, 0}, {2, 0.0625f}, {4, 0.25f}, {8, 1},
        {12, 0.25f}, {14, 0.0625f}, {16, 0}, {17, 0}
    };
    for (const auto& sample : samples)
        if (std::fabs(SwordBlowOpacity(sample[0], 16) - sample[1]) > 0.000001f)
            return 1;
    if (SwordBlowOpacity(0.5f, 0) != 1.0f) return 2;
    for (int i = 0; i <= 160; ++i) {
        const float t = i * 0.1f;
        const float a = SwordBlowOpacity(t, 16);
        if (a < 0 || a > 1 || std::fabs(a - SwordBlowOpacity(16-t,16)) > 0.000001f)
            return 3;
    }
    const auto buffed = GetSwordBlowSpread(true);
    const auto plain = GetSwordBlowSpread(false);
    if (buffed.childCount != 24 || buffed.lateralExtent != 300 ||
        plain.childCount != 32 || plain.lateralExtent != 500) return 4;
    if (kSwordBlowChildStartForwardMin != 600.0f ||
        kSwordBlowChildStartForwardMax != 1000.0f ||
        kSwordBlowChildStartHeightMin != -100.0f ||
        kSwordBlowChildStartHeightMax != 250.0f ||
        kSwordBlowChildEndForward != -1500.0f ||
        kSwordBlowChildEndHeight != 0.0f) return 10;
    const float pitchLow = SwordBlowChildPitchDegrees(-100.0f, 0.0f);
    const float pitchFlat = SwordBlowChildPitchDegrees(0.0f, 0.0f);
    const float pitchHigh = SwordBlowChildPitchDegrees(250.0f, 0.0f);
    if (!(pitchLow < 0.0f) || pitchFlat != 0.0f || !(pitchHigh > 0.0f) ||
        std::fabs(SwordBlowChildPitchDegrees(1500.0f, 0.0f) - 90.0f) > 0.0001f ||
        std::fabs(SwordBlowChildPitchDegrees(-1500.0f, 0.0f) + 90.0f) > 0.0001f)
        return 13;

    std::uint8_t word[2] = {};
    WriteWordLE(word, kSwordBlowSkill);
    if (word[0] != 0xDD || word[1] != 0x05 ||
        ReadWordLE(word) != kSwordBlowSkill) return 5;
    if (SwordBlowPayloadSize(0) != 6 || SwordBlowPayloadSize(10) != 26 ||
        SwordBlowPayloadSize(99) != 26) return 6;
    if (!CrossedDescendingCadence(15.0f, 0.1f, 3.0f) ||
        CrossedDescendingCadence(14.0f, 0.1f, 3.0f) ||
        !CrossedDescendingCadence(14.95f, 0.1f, 3.0f) ||
        CrossedDescendingCadence(10.0f, 1.0f, 0.0f)) return 7;
    std::uint8_t payload[10] = {};
    WriteWordLE(payload, kSwordBlowSkill);
    WriteWordLE(payload + 2, 321);
    WriteWordLE(payload + 4, 3);
    WriteWordLE(payload + 6, 100);
    WriteWordLE(payload + 8, 101);
    SwordBlowTargetPayload decoded = {};
    if (!DecodeSwordBlowPayload(payload, sizeof(payload), decoded) ||
        decoded.skill != kSwordBlowSkill || decoded.caster != 321 ||
        decoded.targetCount != 2 || decoded.targets[0] != 100 ||
        decoded.targets[1] != 101) return 8;
    WriteWordLE(payload, kDragonViolentTargetSkill);
    if (!DecodeSwordBlowPayload(payload, sizeof(payload), decoded) ||
        decoded.skill != kDragonViolentTargetSkill || decoded.caster != 321 ||
        decoded.targetCount != 2) return 11;
    WriteWordLE(payload, kHavocSpearTargetSkill);
    if (!DecodeSwordBlowPayload(payload, sizeof(payload), decoded) ||
        decoded.skill != kHavocSpearTargetSkill || decoded.caster != 321 ||
        decoded.targetCount != 2) return 12;
    WriteWordLE(payload, kRainingArrowTargetSkill);
    if (!DecodeSwordBlowPayload(payload, sizeof(payload), decoded) ||
        decoded.skill != kRainingArrowTargetSkill || decoded.caster != 321 ||
        decoded.targetCount != 2) return 14;
    WriteWordLE(payload, kChaosBladeTargetSkill);
    if (!DecodeSwordBlowPayload(payload, sizeof(payload), decoded) ||
        decoded.skill != kChaosBladeTargetSkill || decoded.caster != 321 ||
        decoded.targetCount != 2) return 15;
    if (DecodeSwordBlowPayload(payload, 5, decoded) ||
        DecodeSwordBlowPayload(nullptr, sizeof(payload), decoded)) return 9;
    std::puts("PASS: Sword Blow colors, flight choreography, math and bounded target-list protocol");
}
