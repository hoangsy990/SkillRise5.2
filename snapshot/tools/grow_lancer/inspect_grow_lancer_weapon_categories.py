"""Cross-check supplied GS equipment eligibility against original client catalog."""
import hashlib
import struct
import xml.etree.ElementTree as ET
from collections import Counter
from pathlib import Path
import inspect_breche_item_models as items


def main():
    server = Path(r'D:\GameServer S21\Data\Items\ItemList.xml').read_bytes()
    assert hashlib.sha256(server).hexdigest() == '61711d9e6b0e6f14028191aa739aad4a3b0c5911cdb3468a3bc61b988b12af78'
    data = items.TABLE.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6'
    count = struct.unpack_from('<I', data)[0]
    assert len(data) == 8 + count * 708
    assert items.checksum(data[4:-4]) == struct.unpack_from('<I', data, len(data)-4)[0]
    rows = {}
    for j in range(count):
        row = bytes(v ^ b'\xfc\xcf\xab'[i % 3] for i, v in enumerate(data[4+j*708:4+(j+1)*708]))
        key = struct.unpack_from('<I', row)[0]
        assert key not in rows
        rows[key] = row
    eligible = []
    for section in ET.fromstring(server).findall('Section'):
        group = int(section.attrib['Index'])
        if group not in range(7):
            continue
        for item in section.findall('Item'):
            if int(item.get('GrowLancer', '0')) <= 0:
                continue
            index = int(item.attrib['Index'])
            row = rows[group*512+index]
            category = row[0x251]
            assert category == int(item.attrib['KindB']), (group, index)
            eligible.append((group, index, category, item.attrib['Name']))
    assert eligible, 'no eligible weapons found'
    assert Counter((g,c) for g,i,c,n in eligible) == {
        (0,0): 1, (0,1): 3, (1,4): 2, (3,7): 23, (6,15): 24}
    assert not any(c in (89,90,114) for g,i,c,n in eligible)
    print('PASS GS GrowLancer-positive weapon/shield rows match client KindB')
    print('Counts(group,category)=', sorted(Counter((g,c) for g,i,c,n in eligible).items()))
    print('SpecialReactionCategories=', [(g,i,c,n) for g,i,c,n in eligible if c in (89,90,114)])
    print('Examples=', eligible[:5])
    print('Scope: supplied catalog eligibility, not equipped slot assignment or authoritative cast validation')


if __name__ == '__main__':
    main()
