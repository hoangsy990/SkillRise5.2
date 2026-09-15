"""Compile exact native DeleteJoint body in a small constexpr slot-reuse fixture.

This tests native filtering and the added call, not real engine scheduling/GPU.
"""
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]
joint_source = (ROOT/'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectJoint.cpp').read_text(encoding='utf-8')
character_source = (ROOT/'ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp').read_text(encoding='utf-8')
delete = 'void DeleteJoint(' + joint_source.split('void DeleteJoint(',1)[1].split('bool SearchJoint(',1)[0]
init = character_source.split('void CreateCharacterPointer(',1)[1].split('c->PositionX = PositionX;',1)[0]
call = 'if (!o->Live)\n        DeleteJoint(BITMAP_FORCEPILLAR, o, 3);'
assert call in init
out = ROOT/'ExMain_RISE_PC/Tests/GrowLancerBuild/CircleReuseTest'
out.mkdir(exist_ok=True)
cpp = out/'NativeCircleTargetReuse.cpp'
cpp.write_text('''#define NULL 0
struct OBJECT { bool Live; };
struct JOINT { bool Live; int Type; int SubType; OBJECT* Target; };
constexpr int BITMAP_FORCEPILLAR = 123; // symbol stand-in; no runtime ID mapping
struct Fixture {
    static constexpr int MAX_JOINTS = 6;
    JOINT Joints[MAX_JOINTS];
    constexpr ''' + delete + '''
    constexpr void Reactivate(OBJECT* o) {
        ''' + call + '''
        o->Live = true;
    }
};
constexpr bool Reuse(bool alreadyLive) {
    OBJECT target{alreadyLive}, other{true};
    Fixture f{{
        {true, BITMAP_FORCEPILLAR, 3, &target},
        {true, BITMAP_FORCEPILLAR, 2, &target},
        {true, BITMAP_FORCEPILLAR, 3, &other},
        {true, BITMAP_FORCEPILLAR+1, 3, &target},
        {false, BITMAP_FORCEPILLAR, 3, &target},
        {true, BITMAP_FORCEPILLAR, 3, nullptr}
    }};
    f.Reactivate(&target);
    return target.Live && f.Joints[0].Live == alreadyLive &&
        f.Joints[1].Live && f.Joints[2].Live && f.Joints[3].Live &&
        !f.Joints[4].Live && f.Joints[5].Live && f.Joints[0].Target == &target;
}
static_assert(Reuse(false), "Dead-slot reuse must retire only old Circle contact");
static_assert(Reuse(true), "Same-live-key refresh must preserve all joints");
''',encoding='utf-8')
compiler = Path(r'C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x86\cl.exe')
subprocess.run([str(compiler),'/nologo','/std:c++17','/W4','/WX','/c',str(cpp),'/Fo'+str(out/'NativeCircleTargetReuse.obj')],check=True)
print('PASS: extracted native DeleteJoint and reuse call, six joint categories, dead-slot and live-refresh constexpr cases.')
print('No gameplay scheduling, effect-controller ownership or renderer acceptance implied.')
