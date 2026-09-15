"""Pin source Lua GL fist/magic expressions without guessing GS int rounding."""
import hashlib
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = Path(r'D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua')
raw = source.read_bytes()
assert hashlib.sha256(raw).hexdigest() == (
    '9da5000449ccec0b3da0cd2ade11e55863b5cc0c56ec4063c5a64f78414c0a70'
)
lua = raw.decode('utf-8')
fist = lua.split('function GrowLancerDamageCalc(', 1)[1].split('\nend', 1)[0]
magic = lua.split('function GrowLancerMagicDamageCalc(', 1)[1].split('\nend', 1)[0]
for token in ('AttackDamageMinLeft = Strength / 8 + Dexterity / 10',
              'AttackDamageMinRight = Strength / 8 + Dexterity / 10',
              'AttackDamageMaxLeft = Strength / 4 + Dexterity / 6',
              'AttackDamageMaxRight = Strength / 4 + Dexterity / 6'):
    assert token in fist
for token in ('MagicDamageMin = Energy / 9',
              'MagicDamageMax = Energy / 4'):
    assert token in magic

adapter = (root / 'ExGameServer/GameServer/RISE/GrowLancerSourceDamage.h').read_text()
assert adapter.count('strength / 8.0 + dexterity / 10.0') == 2
assert adapter.count('strength / 4.0 + dexterity / 6.0') == 2
assert 'energy / 9.0' in adapter and 'energy / 4.0' in adapter
assert 'double physicalMinLeft' in adapter and 'double magicMax' in adapter
assert 'ComputeSourceDamage(int strength, int dexterity, int energy)' in adapter
print('PASS pinned S21 GL Lua fist/magic terms -> fractional native source adapter')
print('OPEN protected GS integer conversion, item bonuses, class7 login and skill damage connection')
