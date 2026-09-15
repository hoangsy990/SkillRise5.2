"""Compile actual legacy BMD animation body without rewriting its encoding."""
from pathlib import Path
import subprocess
import hashlib
root = Path(__file__).resolve().parents[2]
source = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp').read_text(encoding='latin-1')
method = 'bool BMD::PlayAnimation(' + source.split('bool BMD::PlayAnimation(',1)[1].split('void BMD::AnimationTransformWithAttachHighModel_usingGlobalTM',1)[0]
assert method.count('*AnimationFrame += Speed * stepFactor;') == 1
backup = (root/'ExMain_RISE_PC/Tests/GrowLancerBuild/ZzzBMD.before-comment-format.bin').read_bytes()
assert hashlib.sha256(backup).hexdigest() == '831a4c0bd79cad2720c2b3ca5c54efeb9325d115dbf243903af3288028be7d4a'
original = backup.decode('latin-1').replace('\r\n','\n').split('bool BMD::PlayAnimation(',1)[1].split('void BMD::AnimationTransformWithAttachHighModel_usingGlobalTM',1)[0]
original_body = original[original.index('{'):].replace('FPS_ANIMATION_FACTOR', 'stepFactor')
step = method.split('bool BMD::PlayAnimationStep(',1)[1]
assert step[step.index('{'):] == original_body, 'Unexpected native algorithm change'
method = method.replace('bool BMD::', 'constexpr bool BMD::')
fixture = r'''
using vec3_t = float[3];
constexpr float FPS_ANIMATION_FACTOR = .5f;
constexpr int SceneFlag = 5, CHARACTER_SCENE = 4;
constexpr int WD_39KANTURU_3RD = 39, MONSTER01_APEAR = 6;
struct Map { int WorldActive = 0; };
constexpr Map gMapManager{};
constexpr float maxf(float a, float b) { return a > b ? a : b; }
struct Action { int NumAnimationKeys = 12; bool Loop = false, LockPositions = false; };
struct BMD {
    int NumActions = 1;
    unsigned short CurrentAction = 0;
    Action Actions[1]{};
    float CurrentAnimation = 0;
    int CurrentAnimationFrame = 0;
    constexpr bool PlayAnimation(float*,float*,unsigned short*,float,vec3_t,vec3_t);
    constexpr bool PlayAnimationStep(float*,float*,unsigned short*,float,vec3_t,vec3_t,float);
};
''' + method + r'''
constexpr bool Exercise() {
    BMD b;
    float frame = 1.f, prior = 0.f;
    unsigned short action = 0;
    float origin[3]{}, angle[3]{};
    if (!b.PlayAnimation(&frame,&prior,&action,.5f,origin,angle) || frame != 1.25f) return false;
    if (prior != 0.f) return false; // No integer crossing: blend prior is unchanged.
    frame = 1.f;
    if (!b.PlayAnimationStep(&frame,&prior,&action,.5f,origin,angle,1.f) || frame != 1.5f) return false;
    frame = 11.75f;
    if (b.PlayAnimation(&frame,&prior,&action,1.f,origin,angle) || frame != .25f || prior != 11.f) return false;
    b.Actions[0].Loop = true; frame = 11.75f;
    if (b.PlayAnimation(&frame,&prior,&action,1.f,origin,angle) || frame != 11.99f) return false;
    b.Actions[0].Loop = false; b.Actions[0].LockPositions = true; frame = 10.75f;
    if (b.PlayAnimation(&frame,&prior,&action,1.f,origin,angle) || frame != .25f) return false;
    b.NumActions = 0; frame = 2.f;
    if (!b.PlayAnimation(&frame,&prior,&action,1.f,origin,angle) || frame != 2.f) return false;
    return b.PlayAnimation(nullptr,&prior,&action,1.f,origin,angle);
}
static_assert(Exercise(), "Actual legacy fractional factor, blending prior, wrap, held end, lock position and guards");
'''
out = root/'ExMain_RISE_PC/Tests/GrowLancerBuild/NativeAnimationBaselineTest'
out.mkdir(exist_ok=True)
cpp = out/'NativeAnimationBaseline.cpp'
cpp.write_text(fixture)
compiler = r'C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x86\cl.exe'
subprocess.run([compiler,'/nologo','/std:c++20','/W4','/WX','/wd4100','/c',str(cpp),'/Fo'+str(out/'NativeAnimationBaseline.obj')],check=True)
print('PASS legacy wrapper behavior and explicit factor; native algorithm matches pinned pre-refactor body except factor operand')
print('No private skill timing integration or visual acceptance implied')
