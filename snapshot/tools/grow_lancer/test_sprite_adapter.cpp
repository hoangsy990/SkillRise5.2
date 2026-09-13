#include <cassert>
#include <cstdio>
#define jdk_shader_local330
typedef int GLint;
enum {GL_TEXTURE_2D=1, GL_TEXTURE_WRAP_S=2, GL_TEXTURE_WRAP_T=3, GL_CLAMP=4};
static int wrapS=7, wrapT=8, bound=0, queued=0, draws=0;
static int expectedS=GL_CLAMP, expectedT=GL_CLAMP;
static bool throwOnDraw=false;
namespace OGL330SPRITE {
void FlushForStateChange(bool change) {
    assert(change);
    if (queued) {
        assert(wrapS == expectedS && wrapT == expectedT);
        ++draws;
        queued=0;
    }
}
}
void glGetTexParameteriv(int target,int axis,int* value) {
    assert(target==GL_TEXTURE_2D);
    *value=axis==GL_TEXTURE_WRAP_S ? wrapS : wrapT;
}
void glTexParameteri(int target,int axis,int value) {
    assert(target==GL_TEXTURE_2D);
    (axis==GL_TEXTURE_WRAP_S ? wrapS : wrapT)=value;
}
void BindTexture(int texture) {bound=texture;}
void RenderSprite(int texture,float*,float width,float height,float*,float rotation) {
    assert(bound==texture && width==32 && height==64 && rotation==45);
    assert(wrapS==GL_CLAMP && wrapT==GL_CLAMP);
    expectedS=GL_CLAMP;
    expectedT=GL_CLAMP;
    queued=1;
    if (throwOnDraw) throw 1;
}
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerSpriteAdapter.h"
int main() {
    float v[3]={};
    rise::growlancer::RenderBrecheFireSprite(10,v,32,64,v,45);
    assert(draws==1 && !queued && wrapS==7 && wrapT==8);
    throwOnDraw=true;
    try {rise::growlancer::RenderBrecheFireSprite(10,v,32,64,v,45);}
    catch(int) {}
    assert(draws==2 && !queued && wrapS==7 && wrapT==8);
    // A queued ordinary sprite on the same texture must draw before mutation.
    throwOnDraw=false;
    queued=1;
    expectedS=7;
    expectedT=8;
    rise::growlancer::RenderBrecheFireSprite(10,v,32,64,v,45);
    assert(draws==4 && !queued && wrapS==7 && wrapT==8);
    std::puts("PASS: scoped wrap, native submission, flush-before-restore, exception restoration; GL model only");
}
