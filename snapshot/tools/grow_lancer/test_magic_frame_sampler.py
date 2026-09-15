"""Compile actual frame sampler against constexpr storage fixtures, not runtime QA."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[2]
source=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
record='struct MagicFrameSample'+source.split('struct MagicFrameSample',1)[1].split('MagicFrameSample gMagicFrames',1)[0]
body='void RecordMagicFrameQA('+source.split('void RecordMagicFrameQA(',1)[1].split('void RecordControllerQASample',1)[0]
assert all(token not in body for token in ('AppendQALog','fopen','CreateEffect','AnimationFrame ='))
fixture='''using DWORD = unsigned long;
struct Actor { struct { unsigned CurrentAction=287; float AnimationFrame=5.5f; } Object; };
'''+record+'''
struct Fixture {
    Actor actor{}; Actor* Hero=&actor;
    int gControllerSkill=274;
    MagicFrameSample gMagicFrames[512]{};
    unsigned gMagicFrameCount=0,gMagicFrameDropped=0;
    DWORD gControllerStart=10;
    constexpr DWORD GetTickCount() { return 50; }
'''+body.replace('void RecordMagicFrameQA(', 'constexpr void RecordMagicFrameQA(',1)+'''
};
constexpr bool Test() {
    Fixture f;
    f.RecordMagicFrameQA(true,true,211,11,5);
    const auto& s=f.gMagicFrames[0];
    if(!s.owned || !s.ran || s.before!=211 || s.after!=11 || s.completed!=5 || s.action!=287 || s.frame!=5.5f || s.elapsed!=40) return false;
    for(unsigned i=0;i<600;++i) f.RecordMagicFrameQA(false,false,0,0,0);
    if(f.gMagicFrameCount!=512 || f.gMagicFrameDropped!=89) return false;
    f.gMagicFrameDropped=0xffffffffu;
    f.RecordMagicFrameQA(false,false,0,0,0);
    if(f.gMagicFrameDropped!=0xffffffffu) return false;
    f.gMagicFrameCount=0; f.gControllerSkill=276;
    f.RecordMagicFrameQA(true,true,40,0,1);
    if(f.gMagicFrameCount) return false;
    f.gControllerSkill=274; f.Hero=nullptr;
    f.RecordMagicFrameQA(true,true,40,0,1);
    return f.gMagicFrameCount==0;
}
static_assert(Test(), "actual bounded frame telemetry and exclusion contract");
'''
build=root/'ExMain_RISE_PC/Tests/GrowLancerBuild'
generated=build/'MagicFrameSampler.cpp'
generated.write_text(fixture,encoding='utf-8')
compiler=Path('C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/cl.exe')
subprocess.run([str(compiler),'/nologo','/std:c++14','/W4','/WX','/c',str(generated),'/Fo'+str(build/'MagicFrameSampler.obj')],check=True)
print('PASS actual Magic frame sampler: values,512 capacity,saturating drops,skill/null exclusions')
print('OPEN real frame capture and clock/animation/pool runtime correctness')
