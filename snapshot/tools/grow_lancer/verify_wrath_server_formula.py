"""Wrath source expressions, not verified GS application/rounding or duration."""
from pathlib import Path
import hashlib
import xml.etree.ElementTree as ET

base = Path(r'D:\GameServer S21\Data')
raw = (base/'FormulaData.xml').read_bytes()
assert hashlib.sha256(raw).hexdigest() == '9a513dd4375116cfd0fa599114b2103b4d21b194b775ae376e0c6289ba36e6ff'
tree = ET.fromstring(raw)
# IDs repeat across sections. Never select .//Formula by ID alone.
expected = {44: '(%d+%d)*0+10', 46: '(%d+%d)/3',
            47: '%d*(((%d+%d)*0+0.05)*-1)'}
for index, expression in expected.items():
    nodes = tree.findall(f'./Character/Formula[@ID="{index}"]')
    assert len(nodes) == 1 and nodes[0].get('Data') == expression
    assert len(tree.findall(f'.//Formula[@ID="{index}"]')) > 1
    print(f'PASS Character/{index}: {expression}')
raw = (base/'Skills/SkillList.xml').read_bytes()
assert hashlib.sha256(raw).hexdigest() == '3e238c786ecab3445a0db4756fe3d2a3923fbc0594506bb9c3ff206020a7e0a0'
tree = ET.fromstring(raw)
for skill, buff in ((278, 424), (895, 425)):
    nodes = tree.findall(f'./Skill[@Index="{skill}"]')
    assert len(nodes) == 1
    row = nodes[0]
    assert row.get('BuffIndex') == str(buff)
    assert row.get('Duration') == '0'
    print(f'PASS skill {skill} -> buff {buff}, table Duration=0 (meaning unresolved)')
print('OPEN duration/lifecycle implementation, formula call sites, units and integer rounding')

raw = (base/'BuffEffectManager.xml').read_bytes()
assert hashlib.sha256(raw).hexdigest() == '201452c00d9d5ce0db83820ef48ec4317a6654864c0be9176a6389694a4ad6b0'
# This supplied file has malformed unrelated XML at line414. Parse only the
# exact single-line records under audit, without repairing or rewriting source.
def buff_rows(buff):
    marker = f'<Buff Index="{buff}"'.encode('ascii')
    return [ET.fromstring(line.strip()) for line in raw.splitlines() if marker in line]

for buff, name in ((424, 'Wrath'), (425, 'Wrath Strengthener')):
    rows = buff_rows(buff)
    assert len(rows) == 1
    row = rows[0]
    for key, value in {
        'EffectType': '123', 'ItemType': '-1', 'ItemIndex': '-1',
        'Type': '0', 'NoticeType': '1', 'ClearType': '1',
        'BuffOutputValue': '0', 'Name': name,
        'Description': 'Attack and combat power increase. Defense decreases.',
    }.items():
        assert row.get(key) == value, (buff, key, row.get(key))
for buff in (218, 223, 224):
    rows = buff_rows(buff)
    assert len(rows) == 1 and rows[0].get('EffectType') == '123'
    assert rows[0].get('Description') == 'Decreases Defense by 30%, increases Damage by 50%'
print('PASS: exact424/425 rows pinned; same EffectType123 also labels legacy218/223/224 with different descriptions.')
print('Do not identify buff behavior by EffectType or copy legacy50%/30% descriptions into new Wrath rules.')
