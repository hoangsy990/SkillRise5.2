"""Pin the native GS item-section boundaries before importing S21 lances."""
from __future__ import annotations

import hashlib
import shlex
from pathlib import Path

from inspect_gl_lance_item_chain import S21_XML, S21_XML_SHA, SELECTED

ROOT = Path(__file__).resolve().parents[2]
NATIVE = ROOT / ('ExGameServer/Tests/GrowLancerBuild/ServerStackSmoke/'
                 '4.MuServer/Sub-1/Data/Item/Item.txt')
NATIVE_SHA = '85E92BB41D2D0B9959B91DC646F25258D6C45B708E49D35EB5EB9A6BB7EEDE68'
GROUP4_SANITY = {
    13: 'Bluewing Crossbow',
    14: 'Aquagold Crossbow',
    15: 'Arrow',
    16: 'Saint Crossbow',
    17: 'Celestial Bow',
    # Source row omits whitespace after the closing quote before level100.
    18: 'Divine Crossbow of Archangel100',
    19: 'Great Reign Crossbow',
}


def main() -> None:
    assert hashlib.sha256(S21_XML.read_bytes()).hexdigest().upper() == S21_XML_SHA
    assert hashlib.sha256(NATIVE.read_bytes()).hexdigest().upper() == NATIVE_SHA
    section = None
    names3: dict[int, str] = {}
    names4: dict[int, str] = {}
    with NATIVE.open('r', encoding='cp949', errors='replace') as rows:
        for row in rows:
            content = row.split('//', 1)[0].strip()
            if not content:
                continue
            if content == 'end':
                section = None
                continue
            if content.isdecimal() and section is None:
                section = int(content)
                continue
            if section in (3, 4):
                fields = shlex.split(content)
                index = int(fields[0])
                if index in SELECTED:
                    names = names3 if section == 3 else names4
                    assert index not in names, f'duplicate GS item index {section}:{index}'
                    names[index] = fields[8]
    assert not names3, f'GS group3 lance ID collision: {names3}'
    assert names4 == GROUP4_SANITY, f'GS group4 section sanity changed: {names4}'
    print('PASS source_S21_group3_13_to_19=lances '
          'native_GS_group3_13_to_19=unoccupied '
          'native_GS_group4_13_to_19=seven_preexisting_bow_arrow_items '
          'same_id_import_requires_metadata_and_authorization')


if __name__ == '__main__':
    main()
