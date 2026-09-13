"""Read-only table naming evidence; does not establish S21 packet translation."""
from pathlib import Path
import hashlib
import re
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r'D:\GameServer S21\Data\BuffEffectManager.xml')
raw = SOURCE.read_bytes()
assert hashlib.sha256(raw).hexdigest() == '201452c00d9d5ce0db83820ef48ec4317a6654864c0be9176a6389694a4ad6b0'
# Supplied file has an XML-invalid triple-hyphen comment at line 414.
# Remove comments in memory only; preserve every element/attribute and source.
without_comments = re.sub(rb'<!--.*?-->', b'', raw, flags=re.S)
rows = ET.fromstring(without_comments).findall('./General/Buff')
expected = {56: 'Ice', 141: 'Reduce attack speed', 146: 'Freeze',
            158: 'Frost', 240: 'Paralysis', 274: 'Chilling',
            278: 'Freezing', 86: 'Cold', 61: 'Stun', 72: 'Sleep',
            176: 'Tornado', 178: 'Darkness', 280: 'Stun'}
for index, name in expected.items():
    matches = [row for row in rows if int(row.attrib['Index']) == index]
    assert len(matches) == 1
    assert matches[0].attrib['Name'] == name
    assert matches[0].attrib['Type'] == '1'
    if index == 176:
        assert matches[0].attrib['Description'] == 'Attack speed is reduced to half.'
        assert matches[0].attrib['EffectType'] == '100'
    print(f'TABLE 0x{index:X} / {index}: {name}; EffectType={matches[0].attrib["EffectType"]}')

# Parse only the simple enum prefix through explicit value 86, rejecting
# expressions/directives instead of silently counting commented-out entries.
source = (ROOT/'ExMain_RISE_PC/Main5.2_RISE/_enum.h').read_bytes()
prefix = source.split(b'enum eBuffState', 1)[1].split(b'{', 1)[1]
prefix = prefix.split(b'eBuff_Seal_HpRecovery', 1)[0].decode('ascii')
prefix = re.sub(r'/\*.*?\*/|//[^\n]*', '', prefix, flags=re.S)
values = {}
value = -1
for entry in prefix.split(','):
    if not entry.strip():
        continue
    match = re.fullmatch(r'\s*(\w+)\s*(?:=\s*(\d+))?\s*', entry)
    assert match, entry
    value = int(match[2]) if match[2] else value + 1
    values[match[1]] = value
assert values['eDeBuff_Freeze'] == 56
assert values['eDeBuff_BlowOfDestruction'] == 86
assert values['eDeBuff_Stun'] == 61
assert values['eDeBuff_Sleep'] == 72
print('PASS pinned S21 General/Buff names; native enum Freeze=56, BlowOfDestruction=86')
print('PASS native enum Stun=61 and Sleep=72; matching names/numbers are not full delivery proof')
print('OPEN client status ingestion/translation for additional S21 states, including Darkness178 and Stun280')
