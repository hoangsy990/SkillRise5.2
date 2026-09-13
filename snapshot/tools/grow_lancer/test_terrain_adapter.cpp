// Contract test with a tiny GL matrix model, not a visual/parity test.
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
typedef int GLint;
typedef float GLfloat;
typedef float vec3_t[3];
enum { GL_MATRIX_MODE=1, GL_TEXTURE_MATRIX=2, GL_TEXTURE=3, GL_MODELVIEW=4 };
static GLint mode=GL_MODELVIEW;
static float matrix[16], observed[16];
static int calls=0, glCalls=0;
static bool throwOnDraw=false;
static void glGetIntegerv(int key, GLint* out) { assert(key==GL_MATRIX_MODE); *out=mode; ++glCalls; }
static void glGetFloatv(int key, GLfloat* out) { assert(key==GL_TEXTURE_MATRIX); std::memcpy(out,matrix,sizeof(matrix)); ++glCalls; }
static void glMatrixMode(GLint value) { mode=value; ++glCalls; }
static void glLoadMatrixf(const GLfloat* in) { assert(mode==GL_TEXTURE); std::memcpy(matrix,in,sizeof(matrix)); ++glCalls; }
static void glTranslatef(float x,float y,float z)
{
    assert(mode==GL_TEXTURE); ++glCalls;
    for(int r=0;r<4;++r) matrix[12+r]+=matrix[r]*x+matrix[4+r]*y+matrix[8+r]*z;
}
static void glScalef(float x,float y,float z)
{
    assert(mode==GL_TEXTURE); ++glCalls;
    for(int r=0;r<4;++r) { matrix[r]*=x; matrix[4+r]*=y; matrix[8+r]*=z; }
}
static void RenderTerrainAlphaBitmap(int texture,float x,float y,float sx,float sy,
    vec3_t light,float rotation,float alpha,float height)
{
    assert(texture==32927 && x==100 && y==200 && sx==2.5f && sy==2.5f);
    assert(light[0]==.5f && light[1]==.07f && light[2]==.07f);
    assert(rotation==15 && alpha==.5f && height==5);
    std::memcpy(observed,matrix,sizeof(matrix)); ++calls;
    if(throwOnDraw) throw 1;
}
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerTerrainAdapter.h"
static void draw(bool flip)
{
    vec3_t light={.5f,.07f,.07f};
    rise::growlancer::RenderGrowLancerTerrainBitmap(32927,100,200,2.5f,2.5f,light,15,.5f,5,flip);
}
int main()
{
    for(int variant=0;variant<2;++variant)
    {
        for(int i=0;i<16;++i) matrix[i]=variant ? float(i+1)/8.f : (i%5==0 ? 1.f : 0.f);
        float before[16]; std::memcpy(before,matrix,sizeof(matrix));
        mode=variant ? GL_TEXTURE : GL_MODELVIEW;
        const GLint savedMode=mode;
        glCalls=0; draw(false);
        assert(glCalls==0 && std::memcmp(before,observed,sizeof(matrix))==0);
        draw(true);
        assert(mode==savedMode && std::memcmp(before,matrix,sizeof(matrix))==0);
        const float uv[][4]={{0,0,0,1},{1,1,0,1},{.3f,.7f,0,1},{-2,3,0,1}};
        for(int p=0;p<4;++p) for(int r=0;r<4;++r)
        {
            float actual=0;
            for(int c=0;c<4;++c) actual+=observed[c*4+r]*uv[p][c];
            float expected=before[r]*uv[p][0]+before[4+r]*(1-uv[p][1])+before[12+r];
            assert(std::fabs(actual-expected)<0.00001f);
        }
        throwOnDraw=true;
        try { draw(true); assert(false); } catch(int) {}
        throwOnDraw=false;
        assert(mode==savedMode && std::memcmp(before,matrix,sizeof(matrix))==0);
    }
    assert(calls==6);
    std::puts("PASS: native delegation, opt-out, V reflection, existing matrix, mode and exception restoration");
}
