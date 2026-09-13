#pragma once
namespace rise { namespace growlancer {
// Private carrier mapping: Distance=size multiplier, StartPosition=initial RGB.
// These fields are local native storage, not transplanted S21 offsets.
template<class Object>
void InitializeSpinGround(Object& object)
{
    object.LifeTime = 10.f;
    object.Timer = 0.f;
    object.Alpha = 1.f;
    object.Distance = 0.f; // S21 reset131607C, called143E71F before constructor.
    for (int axis = 0; axis < 3; ++axis)
        object.StartPosition[axis] = object.Light[axis];
}
template<class Object>
void UpdateSpinGroundTick(Object& object)
{
    object.Distance = (11.f - object.LifeTime) * .1f;
    object.Alpha -= .1f;
    for (int axis = 0; axis < 3; ++axis)
        object.Light[axis] = object.StartPosition[axis] * object.Alpha;
}
} }
