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
