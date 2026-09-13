#pragma once
namespace rise { namespace growlancer {
// Shining Peak8073 subtypes2/3/4: S21 ctor15DE53A, update1612963.
template<class Joint> void InitPinJoint(Joint& j, float scale)
{
    j.Scale=scale; j.Velocity=60.f; j.LifeTime=20.f; j.MaxTails=18;
    j.m_byReverseUV=3; j.Collision=false; j.Angle[0]-=135.f;
    j.Light[0]=j.Light[1]=j.Light[2]=0.f;
}
template<class Joint, class Move, class Tail>
void AdvancePinJoint(Joint& j, float& remainder, float factor, Move move, Tail tail)
{
    if (!j.Live || factor<=0.f) return;
    remainder+=factor;
    while (remainder>=1.f && j.Live) {
        remainder-=1.f;
        move(j); // old angle/velocity; preserve this matrix for tail
        if (j.LifeTime<=16.f && !j.Collision) {
            j.Collision=true;
            j.Light[0]=j.Light[1]=.6f; j.Light[2]=1.f;
        }
        for (int axis=0;axis<3;++axis) j.Light[axis]*=.88f;
        if (j.SubType==2) {
            if (j.LifeTime>=16.f) j.Angle[0]+=32.f;
            else if (j.LifeTime>=10.f) j.Angle[0]-=5.f;
            else if (j.LifeTime>=5.f) j.Angle[0]+=2.f;
        } else {
            j.Angle[0]+=j.LifeTime>=14.f ? 30.f : (j.LifeTime>=5.f ? -15.f : 3.f);
        }
        tail(j);
        j.LifeTime-=1.f;
        if (j.LifeTime<0.f) j.Live=false;
    }
}
} }
