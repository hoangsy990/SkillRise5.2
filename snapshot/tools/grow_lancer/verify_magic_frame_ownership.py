"""Frame wiring audit, not native runtime/visual acceptance."""
from pathlib import Path
root=Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE'
runtime=(root/'RISE/GrowLancerEffectRuntime.cpp').read_text()
scene=(root/'ZzzScene.cpp').read_text(encoding='latin-1')
game=scene.split('void MoveMainScene()',1)[1].split('bool RenderMainScene()',1)[0]
calls=['MoveHero();','BeginMagicPinFrame();','MoveCharactersClient();',
       'RunMagicPinFrame();','MoveEffects();','MoveJoints();','MoveParticles();','EndMagicPinFrame();']
positions=[game.index(c) for c in calls]
assert positions==sorted(positions)
for call in ('BeginMagicPinFrame();','RunMagicPinFrame();','EndMagicPinFrame();'):
    assert scene.count(call)==1
begin=runtime.split('void BeginMagicPinFrame()',1)[1].split('bool MagicPinFrameOwnsActor',1)[0]
assert '#ifdef RISE_GROW_LANCER_RUNTIME_QA' in begin
assert 'GetEnvironmentVariableA("RISE_GL_MAGIC_FIXED_TICK_QA", value, 2) == 1' in begin
assert "value[0] == '1'" in begin and 'if (!enabled) return;' in begin
for token in ('!Hero','!Hero->Object.Live','SceneFlag != MAIN_SCENE',
              'gAnimationActorIdentity.Matches','gMagicClock.Start(now)',
              'gMagicClock.Observe(now)',
              'Hero->Object.CurrentAction != 287 && !hasLiveMagicModel()',
              'effect.Live && (effect.Type == kMagicPinControllerModel'):
    assert token in begin,token
run=runtime.split('void RunMagicPinFrame()',1)[1].split('void EndMagicPinFrame()',1)[0]
assert 'if (!gMagicFrameOwned || gMagicFrameRan || !Hero) return;' in run
assert run.index('gMagicFrameRan = true;') < run.index('RunMagicPinQuanta(')
assert 'gMagicClock.Consume(completed)' in run and 'completed != ticks' in run
reset=runtime.split('void ResetAllEffectAnimationSamples()',1)[1].split('void RetireCharacterAnimationSamples',1)[0]
for token in ('gMagicClock.Reset();','gMagicClockStarted = false;',
              'gMagicFrameOwned = false;','gMagicFrameRan = false;'):
    assert token in reset
effects=(root/'ZzzEffect.cpp').read_text(encoding='latin-1')
assert 'void MoveEffect(OBJECT* o, int iIndex)\n{\n    if (rise::growlancer::MagicPinFrameOwnsModel(*o)) return;' in effects
particles=(root/'ZzzEffectParticle.cpp').read_text(encoding='utf-8')
private=particles.split('void rise::growlancer::StepMagicPinParticles()',1)[1].split('int rise::growlancer::CreateBrecheFireParticle',1)[0]
guard='if (!g_pOption->GetRenderAllEffects() || !g_pOption->GetRenderSkillEffects())'
assert guard in particles.split('void MoveParticles()',1)[1]
assert private.index(guard) < private.index('StepOrderedParticlePhase(')
assert 'if (rise::growlancer::MagicPinFrameOwnsParticles() &&\n                rise::growlancer::IsMagicPinParticleSlot(i)) continue;' in particles
end=runtime.split('void EndMagicPinFrame()',1)[1].split('unsigned RunMagicPinQuanta',1)[0]
assert end.index('RecordMagicFrameQA(') < end.index('gMagicFrameOwned = false;')
print('PASS explicit QA opt-in, scoped main-scene begin/run/end and legacy exclusions')
print('PASS completed-time accounting, duplicate Run guard and actor/map reset wiring')
print('OPEN live zero/multi-tick, action handoff, movement/pose parity, pool stress and visual QA')
