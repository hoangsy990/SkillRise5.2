#pragma once

namespace rise { namespace growlancer {
enum BrecheGroundKind { kBrecheMarks, kBrecheTwilight02, kBrecheTwilight01 };

// Invoked once per whole effect tick, including LifeTime==0 before cleanup.
// Preserve source overshoot/negative alpha; do not add a visual clamp here.
template<class Effect>
void UpdateBrecheGroundTick(Effect& effect, BrecheGroundKind kind)
{
    if (kind == kBrecheMarks)
    {
        // S21 153881C: +/- 1/(MaxLife*.5); constructor fixes MaxLife=20.
        effect.Alpha += effect.LifeTime > 10.f ? .1f : -.1f;
        return;
    }
    // S21 14FC73A (809F/0) and14FC3D7 (809E/14).
    effect.Angle[2] += 15.f;
    effect.Alpha += effect.LifeTime >= 11.f ? .1f : -.1f;
    if (kind == kBrecheTwilight01)
        effect.Scale += .1f;
}
} }
