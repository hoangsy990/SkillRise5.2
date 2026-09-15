"""Compile the actual sampler body against bounded constexpr fixtures.

This validates filters/storage only, not engine timing or visible effects.
"""
from pathlib import Path
import subprocess

root = Path(__file__).resolve().parents[2]
source = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
ai = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzAI.cpp').read_text(encoding='latin-1')
setter = 'void SetAction(' + ai.split('void SetAction(', 1)[1].split('bool TestDistance(', 1)[0]
body = 'void RecordControllerQASample(' + source.split('void RecordControllerQASample(', 1)[1].split('void RecordSpinOutcomeQA(', 1)[0]
assert 'AppendQALog' not in body and 'fopen' not in body
sample = 'struct ControllerSample' + source.split('struct ControllerSample', 1)[1].split('ControllerSample gControllerSamples', 1)[0]
fixture = r'''
using DWORD = unsigned long;
struct OBJECT {
    OBJECT* Owner = nullptr;
    int SubType = 0, Type = 0, CurrentAction = 0, PriorAction = 0, m_sTargetIndex = 0;
    float AnimationFrame = 0, PriorAnimationFrame = 0, LifeTime = 0;
    int AttackPoint[1] = {};
};
struct CHARACTER { OBJECT Object; };
constexpr int MODEL_PLAYER = 0;
struct ACTION { float PlaySpeed = .5f; };
struct BMD { int NumActions = 300; ACTION* Actions = nullptr; };
// Deliberately symbolic IDs: this test is not resource registration proof.
constexpr int kSpinControllerModel = 1, kMagicPinControllerModel = 2,
    kHarshStrikeControllerModel = 3;
''' + sample + r'''
struct Fixture {
    CHARACTER actor{};
    CHARACTER* Hero = &actor;
    ACTION actions[300]{};
    BMD model[1] = {{300, actions}};
    BMD* Models = model;
    ControllerSample gControllerSamples[512]{};
    unsigned int gControllerCount = 0, gControllerDropped = 0;
    int gControllerSkill = 271;
    DWORD gControllerStart = 100;
    constexpr DWORD GetTickCount() const { return 125; }
    // Stub tests sampler wiring only; real cursor semantics have separate tests.
    constexpr bool ReadCharacterAnimationObservation(const OBJECT&, unsigned short& action, float& frame)
    { action = 287; frame = 3.f; return true; }
    constexpr ''' + body + r'''
    constexpr ''' + setter + r'''
};
constexpr bool Exercise(int skill, int model) {
    Fixture f{};
    f.gControllerSkill = skill;
    OBJECT o{};
    o.Owner = &f.actor.Object; o.Type = model;
    o.LifeTime = 10; o.AttackPoint[0] = 2; o.m_sTargetIndex = 5;
    f.actor.Object.CurrentAction = 287; f.actor.Object.AnimationFrame = 3.25f;
    f.actor.Object.PriorAnimationFrame = 2.75f;
    f.RecordControllerQASample(o, 0.5f);
    const ControllerSample& s = f.gControllerSamples[0];
    if (f.gControllerCount != 1 || s.slot != &o || s.elapsed != 25 ||
        s.frame != 3.25f || s.action != 287 || s.stage != 2 || s.life != 10 ||
        s.factor != 0.5f || s.target != 5 || s.priorFrame != 2.75f ||
        s.actionSpeed != .5f || s.observationAvailable != (skill == 274) ||
        s.observedFrame != (skill == 274 ? 3.f : -1.f) ||
        s.observedAction != (skill == 274 ? 287 : 0)) return false;
    for (int i = 0; i < 519; ++i) f.RecordControllerQASample(o, 1);
    if (f.gControllerCount != 512 || f.gControllerDropped != 8) return false;
    f.gControllerDropped = 0xffffffffu;
    f.RecordControllerQASample(o, 1);
    return f.gControllerDropped == 0xffffffffu;
}
constexpr bool Filters() {
    Fixture f{};
    OBJECT o{};
    o.Type = kSpinControllerModel;
    f.RecordControllerQASample(o, 1); // wrong/null owner
    o.Owner = &f.actor.Object;
    o.SubType = 1; f.RecordControllerQASample(o, 1);
    o.SubType = 0; o.Type = 99; f.RecordControllerQASample(o, 1);
    o.Type = kMagicPinControllerModel; f.RecordControllerQASample(o, 1);
    o.Type = kSpinControllerModel;
    f.gControllerSkill = 0; f.RecordControllerQASample(o, 1);
    f.gControllerSkill = 271; f.Hero = nullptr; f.RecordControllerQASample(o, 1);
    return f.gControllerCount == 0 && f.gControllerDropped == 0;
}
static_assert(Exercise(271, kSpinControllerModel), "Spin samples and capacity");
static_assert(Exercise(274, kMagicPinControllerModel), "Magic samples and capacity");
static_assert(Exercise(276, kHarshStrikeControllerModel), "Harsh samples and capacity");
static_assert(Filters(), "Inactive, owner, subtype, skill and type filters");
constexpr bool UnavailableSpeed() {
    Fixture f{};
    OBJECT o{}; o.Owner = &f.actor.Object; o.Type = kSpinControllerModel;
    f.actor.Object.CurrentAction = 300;
    f.RecordControllerQASample(o, 1);
    f.actor.Object.CurrentAction = -1;
    f.RecordControllerQASample(o, 1);
    f.actor.Object.CurrentAction = 0; f.model[0].Actions = nullptr;
    f.RecordControllerQASample(o, 1);
    f.Models = nullptr;
    f.RecordControllerQASample(o, 1);
    f.Models = f.model; f.model[0].Actions = f.actions; f.actor.Object.Type = 999;
    f.RecordControllerQASample(o, 1);
    for (unsigned i = 0; i < f.gControllerCount; ++i)
        if (f.gControllerSamples[i].actionSpeed != -1.f) return false;
    return f.gControllerCount == 5;
}
static_assert(UnavailableSpeed(), "No out of range, absent model/action or wrong actor lookup");
constexpr bool TransitionSamples() {
    Fixture f{};
    f.gControllerSkill = 274;
    OBJECT o{}; o.Owner = &f.actor.Object; o.Type = kMagicPinControllerModel;
    OBJECT& actor = f.actor.Object;
    actor.CurrentAction = 11; actor.AnimationFrame = 4.25f;
    f.SetAction(&actor, 287, true);
    f.RecordControllerQASample(o, 1);
    if (actor.PriorAction != 11 || f.gControllerSamples[0].frame != 0.f ||
        f.gControllerSamples[0].priorFrame != 4.25f) return false;
    actor.AnimationFrame = 5.5f;
    f.SetAction(&actor, 287, true); // Same-action recast must not synthesize frame zero.
    f.RecordControllerQASample(o, 1);
    if (f.gControllerSamples[1].frame != 5.5f ||
        f.gControllerSamples[1].priorFrame != 4.25f) return false;
    f.SetAction(&actor, 11, true); // Exit: sampler retains the actual non-cast action.
    f.RecordControllerQASample(o, 1);
    if (actor.PriorAction != 287 || f.gControllerSamples[2].action != 11 ||
        f.gControllerSamples[2].priorFrame != 5.5f) return false;
    f.SetAction(&actor, 300, true); // Actual native upper-bound rejection.
    return actor.CurrentAction == 11 && actor.AnimationFrame == 0.f;
}
static_assert(TransitionSamples(), "Actual native setter plus sampler: transition, recast, exit and rejected action");
'''
out = root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/ControllerSamplerTest'
out.mkdir(exist_ok=True)
cpp = out / 'ControllerSampler.cpp'
cpp.write_text(fixture)
compiler = Path(r'C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x86\cl.exe')
subprocess.run([str(compiler), '/nologo', '/std:c++17', '/W4', '/WX', '/c',
                str(cpp), '/Fo' + str(out / 'ControllerSampler.obj')], check=True)
print('PASS: actual sampler body, three skills, stored fields, 512 bound, overflow saturation and exclusion filters.')
print('PASS: actual native SetAction plus sampler, transition/repeated action/exit/upper-bound rejection.')
print('No engine scheduling, runtime capture or visual acceptance implied.')
