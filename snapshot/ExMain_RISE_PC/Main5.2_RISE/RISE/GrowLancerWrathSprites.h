#pragma once
namespace rise { namespace growlancer {
// Source groups stay separate: terrain and scatter emissions occur between them.
// Sink arguments: bone index, world-Z offset, scale, RGB. No anatomical guesses.
template<class Random, class Emit>
constexpr void SubmitWrathLightmarks(Random random, Emit emit)
{
    const int bones[] = {4,11,34,25,35,26,10,4,17,18,20};
    const float jitter = static_cast<float>(random()%15)/37.f+.4f;
    for(int i=0;i<11;++i)
    {
        const float base = i<6 ? .2f : i<8 ? .5f : .8f;
        emit(bones[i],i==10 ? 20.f : 0.f,base+jitter,.23f,.2f,1.f);
    }
}
template<class Emit>
constexpr void SubmitWrathBoneFlares(Emit emit)
{
    emit(29,0.f,4.f,.22f,.2f,1.f);
    emit(38,0.f,4.f,.22f,.2f,1.f);
}
} }
