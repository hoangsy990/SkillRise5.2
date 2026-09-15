"""S21 base7 -> native client identity, without guessed evolution names.

This is source/packet algebra evidence only; no class7 login or body PASS.
"""
import hashlib
import re
from pathlib import Path

root = Path(__file__).resolve().parents[2]
s21 = Path(r'D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua')
assert hashlib.sha256(s21.read_bytes()).hexdigest() == (
    '9da5000449ccec0b3da0cd2ade11e55863b5cc0c56ec4063c5a64f78414c0a70'
)
assert re.search(rb'CLASS_GROWLANCER\s*=\s*7\b', s21.read_bytes())

src = root / 'ExMain_RISE_PC/Main5.2_RISE'
backup = root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/EncodingBackup/CharacterManager.cpp.cp949'
assert hashlib.sha256(backup.read_bytes()).hexdigest() == (
    '0c6d473aa862c3fe77ed9d0474ab81b9b0b917a5cd769ffa0b05a996010df76b'
)
original = backup.read_bytes().decode('cp949')
current = (src / 'CharacterManager.cpp').read_text(encoding='utf-8')
assert '레이지파이터' in current and '템플나이트' in current
assert '레이지파이터' in original and '템플나이트' in original

identity = (src / 'RISE/GrowLancerClassIdentity.h').read_text()
manager = (src / 'CharacterManager.cpp').read_text(encoding='utf-8')
header = (src / 'CharacterManager.h').read_text()
enum = (src / '_enum.h').read_bytes().decode('cp949')
assert re.search(r'CLASS_TEMPLENIGHT,\s*\};', enum)
assert 'kGrowLancerSourceBaseClass = 7' in identity
assert 'kGrowLancerNativeClassType = 18' in identity
assert 'kUnmappedGrowLancerEvolutionClassType = 0xff' in identity
assert 'CLASS_TEMPLENIGHT + 1 == rise::growlancer::kGrowLancerNativeClassType' in manager
assert 'case rise::growlancer::kGrowLancerSourceBaseClass:' in manager
assert 'byCharacterClass = (bySecondClass || byThirdClass) ?' in manager
assert 'rise::growlancer::kUnmappedGrowLancerEvolutionClassType :' in manager
assert 'rise::growlancer::kGrowLancerNativeClassType;' in manager
assert 'return "Grow Lancer";' in manager
assert '== rise::growlancer::kGrowLancerSourceBaseClass' in header

for stage in range(4):
    wire = (112 + stage) % 16 * 16
    wire -= wire // 32
    wire += 7 * 32
    if wire <= 255:
        client = (((wire >> 4) & 1) << 3) | (wire >> 5) | (((wire >> 3) & 1) << 4)
        assert client & 7 == 7
        assert (client == 7) == (stage == 0)

print('PASS pinned S21 class7 identity and byte-safe native unevolved wire224 -> client base7')
print('PASS native class selector18 appended; unsupported evolutions do not resolve to Wizard')
print('OPEN true class7 create/login/body/evolutions/defaults/skills and live QA')
