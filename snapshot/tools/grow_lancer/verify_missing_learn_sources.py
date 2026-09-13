"""Scoped negative/config evidence. No inferred scroll or class conversion."""
from pathlib import Path
import hashlib
import re
import xml.etree.ElementTree as ET
from collections import Counter
root = Path(r'D:\GameServer S21\Data')
pins = {
 'ClassQuest.xml':'5b2ce593af8694f55061baefd95cc566e9cd4aaa2491cb0bb9819ff87201d6ac',
 'Warps/ClassQuest.xml':'20787d902a8b58208804e1989f6ee50ac301e689ccf167511964c16a448724f3',
 'Skills/SkillRequire.xml':'4e6b765ff7dcfb62e58ad9720f5e8c8e81dc8ecc5ba30c3bc3b842b390c9ac58',
 'Items/ItemList.xml':'61711d9e6b0e6f14028191aa739aad4a3b0c5911cdb3468a3bc61b988b12af78',
}
for name, pin in pins.items():
    data = (root/name).read_bytes()
    assert hashlib.sha256(data).hexdigest() == pin
    # Supplied comments contain invalid XML double hyphens; strip only in memory.
    tree = ET.fromstring(re.sub(r'<!--.*?-->', '', data.decode('utf-8-sig'), flags=re.S))
    if name == 'Items/ItemList.xml':
        assert not any(x.attrib.get('SkillIndex') in ('272','275') for x in tree.iter('Item'))
        skills = Counter(x.attrib.get('SkillIndex') for x in tree.iter('Item')
                         if x.attrib.get('GrowLancer') == '1' and x.attrib.get('SkillIndex','0') != '0')
        assert skills == {'20':10,'18':8}
        print('LEAD eligible GL item skills18(8 rows),20(10 rows); no direct272/275 Item SkillIndex')
    else:
        assert not any(v in ('272','275') for x in tree.iter() for v in x.attrib.values())
        print('SCOPED ABSENCE', name, 'no active attribute value272/275')
print('OPEN automatic grant/equipment/class/siege conversion; config absence is not proof skill cannot be learned')
