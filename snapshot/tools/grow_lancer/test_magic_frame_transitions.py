"""Run actual frame/reset functions with controlled clock and batch callback.

The callback is a fixture, not BMD/effect execution or visual acceptance.
"""
from pathlib import Path
import subprocess
import re
root=Path(__file__).resolve().parents[2]
source=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
frame='void BeginMagicPinFrame()'+source.split('void BeginMagicPinFrame()',1)[1].split('unsigned RunMagicPinQuanta(',1)[0]
reset='void ResetAllEffectAnimationSamples()'+source.split('void ResetAllEffectAnimationSamples()',1)[1].split('void RetireCharacterAnimationSamples',1)[0]
prefix='''#define RISE_GROW_LANCER_RUNTIME_QA
#include <cassert>
#include <cstdint>
#include <cstdio>
'''+f'#include "{(root/"GrowLancer/compat/MagicQuantumClock.h").as_posix()}"\n'+f'#include "{(root/"GrowLancer/compat/AnimationActorIdentity.h").as_posix()}"\n'+'''
using namespace rise::growlancer;
constexpr int MODEL_PLAYER=1, MAIN_SCENE=5;
constexpr int kMagicPinControllerModel=10,kMagicPin01Model=11,kMagicPin03Model=12,kMagicPinRootModel=13,kMagicPinAuxModel=14;
struct OBJECT { bool Live=true; int Type=MODEL_PLAYER; unsigned CurrentAction=287; };
constexpr int MAX_EFFECTS=2;
OBJECT Effects[MAX_EFFECTS];
struct Character { OBJECT Object; } actor;
Character* Hero=&actor;
int SceneFlag=MAIN_SCENE;
MagicQuantumClock gMagicClock;
bool gMagicClockStarted=false,gMagicFrameOwned=false,gMagicFrameRan=false;
unsigned long long gMagicPendingBefore=0,gMagicCompleted=0;
AnimationActorIdentity<uintptr_t> gAnimationActorIdentity;
struct Resettable { void Reset() {} void Invalidate() {} };
Resettable gAnimationObservation,gPrimaryAnimationCursors[2],gSecondaryAnimationCursors[2];
unsigned gAnimationObservationSerial=0;
bool gAnimationObservationArmed=false;
unsigned now=100,calls=0,limit=0xffffffffu;
bool enabled=true,resetDuringBatch=false;
unsigned GetTickCount() { return now; }
unsigned GetEnvironmentVariableA(const char*,char* value,unsigned) { value[0]=enabled?'1':'0';return 1; }
void RecordMagicFrameQA(bool,bool,unsigned long long,unsigned long long,unsigned long long) {}
void ResetAllEffectAnimationSamples();
unsigned RunMagicPinQuanta(OBJECT&,unsigned ticks) {
    ++calls;
    if(resetDuringBatch) { ResetAllEffectAnimationSamples(); return 1; }
    return ticks<limit?ticks:limit;
}
'''
tests='''
int main(int argc,char**) {
    enabled=argc==1;
    gAnimationActorIdentity.Bind(reinterpret_cast<uintptr_t>(&actor.Object));
    BeginMagicPinFrame();
    if(!enabled) { assert(!gMagicFrameOwned && !gMagicClockStarted);RunMagicPinFrame();assert(calls==0);std::puts("PASS disabled opt-in");return 0; }
    assert(gMagicFrameOwned && MagicPinFrameOwnsActor(actor.Object));
    OBJECT other; assert(!MagicPinFrameOwnsActor(other));
    other.Type=kMagicPinRootModel; assert(MagicPinFrameOwnsModel(other));
    other.Type=999; assert(!MagicPinFrameOwnsModel(other));
    RunMagicPinFrame(); assert(calls==0); EndMagicPinFrame();
    assert(!MagicPinFrameOwnsParticles());
    now=311; BeginMagicPinFrame(); RunMagicPinFrame();
    assert(calls==1 && gMagicCompleted==5 && gMagicClock.PendingMilliseconds()==11);
    RunMagicPinFrame(); assert(calls==1); EndMagicPinFrame();
    actor.Object.CurrentAction=11; Effects[0].Type=kMagicPinRootModel;
    now=351; BeginMagicPinFrame();
    assert(!MagicPinFrameOwnsActor(actor.Object) && MagicPinFrameOwnsParticles());
    RunMagicPinFrame(); assert(calls==2); EndMagicPinFrame();
    limit=1; now=471; BeginMagicPinFrame(); RunMagicPinFrame();
    assert(gMagicClock.PendingMilliseconds()==91); EndMagicPinFrame();
    Effects[0].Live=false; now=511; BeginMagicPinFrame();
    assert(!gMagicFrameOwned && !gMagicClockStarted && gMagicClock.Due()==0);
    RunMagicPinFrame(); assert(calls==3); EndMagicPinFrame();
    actor.Object.Live=false; BeginMagicPinFrame();
    assert(!gMagicFrameOwned && !gMagicClockStarted && gMagicClock.Due()==0);
    actor.Object.Live=true; SceneFlag=0; BeginMagicPinFrame(); assert(!gMagicFrameOwned);
    SceneFlag=MAIN_SCENE; ResetAllEffectAnimationSamples(); BeginMagicPinFrame();assert(!gMagicFrameOwned);
    gAnimationActorIdentity.Bind(reinterpret_cast<uintptr_t>(&actor.Object));
    actor.Object.CurrentAction=287;
    BeginMagicPinFrame(); now+=80; EndMagicPinFrame(); BeginMagicPinFrame();
    resetDuringBatch=true; RunMagicPinFrame();
    assert(!gMagicFrameOwned && !gMagicClockStarted && gMagicClock.PendingMilliseconds()==0);
    EndMagicPinFrame();
    std::puts("PASS actual frame functions: zero/multiple ticks, duplicate run, post-cast live-model continuation, unrelated-action guard, partial batch, dead/map/identity reset");
}
'''
build=root/'ExMain_RISE_PC/Tests/GrowLancerBuild'
for old,new in (('actor','fixtureActor'),('enabled','fixtureEnabled'),('now','fixtureNow')):
    prefix=re.sub(r'\b'+old+r'\b',new,prefix)
    tests=re.sub(r'\b'+old+r'\b',new,tests)
cpp=build/'MagicFrameTransitions.cpp'; cpp.write_text(prefix+reset+frame+tests,encoding='utf-8')
vc=Path('C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231')
sdk=Path('C:/Program Files (x86)/Windows Kits/10')
exe=build/'MagicFrameTransitions.exe'
subprocess.run([str(vc/'bin/Hostx64/x86/cl.exe'),'/nologo','/std:c++14','/EHsc','/MT','/W4','/WX',
    '/I'+str(vc/'include'),'/I'+str(sdk/'Include/10.0.26100.0/ucrt'),str(cpp),
    '/Fo'+str(build/'MagicFrameTransitions.obj'),'/Fe'+str(exe),'/link',
    '/LIBPATH:'+str(vc/'lib/x86'),'/LIBPATH:'+str(sdk/'Lib/10.0.26100.0/ucrt/x86'),
    '/LIBPATH:'+str(sdk/'Lib/10.0.26100.0/um/x86')],check=True)
subprocess.run([str(exe)],check=True)
subprocess.run([str(exe),'disabled'],check=True)
print('Scope: actual frame control flow, controlled clock/quantum callback; not native pool/animation QA')
