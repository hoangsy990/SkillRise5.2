#include "../Main5.2_RISE/RISE/PegasusSwordWrathMath.h"
#include <cassert>
#include <cmath>
#include <iostream>

using namespace rise::pegasus;

static bool Near(float a, float b)
{
    return std::fabs(a - b) < 0.0001f;
}

int main()
{
    static_assert(kSwordWrathRootLifetime == 200, "Pegasus root lifetime drift");
    static_assert(kSwordWrathGroundStarLifetime == 30, "Pegasus star lifetime drift");
    static_assert(kSwordWrathWaveLifetime == 40, "Pegasus wave lifetime drift");
    static_assert(kSwordWrathLineLifetime == 20 &&
        kSwordWrathSmokeLifetime == 20 &&
        kSwordWrathPinStarLifetime == 30 &&
        kSwordWrathSmokeEmitThreshold == 160,
        "Pegasus accent lifetime/threshold drift");
    static_assert(kSwordWrathPinStarInputScale == 0.45f &&
        kSwordWrathSmokeScaleSplit == 14.0f &&
        kSwordWrathSmokeScaleDelta == 0.75f &&
        kSwordWrathPinStarScaleDelta == 0.1f &&
        kSwordWrathPinStarRise == 10.0f,
        "Pegasus accent movement drift");
    static_assert(kSwordWrathRedLight.r == 1.0f &&
        kSwordWrathRedLight.g == 0.2f && kSwordWrathRedLight.b == 0.19f,
        "Pegasus Sword Wrath red light drift");
    static_assert(kSwordWrathParticleLight.r == 1.0f &&
        kSwordWrathParticleLight.g == 0.15f && kSwordWrathParticleLight.b == 0.05f,
        "Pegasus Sword Wrath particle light drift");
    static_assert(kSwordWrathWaveLight.r == 1.0f &&
        kSwordWrathWaveLight.g == 0.158f && kSwordWrathWaveLight.b == 0.119f,
        "Pegasus Sword Wrath wave light drift");

    assert(Near(SwordWrathPulse(200, 20, 0.7f), 0.3f));
    assert(Near(SwordWrathPulse(190, 20, 0.7f), 1.0f));
    assert(Near(SwordWrathPulse(200, 15, 1.0f), 0.5f));
    assert(SwordWrathGroundStarAlphaDelta(16, 1.0f) > 0.0f);
    assert(SwordWrathGroundStarAlphaDelta(15, 1.0f) < 0.0f);
    assert(SwordWrathGroundStarScaleDelta(16, 1.0f) > 0.0f);
    assert(SwordWrathGroundStarScaleDelta(15, 1.0f) < 0.0f);

    // Pegasus 0x67AD7D calls the accent emitter only from the 0x27B3 root
    // branch. The four 0x27B4 layers must never multiply the particle count.
    assert(ShouldEmitSwordWrathAccent(true, 200.0f, 1.0f));
    assert(!ShouldEmitSwordWrathAccent(false, 200.0f, 1.0f));

    assert(SwordWrathAtlasFrame(200.0f) == 5);
    assert(Near(SwordWrathAtlasU(200.0f), 0.25f));
    assert(Near(SwordWrathAtlasV(200.0f), 0.25f));
    assert(SwordWrathAtlasFrame(195.0f) == 0);
    assert(SwordWrathAtlasFrame(-1.0f) == 0);

    assert(IsSwordWrathRenderAction(0x43));
    assert(IsSwordWrathRenderAction(0x9D));
    assert(IsSwordWrathRenderAction(0x9E));
    assert(IsSwordWrathRenderAction(0x9F));
    assert(!IsSwordWrathRenderAction(0x44));

    static_assert(kSwordWrathLineTailMin == 5 &&
        kSwordWrathLineTailMax == 10 &&
        kSwordWrathLineVelocityFactor == 0.06f,
        "Pegasus Sword Wrath line joint drift");
    assert(Near(SwordWrathLineLightFactor(20.0f), 1.0f));
    assert(Near(SwordWrathLineLightFactor(10.0f), 0.5f));
    assert(Near(SwordWrathLineLightFactor(0.0f), 0.0f));

    std::cout << "Pegasus Sword Wrath math/provenance: PASS\n";
    return 0;
}
