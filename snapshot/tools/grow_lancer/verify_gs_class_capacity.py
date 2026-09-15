"""Bounded GS class-7 capacity and SS6 seven-column data regression.

This is a source/build prerequisite, not class create/login/cast acceptance.
"""
import hashlib
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
gs = ROOT / 'ExGameServer/GameServer'
s21 = Path(r'D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua')
assert hashlib.sha256(s21.read_bytes()).hexdigest() == (
    '9da5000449ccec0b3da0cd2ade11e55863b5cc0c56ec4063c5a64f78414c0a70'
)
assert re.search(rb'CLASS_GROWLANCER\s*=\s*7\b', s21.read_bytes())

for name in ('DefaultClassInfo.h', 'CustomStartItem.h', 'CustomStartItemDame.h'):
    text = (gs / name).read_text()
    assert re.search(r'^#define MAX_CLASS 8$', text, re.M), name
assert '#define CLASS_GL 7' in (gs / 'DefaultClassInfo.h').read_text()

legacy = (ROOT / 'Shared/LegacySkillClassColumns.h').read_text()
assert 'static_assert(Capacity >= 7' in legacy
assert 'for (unsigned int i = 0; i < Capacity; ++i)' in legacy
assert 'for (unsigned int i = 0; i < 7; ++i)' in legacy
for name in ('ItemManager.cpp', 'SkillManager.cpp', 'MasterSkillTree.cpp', 'Quest.cpp'):
    assert 'LegacySkillClassColumns.h' in (gs / name).read_text(), name

qa_defaults = (ROOT / 'ExGameServer/Tests/GrowLancerBuild/ServerStackSmoke/'
               '4.MuServer/Sub-1/Data/Character/DefaultClassInfo.txt').read_text()
assert not re.search(r'^\s*7\s+', qa_defaults, re.M)
assert 'DB_CLASS_GL' not in (gs / 'ObjectManager.cpp').read_text()
assert 'DB_CLASS_GL' not in (gs / 'Protocol.cpp').read_text()

print('PASS class7 source identity; all three GS class capacities8')
print('PASS legacy seven-column SS6 read contract; QA defaults have no unverified row7')
print('OPEN class7 defaults/create/login/packet/equipment/cast; capacity is not activation')
