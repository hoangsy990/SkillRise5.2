"""Audit native lifecycle sites required before sample sidecar integration."""
from pathlib import Path

root = Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE'
effect = (root/'ZzzEffect.cpp').read_text(encoding='latin-1')
destructor = effect.split('void EffectDestructor(OBJECT* o)',1)[1].split('void TerminateOwnerEffectObject',1)[0]
assert 'o->Live = false;' in destructor and 'o->Owner = NULL;' in destructor
allocation = effect.split('void CreateEffect(int Type,',1)[1].split('float Matrix[3][4];',1)[0]
for token in ('OBJECT* o = &Effects[icntEffect];', 'o = g_SkillEffects.CreateEffect();',
              'if (!o->Live)', 'o->Live = true;', 'o->Type = Type;',
              'if (!resourceReady)', 'o->Live = false;',
              'rise::growlancer::InitializeEffect(*o);'):
    assert token in allocation, token
assert allocation.index('o->Live = true;') < allocation.index('if (!resourceReady)') < allocation.index('rise::growlancer::InitializeEffect(*o);')
maps = (root/'MapManager.cpp').read_text(encoding='latin-1')
clear = maps.split('void CMapManager::DeleteObjects()',1)[1].split('bool CMapManager::InChaosCastle',1)[0]
assert 'Effects[i].Live = false;' in clear
assert 'EffectDestructor(' not in clear
assert 'rise::growlancer::ResetEffectAnimationSamples(o);' in destructor
assert allocation.index('o = g_SkillEffects.CreateEffect();') < allocation.index('rise::growlancer::ResetEffectAnimationSamples(o);') < allocation.index('o->Live = true;')
assert 'rise::growlancer::ResetAllEffectAnimationSamples();' in clear
print('PASS native allocation selects primary or secondary object BEFORE Live/type assignment')
print('PASS resource failure can return before imported InitializeEffect; reset only there is insufficient')
print('PASS map DeleteObjects directly clears primary Live flags without EffectDestructor')
print('Required: reset on allocation after actual slot selection, destruction and map reset; do not index secondary object with primary loop counter')
print('PASS private cursor reset hooks cover actual allocation, destructor and map reset')
character = (root/'ZzzCharacter.cpp').read_text(encoding='latin-1')
for signature, end in (
    ('void DeleteCharacter(int Key)', 'void DeleteCharacter(CHARACTER* c, OBJECT* o)'),
    ('void DeleteCharacter(CHARACTER* c, OBJECT* o)', 'int FindCharacterIndex(int Key)'),
    ('void CreateCharacterPointer(CHARACTER* c, int Type, unsigned char PositionX, unsigned char PositionY, float Rotation)', 'c->PositionX = PositionX;')):
    body = character.split(signature,1)[1].split(end,1)[0]
    assert 'rise::growlancer::RetireCharacterAnimationSamples(o);' in body
print('PASS character deletion overloads and reconstruction invalidate matching private actor identity')
print('OPEN actor generation, producer/consumer and runtime lifecycle; no visual PASS')
