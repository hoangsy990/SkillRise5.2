#pragma once
namespace rise { namespace growlancer {
// S21 18F930D, specialized ONLY for Wrath's verified phase=0 calls.
// Source clock units remain an integration contract: do not pass seconds or
// frame count without establishing correspondence to source global70B53D8.
// x86 CVTTSS2SI returns integer-indefinite for invalid/out-of-range inputs.
// Explicit handling avoids undefined C++ float-to-int conversion at long uptime.
constexpr int WrathClockToSourceTick(float clock)
{
    return clock != clock || clock >= 2147483648.f || clock < -2147483648.f
        ? (-2147483647 - 1) : static_cast<int>(clock);
}
constexpr float WrathGroundPulseAngle(float sourceClock, bool colorPulse)
{
    const float speed = colorPulse ? -8.f : 4.f;
    const int period = static_cast<int>(6283.185546875f / speed);
    const int remainder = WrathClockToSourceTick(sourceClock) % period;
    return (static_cast<float>(remainder) * .001f) * speed;
}

template<class Cosine>
constexpr float WrathGroundPulse(float sourceClock, bool colorPulse, Cosine cosine)
{
    const float angle = WrathGroundPulseAngle(sourceClock, colorPulse);
    // Source promotes float argument for cosine, then rounds the result to float.
    float value = static_cast<float>(cosine(static_cast<double>(angle)));
    if (angle < 3.1415927410125732f) value = -value;
    return (value + 1.f) * .5f;
}

// Layer0=lightmarks,1/2=flareBlue,3=Shockwave2. All share caller XY.
// Resource registration, additive state and native terrain ownership stay outside.
template<class Cosine, class Draw>
constexpr void SubmitWrathGroundLayers(float sourceClock, Cosine cosine, Draw draw)
{
    draw(0,8.f,.15f,.1f,1.f,0.f);
    const float rotation = sourceClock * .05f;
    draw(1,2.8f,.5f,.5f,.5f,rotation);
    draw(2,4.5f,.5f,.5f,.5f,-rotation);
    const float color = WrathGroundPulse(sourceClock,true,cosine);
    const float size = WrathGroundPulse(sourceClock,false,cosine)*2.f+.9f;
    draw(3,size,color*.25f,color*.65f,color*.85f,0.f);
}
} }
