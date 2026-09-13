"""Pin reaction sound selectors without activating an incomplete native class."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)

def block(begin, end):
    return list(decoder.disasm(data[begin-IMAGE_BASE:end-IMAGE_BASE], begin))

sex = block(0xBB6224, 0xBB629C)
assert (sex[-1].address, sex[-1].mnemonic) == (0xBB629B, 'ret')
assert [i.op_str for i in sex if i.mnemonic == 'cmp'] == [
    'eax, 2', 'eax, 5', 'eax, 7', 'eax, 8', 'eax, 9', 'eax, 0xc', 'eax, 0xe']
assert all(i.op_str == '0x960fb1' for i in sex if i.mnemonic == 'call')
assert all(i.op_str == '0xbb6290' for i in sex if i.mnemonic == 'je')
instructions = {i.address: (i.mnemonic, i.op_str) for start, end in (
    (0x960FB1, 0x960FBC), (0xBB6224, 0xBB629C),
    (0x140AD10, 0x140ADCE), (0x1314FAE, 0x1314FC1)) for i in block(start, end)}
for address, expected in {
    0x960FB7: ('and', 'eax, 0xf'),
    0xBB6290: ('mov', 'dword ptr [ebp - 4], 1'),
    0x140AD18: ('call', '0xbb6224'),
    0x140AD20: ('jne', '0x140ad9a'),
    0x140ADA5: ('cmp', 'eax, 0x1a25'),
    0x140ADAA: ('je', '0x140adce'),
    0x140ADC2: ('add', 'eax, 0x6d'),
    0x140ADC6: ('call', '0xa74f94'),
    0x1314FB8: ('movzx', 'eax, word ptr [eax + 0x322e]'),
}.items():
    assert instructions[address] == expected, hex(address)
root = Path(__file__).resolve().parents[2]
native = (root / 'ExMain_RISE_PC/Main5.2_RISE/CharacterManager.h').read_text(encoding='latin1')
assert 'CLASS_ELF || this->GetBaseClass(iClass) == CLASS_SUMMONER' in native
print('PASS S21 low-nibble class7 selects female reaction branch; native IsFemale does not yet include new class')
print('PASS female sound109/110 selector, with source field322E==1A25 suppression')
registration = {i.address: (i.mnemonic, i.op_str) for i in block(0x18E015B, 0x18E01D3)}
for address, expected in {
    0x18E015B: ('push', '0x1bb3c74'), 0x18E017D: ('push', '0x6d'),
    0x18E017F: ('call', '0xa74e69'),
    0x18E0197: ('push', '0x1bb3c94'), 0x18E01B9: ('push', '0x6e'),
    0x18E01BB: ('call', '0xa74e69'),
}.items():
    assert registration[address] == expected
for index, address, expected_hash in (
    (1, 0x1BB3C74, '469133E1F77907F321BF385E19E93A608D1FC88E51630C77C35AB7D1990AE701'),
    (2, 0x1BB3C94, 'EAE7D56D04DEC5417BB98CAE3A246F801549D9CFD5928AFE89ED0DA14656AB06'),
):
    name = f'pFemaleScream{index}.wav'
    literal = ('Data\\Sound\\' + name).encode() + b'\0'
    assert data[address-IMAGE_BASE:address-IMAGE_BASE+len(literal)] == literal
    for base in (Path(r'D:\MU FICA Season 21'),
                 root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client'):
        asset = (base / 'Data/Sound' / name).read_bytes()
        assert asset[:4] == b'RIFF' and asset[8:12] == b'WAVE'
        assert hashlib.sha256(asset).hexdigest().upper() == expected_hash
print('PASS registration109/110 filename pins and identical SHA256 WAV assets in S21 and isolated QA')
import inspect_breche_item_models as items
table = items.TABLE.read_bytes()
assert hashlib.sha256(table).hexdigest() == '9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6'
count = struct.unpack_from('<I', table)[0]
assert len(table) == 8 + count * 708
assert items.checksum(table[4:-4]) == struct.unpack_from('<I', table, len(table)-4)[0]
matches = []
for ordinal in range(count):
    encoded = table[4+ordinal*708:4+(ordinal+1)*708]
    row = bytes(v ^ b'\xfc\xcf\xab'[i % 3] for i, v in enumerate(encoded))
    if struct.unpack_from('<I', row)[0] == 0x1A25:
        matches.append(row)
assert len(matches) == 1
assert struct.unpack_from('<hh', matches[0], 4) == (13, 37)
assert matches[0][528:578].split(b'\0')[0] == b'Horn of Fenrir'
assert matches[0][268:528].split(b'\0')[0] == b'fr_6.bmd'
mount = {i.address: (i.mnemonic, i.op_str) for i in block(0x140AAA6, 0x140AB46)}
for address, expected in {
    0x140AAC3: ('call', '0x10c352e'),
    0x140AAEF: ('je', '0x140ab41'),
    0x140AB02: ('call', '0x10c64d8'),
    0x140AB07: ('cmp', 'eax, 0x1a25'),
    0x140AB1B: ('jne', '0x140ab41'),
    0x140AB33: ('add', 'eax, 0x2bc'),
    0x140AB39: ('call', '0xa74f94'),
}.items():
    assert mount[address] == expected
print('PASS suppression identity1A25 is item13/37 Horn of Fenrir; guarded auxiliary branch selects sound700/701')
print('OPEN complete auxiliary activation predicate, native class integration and live acceptance')
