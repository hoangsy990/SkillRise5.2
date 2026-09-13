#pragma once

namespace rise { namespace windsoul {

struct Base737Contract
{
    static constexpr int SkillId = 737;
    static constexpr int RootCount = 3;
    static constexpr int RootSubtype = 4;
    static constexpr float RootScale = 60.f;
    static constexpr float LocalLaunchY = 60.f;
    static constexpr float InitialTargetZOffset = 90.f;
    static constexpr int RandomXBase = -119;
    static constexpr int RandomXModulo = 240;
    static constexpr int RandomZBase = 49;
    static constexpr int RandomZModulo = 60;

    // Hash-pinned S21 joint constructor/move constants.  Keep these together
    // so the native 5.2 adapter cannot silently drift back to guessed values.
    static constexpr int SmokeSubtype = 0x50;
    static constexpr int MaxTails = 12;
    static constexpr float LifeTime = 35.f;
    static constexpr float InitialVelocity = 10.f;
    static constexpr float DirectionY = -15.f;
    static constexpr float TargetZOffset = 100.f;
    static constexpr float FadeStartLife = 17.f;
    static constexpr float FadeFactor = 0.1f;
    static constexpr float JointLightR = 0.4f;
    static constexpr float JointLightG = 0.6f;
    static constexpr float JointLightB = 0.7f;
    static constexpr float SmokeLightR = 0.2f;
    static constexpr float SmokeLightG = 0.65f;
    static constexpr float SmokeLightB = 0.75f;
    static constexpr float SmokeScale = 0.8f;
    static constexpr float VelocityStep = 0.4f;
    static constexpr float FinalVelocityStep = 0.1f;
    static constexpr float PitchThreshold = -90.f;
    static constexpr int PitchJitterBase = 12;
    static constexpr int PitchJitterModulo = 16;
    static constexpr float ImpactDistance = 100.f;
    static constexpr int ImpactLife = 5;
    static constexpr float ImpactScaleFactor = 0.6f;
    static constexpr float ImpactLight = 0.5f;
};

}}
