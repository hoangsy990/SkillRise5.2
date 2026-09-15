"""Source guards only: Circle native JOINT ownership, not live pool QA."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
native = (ROOT / 'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectJoint.cpp').read_text(encoding='utf-8')
create = native.split('void CreateJoint(', 1)[1].split('void DeleteJoint(', 1)[0]
delete = native.split('void DeleteJoint(', 1)[1].split('bool SearchJoint(', 1)[0]
move = native.split('void MoveJoint(', 1)[1].split('void MoveJoints()', 1)[0]
contact = move.split('if (o->Type == BITMAP_FORCEPILLAR && o->SubType == 3)', 1)[1].split('if (o->Type == BITMAP_FORCEPILLAR && o->SubType == 2)', 1)[0]
loop = native.split('void MoveJoints()', 1)[1].split('void RenderJoints(', 1)[0]
assert 'for (int i = 0; i < MAX_JOINTS; i++)' in create
assert create.index('if (!o->Live)') < create.index('g_circleJointRemainder[i] = 0.f;') < create.index('o->Live = true;')
assert create.index('o->Target = NULL;') < create.index('o->Target = Target;')
assert 'o->Live && o->Type == Type && o->Target == Target' in delete
assert 'SubType == -1 || o->SubType == SubType' in delete
assert 'o->Live = false;' in delete
assert contact.index('if (!o->Target || !o->Target->Live)') < contact.index('VectorCopy(o->Target->Position')
assert 'o->Live = false;\n            return;' in contact
assert 'g_circleJointRemainder[iIndex], FPS_ANIMATION_FACTOR' in contact
assert contact.rstrip().endswith('return;\n    }')
assert 'if (o->Live)' in loop and 'MoveJoint(o, i);' in loop
assert 'EffectDestructor' not in contact + delete
characters = (ROOT / 'ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp').read_text(encoding='utf-8')
initialize = characters.split('void CreateCharacterPointer(', 1)[1].split('CHARACTER* CreateCharacter(', 1)[0]
assert 'if (!o->Live)' in initialize
assert 'DeleteJoint(BITMAP_FORCEPILLAR, o, 3);' in initialize
assert initialize.index('if (!o->Live)') < initialize.index('DeleteJoint(BITMAP_FORCEPILLAR, o, 3);') < initialize.index('o->Live = true;')
assert initialize.count('DeleteJoint(') == 1
assert initialize.index('rise::growlancer::RetireCircleCharacterEffects(*o);') < initialize.index('o->Live = true;')
runtime = (ROOT/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8')
retire = runtime.split('void RetireCircleCharacterEffects(',1)[1].split('void CreateCircleShieldRoot(',1)[0]
assert 'if (retiredCharacter.Live)\n        return;' in retire
assert 'DeleteEffect(kCircleShieldControllerModel, &retiredCharacter, -1);' in retire
assert retire.count('DeleteEffect(') == 1
print('PASS: native fixed-pool allocation/reset, target assignment, deletion filter, live guard and isolated update return.')
print('PASS: dead character slot reuse retires only Circle contact subtype3 before reactivation; live-key refresh unchanged.')
print('Scope: source guards only; other ownership/replacement paths, runtime reuse, exhaustion and GPU acceptance remain open.')
