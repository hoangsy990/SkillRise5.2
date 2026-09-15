"""Check factor-only native character refactor against repository baseline."""
from pathlib import Path
import subprocess
import re
root = Path(__file__).resolve().parents[2]
relative = 'ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp'
current = (root/relative).read_text(encoding='latin-1')
baseline = subprocess.run(['git','show','HEAD:'+relative],cwd=root,capture_output=True,check=True).stdout.decode('latin-1').replace('\r\n','\n')
def body(text, signature, following):
    part = text.split(signature,1)[1].split(following,1)[0]
    return part[part.index('{'):].strip()
old = body(baseline,'bool CharacterAnimation(CHARACTER* c, OBJECT* o)','int GetHandOfWeapon')
expected = old.replace('FPS_ANIMATION_FACTOR','stepFactor').replace('b->PlayAnimation(', 'b->PlayAnimationStep(').replace('PlaySpeed, o->Position, o->Angle);','PlaySpeed, o->Position, o->Angle, stepFactor);')
actual = body(current,'bool CharacterAnimationStep(CHARACTER* c, OBJECT* o, float stepFactor)','int GetHandOfWeapon')
# RuntimeQA has diagnostic-only blocks around the native call and the
# stun/sleep early-return. Strip those blocks before comparing the production
# algorithm to the repository baseline; they must not be allowed to mask a
# non-QA behavior change.
actual_production = re.sub(r'#ifdef RISE_GROW_LANCER_RUNTIME_QA.*?#endif', '', actual, flags=re.S)
normalize_blanks = lambda value: '\n'.join(line for line in value.splitlines() if line.strip())
assert normalize_blanks(actual_production) == normalize_blanks(expected), 'Additional algorithm change in character animation'
assert 'return CharacterAnimationStep(c, o, FPS_ANIMATION_FACTOR);' in current
old_end = body(baseline,'void AnimationCharacter(CHARACTER* c, OBJECT* o, BMD* b)','void CreateWeaponBlur')
new_end = body(current,'void AnimationCharacterStep(CHARACTER* c, OBJECT* o, BMD* b, float stepFactor)','void AnimationCharacter(')
new_end_production = re.sub(r'#ifdef RISE_GROW_LANCER_RUNTIME_QA.*?#endif', '', new_end, flags=re.S)
assert normalize_blanks(new_end_production) == normalize_blanks(old_end.replace('CharacterAnimation(c, o);','CharacterAnimationStep(c, o, stepFactor);'))
assert 'AnimationCharacterStep(c, o, b, FPS_ANIMATION_FACTOR);' in current
assert 'FPS_ANIMATION_FACTOR' not in actual
print('PASS native speed modifiers, stun/sleep and action-end bodies unchanged except explicit factor handoff')
print('PASS old public CharacterAnimation signature and ordinary scene wrapper retain global-factor calls')
print('OPEN private fixed-tick scheduling and child lifecycle interleaving; no visual PASS')
