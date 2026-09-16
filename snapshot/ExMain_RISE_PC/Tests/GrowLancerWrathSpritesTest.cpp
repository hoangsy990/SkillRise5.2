#include "../Main5.2_RISE/RISE/GrowLancerWrathSprites.h"
struct Capture {
    int count=0,calls=0,bones[13]={};
    float z[13]={},scale[13]={},red[13]={};
};
struct Random {
    Capture* c;
    constexpr int operator()() const {++c->calls;return 14;}
};
struct Emit {
    Capture* c;
    constexpr void operator()(int b,float z,float s,float r,float g,float blue) const {
        if(g!=.2f || blue!=1.f || c->count>=13) {c->count=-100;return;}
        const int n=c->count++;c->bones[n]=b;c->z[n]=z;c->scale[n]=s;c->red[n]=r;
    }
};
constexpr bool Check() {
    Capture c{};
    rise::growlancer::SubmitWrathLightmarks(Random{&c},Emit{&c});
    rise::growlancer::SubmitWrathBoneFlares(Emit{&c});
    const int bones[]={4,11,34,25,35,26,10,4,17,18,20,29,38};
    if(c.count!=13 || c.calls!=1) return false;
    for(int i=0;i<13;++i) {
        const float expected=i<11 ? (i<6?.2f:i<8?.5f:.8f)+(14.f/37.f+.4f):4.f;
        if(c.bones[i]!=bones[i] || c.z[i]!=(i==10?20.f:0.f) ||
            c.scale[i]!=expected || c.red[i]!=(i<11?.23f:.22f)) return false;
    }
    return true;
}
static_assert(Check(),"13 ordered bone sprites, one shared random draw, world offset and two groups");
