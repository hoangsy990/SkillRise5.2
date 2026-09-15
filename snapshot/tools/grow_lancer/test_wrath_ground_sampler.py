"""Actual Wrath sampler with controlled GL readback; not an OpenGL context test."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[2]
text=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
record='struct WrathGroundSample'+text.split('struct WrathGroundSample',1)[1].split('WrathGroundSample gWrathGroundSamples',1)[0]
body='void RecordWrathGroundQA('+text.split('void RecordWrathGroundQA(',1)[1].split('void RecordWrathBuffVisualQA(',1)[0]
for forbidden in ('AppendQALog(', 'glBlendFunc(', 'glEnable(', 'glDisable(', 'CreateEffect('):
    assert forbidden not in body
fixture='''using DWORD=unsigned long;
constexpr int kWrathBrokenBitmap=10,GL_BLEND=1,GL_BLEND_SRC=2,GL_BLEND_DST=3,GL_TEXTURE_BINDING_2D=4;
struct OBJECT { int Type=kWrathBrokenBitmap; float Scale=2.f,LifeTime=10.f,Light[3]={.1f,.2f,.3f}; };
'''+record+'''
struct Fixture {
    int gControllerSkill=278;
    unsigned gWrathGroundCount=0,reads=0;
    DWORD gControllerStart=20;
    WrathGroundSample gWrathGroundSamples[32]{};
    bool blendEnabled=true;
    int sourceFactor=1,destinationFactor=1,boundTexture=123;
    constexpr bool glIsEnabled(int) { ++reads; return blendEnabled; }
    constexpr void glGetIntegerv(int name,int* value) {
        ++reads;
        *value=name==GL_BLEND_SRC?sourceFactor:name==GL_BLEND_DST?destinationFactor:boundTexture;
    }
    constexpr DWORD GetTickCount() {return 50;}
'''+body.replace('void RecordWrathGroundQA(', 'constexpr void RecordWrathGroundQA(',1)+'''
};
constexpr bool Contract(){
    Fixture f; OBJECT effect;
    f.RecordWrathGroundQA(effect);
    const auto& s=f.gWrathGroundSamples[0];
    if(f.reads!=4 || s.blend!=1 || s.source!=1 || s.destination!=1 || s.texture!=123 || s.elapsed!=30) return false;
    if(s.scale!=2.f || s.life!=10.f || s.light[0]!=.1f || s.light[2]!=.3f) return false;
    // Preserve a deliberately wrong GL state in diagnostics; do not normalize it.
    f.blendEnabled=false;f.sourceFactor=0;f.destinationFactor=5;
    f.RecordWrathGroundQA(effect);
    if(f.gWrathGroundSamples[1].blend || f.gWrathGroundSamples[1].source!=0 || f.gWrathGroundSamples[1].destination!=5) return false;
    for(unsigned i=0;i<100;++i) f.RecordWrathGroundQA(effect);
    if(f.gWrathGroundCount!=32 || f.reads!=128) return false;
    f.gWrathGroundCount=0;f.gControllerSkill=274;
    f.RecordWrathGroundQA(effect);
    if(f.gWrathGroundCount || f.reads!=128) return false;
    f.gControllerSkill=278;effect.Type=999;
    f.RecordWrathGroundQA(effect);
    return !f.gWrathGroundCount && f.reads==128;
}
static_assert(Contract(), "actual Wrath readback: values, wrong state retained,32 bound and zero reads on excluded calls");
'''
build=root/'ExMain_RISE_PC/Tests/GrowLancerBuild'
cpp=build/'WrathGroundSampler.cpp';cpp.write_text(fixture,encoding='utf-8')
compiler=Path('C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/cl.exe')
subprocess.run([str(compiler),'/nologo','/std:c++14','/W4','/WX','/c',str(cpp),'/Fo'+str(build/'WrathGroundSampler.obj')],check=True)
print('PASS actual Wrath sampler:32 sample bound,128 reads maximum,field fidelity and excluded-call zero reads')
print('Scope: controlled GL API results; no actual GL/GPU or visual acceptance')
