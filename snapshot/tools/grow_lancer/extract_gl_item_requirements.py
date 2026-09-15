"""Read-only, hash-pinned S21 ItemList -> private native-capacity descriptor.

--patch prints an apply_patch payload; this script never writes any file.
--verify checks the generated descriptor against every relevant XML row.
"""
from __future__ import annotations

import argparse
import hashlib
import re
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r'D:\GameServer S21\Data\Items\ItemList.xml')
SOURCE_SHA = '61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78'
DEST = ROOT / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerItemRequirements.generated.h'
GROUP_COUNT = 16  # exact native MAX_ITEM_TYPE, not S21 item-count capacity
INDEX_COUNT = 512  # exact native MAX_ITEM_INDEX
VALUES = {0, 1, 3, 4, 5}


def extract() -> tuple[list[tuple[int, int]], dict[int, int], int]:
    raw = SOURCE.read_bytes()
    assert hashlib.sha256(raw).hexdigest().upper() == SOURCE_SHA
    tree = ET.fromstring(raw)
    assert tree.tag == 'ItemList'
    seen: set[int] = set()
    rows: list[tuple[int, int]] = []
    counts: dict[int, int] = {value: 0 for value in VALUES}
    excluded = 0
    for section in tree.findall('Section'):
        group = int(section.attrib['Index'])
        assert group >= 0
        for item in section.findall('Item'):
            index = int(item.attrib['Index'])
            assert 0 <= index < INDEX_COUNT
            item_type = group * INDEX_COUNT + index
            value_text = item.attrib.get('GrowLancer')
            if group >= GROUP_COUNT:
                if value_text and int(value_text) != 0:
                    excluded += 1
                continue
            assert item_type not in seen, f'duplicate native-capacity S21 item {group}:{index}'
            seen.add(item_type)
            if value_text is None:
                continue  # absent source rule is not a client entitlement
            value = int(value_text)
            assert value in VALUES, f'unrecognized evolution value {value}'
            counts[value] += 1
            if value:
                rows.append((item_type, value))
    rows.sort()
    assert rows and counts[1] > 0 and excluded > 0
    return rows, counts, excluded


def header(rows: list[tuple[int, int]], counts: dict[int, int], excluded: int) -> str:
    lines = [
        '#pragma once', '#include <cstdint>', '',
        'namespace rise { namespace growlancer {',
        '// Generated strictly from S21 ItemList.xml SHA-256 ' + SOURCE_SHA,
        '// Source values 1/3/4/5 are preserved; evolution mapping remains separate.',
        '// Native ItemAttribute has only 16*512 slots; ' + str(excluded) +
        ' eligible S21 items in newer groups are excluded, not aliased.',
        'struct GrowLancerItemRequirement { std::uint16_t type; std::uint8_t stage; };',
        'static const GrowLancerItemRequirement kGrowLancerItemRequirements[] = {',
    ]
    lines.extend('    {%d, %d},' % row for row in rows)
    lines += [
        '};',
        'static const unsigned kGrowLancerItemRequirementCount = ' + str(len(rows)) + ';',
        'static const unsigned kGrowLancerNativeItemCapacity = 8192;',
        'inline std::uint8_t LookupGrowLancerItemRequirement(unsigned itemType)',
        '{',
        '    if (itemType >= kGrowLancerNativeItemCapacity) return 0;',
        '    unsigned first = 0, last = kGrowLancerItemRequirementCount;',
        '    while (first < last)',
        '    {',
        '        const unsigned mid = first + (last - first) / 2;',
        '        if (kGrowLancerItemRequirements[mid].type < itemType) first = mid + 1;',
        '        else last = mid;',
        '    }',
        '    return first < kGrowLancerItemRequirementCount &&',
        '        kGrowLancerItemRequirements[first].type == itemType ?',
        '        kGrowLancerItemRequirements[first].stage : 0;',
        '}',
        '} }', '',
    ]
    assert len(rows) == sum(count for value, count in counts.items() if value)
    return '\n'.join(lines)


def verify(rows: list[tuple[int, int]], counts: dict[int, int], excluded: int) -> None:
    actual = DEST.read_text(encoding='utf-8').replace('\r\n', '\n')
    expected = header(rows, counts, excluded)
    assert actual == expected
    parsed = [(int(a), int(b)) for a, b in re.findall(r'^    \{(\d+), (\d+)\},$', actual, re.M)]
    assert parsed == rows
    print(f'PASS S21 XML SHA and native 16*512 descriptor: {len(rows)} nonzero rows, '
          f'stage1={counts[1]}, stage3={counts[3]}, stage4={counts[4]}, '
          f'stage5={counts[5]}, newer-group-excluded={excluded}')
    print('OPEN S21 new item groups, evolution-stage ABI, model/slot/stat parity, GS item gate')


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--patch', action='store_true')
    parser.add_argument('--verify', action='store_true')
    args = parser.parse_args()
    assert args.patch != args.verify
    rows, counts, excluded = extract()
    if args.verify:
        verify(rows, counts, excluded)
        return
    assert not DEST.exists(), 'generated descriptor already exists; no overwrite'
    payload = header(rows, counts, excluded)
    print('*** Begin Patch')
    print('*** Add File: ' + str(DEST))
    for line in payload.splitlines():
        print('+' + line)
    print('*** End Patch')


if __name__ == '__main__':
    main()
