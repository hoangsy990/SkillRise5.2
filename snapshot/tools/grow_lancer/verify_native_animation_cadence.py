"""Verify native clock expressions and quantify their units, not live FPS."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE'
ai = (root / 'ZzzAI.cpp').read_text(encoding='latin-1')
header = (root / 'ZzzAI.h').read_text(encoding='latin-1')
clock = ai.split('void CalcFPS()', 1)[1].split('\n}', 1)[0]
assert 'constexpr double REFERENCE_FPS = 25.0;' in header
for token in ('const double differenceMs = WorldTime - last;',
              'FPS = 1000 / differenceMs;',
              'minf(static_cast<float>(REFERENCE_FPS / FPS), 2.5f)'):
    assert token in clock
bmd = (root / 'ZzzBMD.cpp').read_text(encoding='latin-1')
assert 'Speed, Origin, Angle, FPS_ANIMATION_FACTOR);' in bmd
assert '*AnimationFrame += Speed * stepFactor;' in bmd
effect = (root / 'ZzzEffect.cpp').read_text(encoding='latin-1')
move = effect.split('void MoveEffect(OBJECT* o, int iIndex)', 1)[1]
assert 'o->Type == rise::growlancer::kMagicPinControllerModel' in move
assert 'rise::growlancer::AdvanceWholeTicks(*o, FPS_ANIMATION_FACTOR,' in move

for elapsed, expected in ((10, .25), (20, .5), (40, 1.), (80, 2.),
                          (100, 2.5), (200, 2.5)):
    factor = min(25. / (1000. / elapsed), 2.5)
    assert factor == expected
    print(f'elapsedMs={elapsed} nativeFactor={factor:g} representedMs={factor*40:g}')
print('PASS native units are already 25Hz/40ms; no extra 25/30 conversion justified')
print('At elapsed>100ms native factor clamps; fractional owner stepping and whole-effect sampling remain distinct')
print('OPEN actual scene call multiplicity/order, source overload behavior and live cadence; no runtime timing change')

# Check the selected ordinary gameplay source path. Token checks do not prove
# preprocessor variants, auxiliary callbacks or observed runtime call counts.
scene = (root / 'ZzzScene.cpp').read_text(encoding='latin-1')
main = scene.split('void MainScene(HDC hDC)', 1)[1]
assert main.index('CalcFPS();') < main.index('MoveMainScene();')
game = scene.split('void MoveMainScene()', 1)[1].split('bool RenderMainScene()', 1)[0]
assert game.count('MoveCharactersClient();') == 1
assert game.count('MoveEffects();') == 1
assert game.index('MoveHero();') < game.index('MoveCharactersClient();') < game.index('MoveEffects();')
character = (root / 'ZzzCharacter.cpp').read_text(encoding='latin-1')
traversal = character.split('void MoveCharactersClient()', 1)[1].split('extern float  ParentMatrix', 1)[0]
assert traversal.count('MoveCharacterClient(&CharactersClient[i]);') == 1
client = character.split('void MoveCharacterClient(CHARACTER* cc)', 1)[1].split('void MoveCharactersClient()', 1)[0]
assert client.index('MoveCharacter(cc, co);') < client.index('MoveCharacterVisual(cc, co);')
actor = character.split('void MoveCharacter(CHARACTER* c, OBJECT* o)', 1)[1].split('void MoveCharacterVisual(', 1)[0]
assert actor.count('AnimationCharacter(c, o, b);') == 1
animation = character.split('void AnimationCharacterStep(CHARACTER* c, OBJECT* o, BMD* b, float stepFactor)', 1)[1].split('void AnimationCharacter(', 1)[0]
assert animation.count('bool Play = CharacterAnimationStep(c, o, stepFactor);') == 1
assert 'AnimationCharacterStep(c, o, b, FPS_ANIMATION_FACTOR);' in character
pool = effect.split('void MoveEffects()', 1)[1].split('#if TEST_SKILL_CHEM_XOAY', 1)[0]
assert 'if (o->Live)' in pool and pool.count('MoveEffect(o, i);') == 1
print('PASS selected native gameplay source path: clock -> character animation traversal -> effect traversal')
print('Both engines order characters before effects; whole-effect catch-up still samples the already-advanced owner frame')
