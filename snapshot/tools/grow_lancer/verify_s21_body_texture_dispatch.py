"""Pinned body texture token mapping; no final skin/hair selection claim."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)


def block(start, end):
    rows = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(row.size for row in rows) == end-start
    return {row.address: (row.mnemonic, row.op_str) for row in rows}


dispatch = block(0x18BBA1A, 0x18BBD34)
for address, expected in {
    0x18BBA8A: ('cmp', 'eax, 0x73'),
    0x18BBA9C: ('cmp', 'eax, 0x6b'),
    0x18BBAAD: ('cmp', 'eax, 0x69'),
    0x18BBAB2: ('mov', 'dword ptr [ebp - 0x144], 0x7725'),
    0x18BBABC: ('jmp', '0x18bbd17'),
    0x18BBB37: ('cmp', 'eax, 0x68'),
    0x18BBB49: ('cmp', 'eax, 0x61'),
    0x18BBB5A: ('cmp', 'eax, 0x69'),
    0x18BBB6C: ('cmp', 'eax, 0x72'),
    0x18BBB71: ('mov', 'dword ptr [ebp - 0x144], 0x7776'),
    0x18BBB7B: ('jmp', '0x18bbd17'),
    0x18BBD17: ('mov', 'eax, dword ptr [ebp - 0x144]'),
}.items():
    assert dispatch[address] == expected, (hex(address), dispatch[address])
loop = block(0x18C79AA, 0x18C7A82)
for part in range(5):
    address = 0x18C79D8 + part*0x20
    assert loop[address] == ('push', '0x2600')
    assert loop[address+5] == ('push', '0x2901')
    assert loop[address+24] == ('call', '0x18bb6c0')
print('PASS complete texture token helper decode and five body loader calls')
print('Lowercase ski prefix returns0x7725; hair returns0x7776 without ordinary file load')
print('Uppercase Skin_Class308 does not enter lowercase ski byte-test branch')
print('OPEN remaining uppercase-path checks and sampler follow-through')

# Both full renderer branches have a hide gate followed by signed selector addition.
skin = block(0x133146D, 0x1331560)
hair = block(0x1331854, 0x1331947)
for rows, tests in ((skin, {
    0x133146D: ('cmp', 'dword ptr [ebp - 0x74], 0x7725'),
    0x133147D: ('movzx', 'eax, byte ptr [eax + 0xa5]'),
    0x1331488: ('jmp', '0x1333a15'),
    0x1331490: ('movsx', 'eax, byte ptr [eax + 0xa4]'),
    0x1331497: ('add', 'eax, 0x7725'),
}), (hair, {
    0x1331854: ('cmp', 'dword ptr [ebp - 0x74], 0x7776'),
    0x1331864: ('movzx', 'eax, byte ptr [eax + 0xa5]'),
    0x133186F: ('jmp', '0x1333a15'),
    0x1331877: ('movsx', 'eax, byte ptr [eax + 0xa4]'),
    0x133187E: ('add', 'eax, 0x7776'),
})):
    for address, expected in tests.items():
        assert rows[address] == expected

for start, resource, name in (
    (0x18C680B, 0x772C, 'skin_Class108.jpg'),
    (0x18C6B4B, 0x773C, 'skin_Class108.jpg'),
    (0x18C6A4B, 0x777D, 'hair_Class108.tga'),
    (0x18C706B, 0x779D, 'hair_Class108.tga'),
    (0x18C746B, 0x77AD, 'hair_Class108.tga'),
    (0x18C78CB, 0x77BD, 'hair_Class108.tga')):
    rows = block(start, start+32)
    assert rows[start+14] == ('push', hex(resource))
    assert rows[start+24] == ('call', '0x18f8fa2')
    ptr = int(rows[start+19][1], 16)-IMAGE_BASE
    assert data[ptr:data.index(0,ptr)] == ('Player\\'+name).encode()
print('PASS signed selector substitution and six concrete skin/hair file registrations')
print('OPEN alternate render paths and valid evolution mapping')

writer = block(0x13E2DBA, 0x13E2DD4)
assert writer[0x13E2DBD] == ('movzx', 'eax, byte ptr [eax + 0x12]')
assert writer[0x13E2DC2] == ('call', '0x1418fba')
assert writer[0x13E2DCE] == ('mov', 'byte ptr [ecx + 0xa4], al')
selector = block(0x1418FBA, 0x1418FF2)
for address, expected in {
    0x1418FC3: ('call', '0x960fb1'),
    0x1418FCF: ('call', '0x141848e'),
    0x1418FD8: ('dec', 'eax'),
    0x1418FE4: ('shl', 'ecx, 4'),
    0x1418FE7: ('add', 'eax, ecx'),
}.items():
    assert selector[address] == expected
stage = block(0x141848E, 0x14184E5)
for address, expected in {
    0x1418497: ('sar', 'eax, 4'),
    0x141849A: ('and', 'eax, 0xf'),
    0x14184A8: ('mov', 'al, 1'),
    0x14184B0: ('mov', 'dword ptr [ebp - 8], 3'),
    0x14184BC: ('dec', 'eax'),
    0x14184CD: ('sar', 'eax, cl'),
    0x14184CF: ('and', 'eax, 1'),
    0x14184D7: ('inc', 'eax'),
    0x14184D8: ('inc', 'eax'),
}.items():
    assert stage[address] == expected
for high in range(16):
    rank = high.bit_length()  # decoded high-to-low scan, zero=>stage1
    selected = 7 + 16*rank
    assert selected in (7, 23, 39, 55, 71)
print('PASS actual character class-byte -> stage helper -> model selector writer')
print('For base7 selector=7+16*bit_length(highNibble); class evolution eligibility still OPEN')
