#pragma once
namespace rise { namespace growlancer {
// Pin5F5:157BF0C; maxLife8, half4 (NOT body-style half+1).
template<class Object> void UpdateShiningPinTick(Object& o)
{
    o.Scale+=.15f;
    o.Alpha=.2f+(o.LifeTime>4.f ? 8.f-o.LifeTime : o.LifeTime)/4.f*.3f;
}
// Spin5F7:157C119; maxLife20, half10 (NOT11).
template<class Object> void UpdateShiningSpinTick(Object& o)
{
    o.Angle[1]+=20.f;
    o.Alpha=.1f+(o.LifeTime>10.f ? 20.f-o.LifeTime : o.LifeTime)/10.f*.7f;
}
// Body5F4:157BE66..157BF07. Caller supplies integral ticks including life0.
template<class Object> void UpdateShiningBodyTick(Object& o)
{
    if(o.SubType==0) o.Scale+=.01f;
    o.Alpha+=o.LifeTime>7.f ? 1.f/7.f : -1.f/7.f;
    // No clamp in the recovered branch.
}
} }
