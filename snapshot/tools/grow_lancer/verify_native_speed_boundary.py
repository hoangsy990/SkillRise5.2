"""Prove native speed already includes ability flags; not a parity certificate."""
from pathlib import Path
import re

root=Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE'
source=(root/'ZzzInfomation.cpp').read_text(encoding='latin-1')
body=source.split('void CHARACTER_MACHINE::CalculateAttackSpeed()',1)[1].split(
    'void CHARACTER_MACHINE::CalculateSuccessfulBlockingPK()',1)[0]
defines=(root/'_define.h').read_text(encoding='latin-1')
for name,value in [('ABILITY_FAST_ATTACK_SPEED','01'),('ABILITY_FAST_ATTACK_SPEED2','08')]:
    assert re.search(r'#define\s+'+name+r'\s+\(\s*0x'+value+r'\s*\)',defines)
pattern=r'''if\s*\(Character.Ability & ABILITY_FAST_ATTACK_SPEED\)\s*\{
\s*Character.AttackSpeed \+= 20;
\s*Character.MagicSpeed \+= 20;
\s*\}\s*else if\s*\(Character.Ability & ABILITY_FAST_ATTACK_SPEED2\)\s*\{
\s*Character.AttackSpeed \+= 20;
\s*Character.MagicSpeed \+= 20;
\s*\}'''
assert re.search(pattern,body)
for stat in ('AttackSpeed','MagicSpeed'):
    assert f'Character.{stat} += g_SocketItemMgr.m_StatusBonus.m_iAttackSpeedBonus;' in body
for buff in ('eBuff_EliteScroll1','eBuff_Hellowin1','eBuff_QuickOfSanta'):
    assert f'g_isCharacterBuff((&Hero->Object), {buff})' in body
character=(root/'ZzzCharacter.cpp').read_text(encoding='latin-1')
setter=character.split('void SetAttackSpeed()',1)[1].split('void ',1)[0]
assert re.search(r'AttackSpeed1\s*=\s*CharacterAttribute->AttackSpeed\s*\*\s*0.004f',setter)
print('PASS native calculated speeds already include flag01/08 +20 once, item/buff/socket additions')
print('PASS native action-speed setter consumes calculated Attribute speed; do NOT add source flag bonus again')
print('OPEN source field writer equivalence and native receive/recalculation ordering; this check does not authorize a universal subtraction or cap')
ws=(root/'WSclient.cpp').read_text(encoding='latin-1')
join=ws.split('BOOL ReceiveJoinMapServer(',1)[1].split('\nBOOL ',1)[0]
assert join.index('CharacterAttribute->AttackSpeed') < join.index('SetCharacterClass(c);')
class_body=character.split('void SetCharacterClass(',1)[1].split('void SetChangeClass(',1)[0]
assert 'CharacterMachine->CalculateAll();' in class_body
helper=ws.split('BOOL ReceiveHelperItem(',1)[1].split('void ReceiveWeather(',1)[0]
assert helper.count('CharacterMachine->CalculateAttackSpeed();')==2
print('PASS join speed assignment precedes SetCharacterClass ->CalculateAll; helper buff reception also recalculates speed')
qa=(root/'RISE/GrowLancerRuntimeQA.cpp').read_text(encoding='utf-8')
assert '#ifdef RISE_GROW_LANCER_RUNTIME_QA' in qa
assert 'WriteCastInputSnapshot("before-dispatch"' in qa
assert 'WriteCastInputSnapshot("after-dispatch"' in qa
snapshot=qa.split('void WriteCastInputSnapshot(',1)[1].split('void WritePoolSnapshot(',1)[0]
assert 'CalculateAttackSpeed(' not in snapshot and 'CalculateAll(' not in snapshot
assert '.PlaySpeed' not in snapshot
print('PASS QA-only before/after input observations do not recalculate stats or read uninitialized action speeds')
