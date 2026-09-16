"""Pinned read-only S21 lance item/model -> native QA item-record comparison."""
from __future__ import annotations

import hashlib
import json
import struct
import xml.etree.ElementTree as ET
from pathlib import Path

from inspect_breche_item_models import checksum
from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_s21_payload, inspect_plaintext, parse_s21_container,
)
from inspect_magic_pin_model_geometry import mesh_bounds

S21 = Path(r'D:\MU FICA Season 21')
S21_BMD = S21 / 'Data/Local/item.bmd'
S21_BMD_SHA = '9B9D62B7F3BA085D31B4AEFBCC9DE5DB505C48D45C336A18CAD424C93D3B02F6'
S21_XML = Path(r'D:\GameServer S21\Data\Items\ItemList.xml')
S21_XML_SHA = '61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78'
ROOT = Path(__file__).resolve().parents[2]
QA_ITEM = ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/RISE/Config/Item.bmd'
QA_SHA = 'C5A5693D6C94F07A65ED49E02AB598644458B38ADA7CA5B92B429DAE0D52AECA'
SELECTED = {13, 14, 15, 16, 17, 18, 19}
KEY = b'\xfc\xcf\xab'
MODEL_SHA = {
    13: 'DD9EAFCBF7A94F81420D6B19D7F21BF2E552DD2395FD150A7108F6B5FAFFCFC0',
    14: 'D7B2041215B4DA028592E1C1D1E3E4D6E20985BE32A457C5AD5238C97022AF0E',
    15: 'CC870A5E63B73E717E680990F584B4A78AD74781E59FF594A54A53C01DAD3BB4',
    16: '565959656576C41A09D4E0E5FADABBC6DEDC1514D2B1DA1BFF24CF5487B1A0C9',
    17: '78165113CB337AA9D9E6023C0C334377EFF1B99674578F24CBA380837D9C42E2',
    18: '5438775C63C2B7F0D27EB4600893E6DE8DC8B553A40D5E2BD8896679D8DD1D81',
    19: '79BB698A30E679EF0A19138C3686D1D4B23E9B3E323B5ED4BF2175ADA57047C1',
}


def decode(encoded: bytes) -> bytes:
    return bytes(value ^ KEY[offset % 3] for offset, value in enumerate(encoded))


def check_sha(path: Path, digest: str) -> bytes:
    raw = path.read_bytes()
    assert hashlib.sha256(raw).hexdigest().upper() == digest, str(path)
    return raw


def main() -> None:
    source = check_sha(S21_BMD, S21_BMD_SHA)
    count = struct.unpack_from('<I', source)[0]
    assert count == 4950 and len(source) == 8 + count * 708
    assert checksum(source[4:-4]) == struct.unpack_from('<I', source, len(source)-4)[0]
    qa = check_sha(QA_ITEM, QA_SHA)
    assert len(qa) == 8192 * 84 + 4
    assert checksum(qa[:-4]) == struct.unpack_from('<I', qa, len(qa)-4)[0]
    xml_raw = check_sha(S21_XML, S21_XML_SHA)
    catalog = ET.fromstring(xml_raw)
    section = next(row for row in catalog.findall('Section')
                   if row.attrib.get('Index') == '3')
    xml_rows = {int(row.attrib['Index']): row for row in section.findall('Item')}

    by_type: dict[int, tuple[int, bytes]] = {}
    for ordinal in range(count):
        record = decode(source[4 + ordinal*708:4 + (ordinal+1)*708])
        item_type = struct.unpack_from('<I', record)[0]
        if item_type in {3*512 + index for index in SELECTED}:
            assert item_type not in by_type
            by_type[item_type] = (ordinal, record)

    results = []
    for index in sorted(SELECTED):
        item_type = 3 * 512 + index
        row = xml_rows[index]
        stage = int(row.attrib['GrowLancer'])
        assert stage in (1, 3, 4, 5)
        assert item_type in by_type, f'missing S21 item model registry {item_type}'
        ordinal, model_record = by_type[item_type]
        directory = model_record[8:268].split(b'\0', 1)[0].decode('ascii')
        filename = model_record[268:528].split(b'\0', 1)[0].decode('ascii')
        model_path = (S21 / directory / filename).resolve()
        assert model_path.is_relative_to(S21.resolve())
        model_data = model_path.read_bytes() if model_path.is_file() else None
        assert model_data and hashlib.sha256(model_data).hexdigest().upper() == MODEL_SHA[index]
        plaintext = decrypt_s21_payload(parse_s21_container(model_data))
        model_header = inspect_plaintext(plaintext)
        meshes, mesh_end = mesh_bounds(plaintext, model_header['meshCount'])
        assert mesh_end < len(plaintext)
        native = decode(qa[item_type*84:(item_type+1)*84])
        native_name = native[:30].split(b'\0', 1)[0].decode('cp949')
        results.append({
            'item': f'3:{index}', 'nativeType': item_type,
            's21Name': row.attrib['Name'], 's21GrowLancerStage': stage,
            's21Slot': int(row.attrib['Slot']),
            's21ReqStrength': int(row.attrib['ReqStrength']),
            's21ReqDexterity': int(row.attrib['ReqDexterity']),
            's21ModelOrdinal': ordinal, 's21ModelSource': str(model_path),
            's21ModelSha256': hashlib.sha256(model_data).hexdigest().upper()
                if model_data else None,
            's21ModelHeaderHex': model_data[:4].hex() if model_data else None,
            's21ModelName': model_header['name'],
            's21MeshCount': model_header['meshCount'],
            's21BoneCount': model_header['boneCount'],
            's21ActionCount': model_header['actionCount'],
            's21MeshTextures': [mesh['texture'] for mesh in meshes],
            'qaName': native_name, 'qaSlot': native[34],
            'qaWidth': native[38], 'qaHeight': native[39],
            'qaReqStrength': struct.unpack_from('<H', native, 50)[0],
            'qaReqDexterity': struct.unpack_from('<H', native, 52)[0],
            'qaLegacyClassColumns': list(native[69:76]),
        })
    assert results[0]['s21Name'] == 'Lapid Lance'
    assert results[0]['qaName'] == '' and results[0]['qaWidth'] == 0
    print(json.dumps({
        'scope': 'source/item-record provenance; no conversion, staging or ingame parity',
        's21ItemBmdSha256': S21_BMD_SHA, 's21XmlSha256': S21_XML_SHA,
        'qaItemBmdSha256': QA_SHA, 's21RecordCount': count,
        'nativeQaRecordCount': 8192, 'items': results,
    }, indent=2))


if __name__ == '__main__':
    main()
