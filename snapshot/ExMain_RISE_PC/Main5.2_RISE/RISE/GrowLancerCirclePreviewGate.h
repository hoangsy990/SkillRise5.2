#pragma once

namespace rise { namespace growlancer {

// Isolated QA policy only. This does not register an S21 buff or authorize a
// cast. Tick subtraction follows GetTickCount's wrap-safe short interval.
template<class Actor>
constexpr bool CirclePreviewStillArmed(const Actor* armed, const Actor* hero,
    bool heroLive, int currentWorld, int armedWorld,
    unsigned now, unsigned until)
{
    return armed && hero && heroLive && hero == armed &&
        currentWorld == armedWorld && static_cast<int>(now - until) < 0;
}

template<class Actor>
constexpr bool CirclePreviewAppliesTo(const Actor* armed, const Actor* actor)
{
    return armed && actor == armed;
}

} }
