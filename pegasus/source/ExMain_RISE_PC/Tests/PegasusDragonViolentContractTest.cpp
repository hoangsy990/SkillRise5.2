#include "../Main5.2_RISE/RISE/PegasusDragonViolentMath.h"
#include <cstdio>

int main()
{
    using namespace rise::pegasus;
    static_assert(kDragonViolentHeadTerrainOffset == 80.0f,
        "Dragon head terrain offset drifted");
    static_assert(kDragonViolentHeadLifetime == 30.0f &&
        kDragonViolentHeadScale == 1.15f,
        "Dragon head lifetime/scale drifted");
    static_assert(kDragonViolentHeadJointZOffset == -140.0f &&
        kDragonViolentHeadJointScale == 75.0f &&
        kDragonViolentHeadJointSubtype == 18,
        "Dragon head JointSpirit02 contract drifted");
    static_assert(!kDragonViolentHeadTracksOwner &&
        kDragonViolentTargetImpactTracksOwner,
        "Dragon head/target ownership roles drifted");
    std::puts("PASS: Dragon Violent radial-head ownership and joint contract");
}
