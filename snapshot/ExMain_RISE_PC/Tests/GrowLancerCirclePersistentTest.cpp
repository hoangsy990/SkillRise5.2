#include "../Main5.2_RISE/RISE/GrowLancerCirclePersistent.h"

struct CircleParticleFixture
{
    bool Live;
    int LifeTime;
    float Alpha, Scale, Gravity, Position[3], Rotation;
    float Light[3], TurningForce[3];
};

constexpr bool TestCirclePersistent()
{
    using namespace rise::growlancer;
    if (!IsCirclePersistentBuff(216) || !IsCirclePersistentBuff(221) ||
        !IsCirclePersistentBuff(222) || IsCirclePersistentBuff(271) ||
        IsCirclePersistentBuff(424)) return false;
    if (kCircleFlareBones[0] != 20 || kCircleFlareBones[1] != 26 ||
        kCircleFlareBones[2] != 35 || kCircleFlareBones[3] != 25 ||
        kCircleFlareBones[4] != 34 || kCircleMonoBones[0] != 26 ||
        kCircleMonoBones[1] != 35 || kCircleMonoSubType != 12) return false;
    if (CircleFlarePulse(0) != .4f ||
        !(CircleFlarePulse(14) > .77f && CircleFlarePulse(14) < .78f))
        return false;
    CircleParticleFixture p = {true,20,0.f,.6f,2.f,{0.f,0.f,100.f},0.f,
        {0.f,0.f,0.f},{.3f,.38f,1.f}};
    UpdateCircleUpperArmMonoTick(p, [](){return 0;});
    if (!p.Live || !(p.Alpha > .19f && p.Alpha < .21f) ||
        !(p.Scale > .57f && p.Scale < .59f) || p.Position[2] != 102.f ||
        p.Rotation != 3.f || !(p.Light[2] > .19f && p.Light[2] < .21f))
        return false;
    p.LifeTime = 14;
    UpdateCircleUpperArmMonoTick(p, [](){return 0;});
    return !p.Live && p.Position[2] == 104.f && p.Rotation == 6.f;
}

static_assert(TestCirclePersistent(), "Circle Shield persistent layer contract");
