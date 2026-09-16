#include "../Main5.2_RISE/RISE/GrowLancerWrathGroundPulse.h"
using rise::growlancer::WrathGroundPulseAngle;
using rise::growlancer::WrathGroundPulse;
using rise::growlancer::WrathClockToSourceTick;
constexpr bool Near(float a,float b) {return a-b<.00001f && b-a<.00001f;}
struct ConstantCosine { constexpr double operator()(double) const {return .5;} };
// Constant cosine isolates branch sign and normalization from libm accuracy.
static_assert(Near(WrathGroundPulseAngle(784.f,true),-6.272f),"negative speed before wrap");
static_assert(WrathGroundPulseAngle(785.f,true)==0.f,"truncated color period785");
static_assert(WrathGroundPulseAngle(1570.f,false)==0.f,"truncated scale period1570");
static_assert(Near(WrathGroundPulseAngle(-1.9f,true),.008f),"signed remainder and truncation toward zero");
static_assert(Near(WrathGroundPulseAngle(1.9f,false),.004f),"fractional clock truncation");
static_assert(WrathGroundPulse(785.f,false,ConstantCosine{})==.25f,"below pi negates cosine");
static_assert(WrathGroundPulse(786.f,false,ConstantCosine{})==.75f,"above pi keeps cosine");
static_assert(WrathGroundPulse(784.f,true,ConstantCosine{})==.25f,"negative angle keeps source branch");
static_assert(WrathClockToSourceTick(2147483648.f)==(-2147483647-1),"x86 positive overflow");
static_assert(WrathClockToSourceTick(4294967296.f)==(-2147483647-1),"DWORD uptime rounded to float");
static_assert(WrathClockToSourceTick(-2147483648.f)==(-2147483647-1),"valid negative boundary");
static_assert(WrathClockToSourceTick(2147483520.f)==2147483520,"last valid positive float");
static_assert(WrathGroundPulseAngle(4294967296.f,true)==
              WrathGroundPulseAngle(-2147483648.f,true),"overflow uses signed source remainder");
struct LayerCapture {
    int count=0;
    float size[4]={},red[4]={},green[4]={},blue[4]={},rotation[4]={};
    constexpr void operator()(int layer,float s,float r,float g,float b,float a) {
        if(layer!=count) {count=-100;return;}
        size[count]=s;red[count]=r;green[count]=g;blue[count]=b;rotation[count]=a;
        ++count;
    }
};
struct CaptureRef {
    LayerCapture* capture;
    constexpr void operator()(int i,float s,float r,float g,float b,float a) const {
        (*capture)(i,s,r,g,b,a);
    }
};
constexpr bool GroundLayers() {
    LayerCapture c{};
    rise::growlancer::SubmitWrathGroundLayers(100.f,ConstantCosine{},CaptureRef{&c});
    return c.count==4 && Near(c.size[0],8.f) && Near(c.red[0],.15f) &&
        Near(c.green[0],.1f) && Near(c.blue[0],1.f) &&
        Near(c.size[1],2.8f) && Near(c.size[2],4.5f) &&
        Near(c.rotation[1],5.f) && Near(c.rotation[2],-5.f) &&
        Near(c.size[3],1.4f) && Near(c.red[3],.0625f) &&
        Near(c.green[3],.1625f) && Near(c.blue[3],.2125f);
}
static_assert(GroundLayers(),"four-layer order, scales, RGB, opposite rotation and pulse mapping");
