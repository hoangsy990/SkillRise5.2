#pragma once
namespace rise { namespace growlancer {
// 13EC433..13ECB4A. Emit calls may consume RNG in the native allocator.
// Do not batch random draws ahead of those calls.
template<class Random, class Scatter, class Flares, class Mono>
constexpr void SubmitWrathParticleSequence(Random random, Scatter scatter,
    Flares flares, Mono mono)
{
    int x = random()%220-110;
    int y = random()%220-110;
    const float scale = static_cast<float>(random()%3+3)*.1f;
    int texture = random()%3;
    scatter(texture,x,y,scale);
    x = random()%220-110;
    y = random()%220-110;
    texture = random()%3;
    scatter(texture,x,y,scale);
    flares();
    const int variant = random()%3 == 1 ? 1 : 0;
    mono(variant,0);
    mono(variant,1);
    mono(2,0);
    mono(2,1);
}
} }
