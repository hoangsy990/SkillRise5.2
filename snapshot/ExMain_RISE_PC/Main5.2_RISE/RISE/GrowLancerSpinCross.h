#pragma once

namespace rise { namespace growlancer {
// S21 157D348: whole-tick update, including life zero. 157D38B adds
// above life 5; 157D3BE subtracts otherwise. No alpha/scale clamp.
template<class Object>
void UpdateSpinCrossTick(Object& object)
{
    object.Alpha -= 0.1f;
    if (object.LifeTime > 5.0f)
        object.Scale += object.Distance * 0.1f;
    else
        object.Scale -= object.Distance * 0.1f;
}
} }
