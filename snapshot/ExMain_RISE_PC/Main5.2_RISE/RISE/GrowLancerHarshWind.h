#pragma once

namespace rise { namespace growlancer {
// Per-slot metadata, captured at allocation rather than dereferencing a
// possibly recycled owner during later updates. Legacy slots take one tick.
struct HarshBlurClock {
    bool enabled = false;
    float remainder = 0.f;
    void Reset(bool privateHarsh) { enabled=privateHarsh; remainder=0.f; }
};
template<class Blur, class Tick>
void AdvanceHarshBlur(Blur& blur, HarshBlurClock& clock, float factor, Tick tick)
{
    if (!blur.Live) return;
    if (!clock.enabled) { tick(blur); return; }
    clock.remainder += factor;
    while (clock.remainder >= 1.f && blur.Live)
    {
        clock.remainder -= 1.f;
        tick(blur);
    }
}
// Scalar portion only; native AngleMatrix/VectorRotate owns movement.
// S21 1577EC2..1577F01 and 1578057..157810E, once per integer tick.
template<class Object> void UpdateHarshWind01Tick(Object& object)
{
    object.Scale += .1f;
    object.Alpha -= .125f; // No clamp in S21; includes life-zero update.
}
template<class Object> void UpdateHarshWind02Tick(Object& object)
{
    object.Scale += .45f;
    for (int channel = 0; channel < 3; ++channel)
        object.Light[channel] *= .9f;
}
} }
