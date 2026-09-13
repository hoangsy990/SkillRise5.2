#include "../Main5.2_RISE/RISE/PegasusShiningBirdMath.h"
#include <cassert>
#include <iostream>

using namespace rise::pegasus;

int main()
{
    static_assert(kShiningBirdRootAdvance == 250.0f, "root advance drift");
    static_assert(kShiningBirdRootLifetime == 26.0f, "root lifetime drift");
    static_assert(kShiningBirdRootScale == 1.3f, "root scale drift");
    static_assert(kShiningBirdAnimationSpeed == 0.5f,
        "animation speed drift");
    static_assert(kShiningBirdLightBoneCount == 12, "bone count drift");
    static_assert(kShiningBirdLightBones[0] == 21 &&
        kShiningBirdLightBones[11] == 70, "bone table drift");
    static_assert(kShiningBirdSmokeLight.r == 0.5f &&
        kShiningBirdSmokeLight.g == 0.4f &&
        kShiningBirdSmokeLight.b == 1.0f, "smoke color drift");
    static_assert(kShiningBirdPinStarLight.r == 0.9f &&
        kShiningBirdPinStarLight.g == 0.8f &&
        kShiningBirdPinStarLight.b == 0.8f, "pin-star color drift");
    static_assert(kShiningBirdWindLight.r == 0.5f &&
        kShiningBirdWindLight.g == 0.65f &&
        kShiningBirdWindLight.b == 0.9f, "wind color drift");
    static_assert(kShiningBirdPillarLight.r == 0.4f &&
        kShiningBirdPillarLight.g == 0.65f &&
        kShiningBirdPillarLight.b == 1.0f, "pillar color drift");
    assert(kShiningBirdLightBones[5] == 47);
    std::cout << "Pegasus Shining Bird geometry/color provenance: PASS\n";
    return 0;
}
