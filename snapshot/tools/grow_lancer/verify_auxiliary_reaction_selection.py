"""Pin fallback/virtual action6 selection, not full mounted-state activation."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
instructions = {}
for start, end in ((0x10C3B1A, 0x10C3BEB), (0x108AC04, 0x108AC77),
                   (0x108AF90, 0x108B078)):
    block = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert block[-1].mnemonic == 'ret'
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in block})
for address, expected in {
    0x10C3B2A: ('call', '0x10c30b1'),
    0x10C3BAB: ('call', '0x108ac04'),
    0x10C3BBF: ('call', 'dword ptr [eax + 0x2c]'),
    0x10C3BD1: ('call', '0x108ac77'),
    0x10C3BE5: ('call', 'dword ptr [eax + 0x30]'),
    0x108AC34: ('cmp', 'dword ptr [ebp - 4], 6'),
    0x108AC38: ('je', '0x108ac66'),
    0x108AC66: ('push', '0x14f'),
    0x108AC6E: ('call', '0x1327d72'),
    0x108AFA3: ('mov', 'dword ptr [ebp - 4], 0x153'),
    0x108AFC2: ('mov', 'dword ptr [ebp - 4], 0x154'),
    0x108AFE1: ('mov', 'dword ptr [ebp - 4], 0x155'),
    0x108B01A: ('mov', 'dword ptr [ebp - 4], 0x152'),
    0x108B03B: ('mov', 'dword ptr [ebp - 4], 0x150'),
    0x108B05C: ('mov', 'dword ptr [ebp - 4], 0x151'),
    0x108B06F: ('call', '0x1327d72'),
}.items():
    assert instructions[address] == expected, hex(address)
print('PASS action6 fallback335 and equipment-dependent336..341 selector pins')
print('PASS registered auxiliary record uses virtual slots2C/30 instead of fallback')
for start, end in ((0x10C48AC, 0x10C48E5), (0x10C49F9, 0x10C4A55),
                   (0x10C59E2, 0x10C59FF), (0x108A781, 0x108A7B7)):
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
for address, expected in {
    0x10C48C6: ('cmp', 'dword ptr [ebp - 0x14], 0x1a25'),
    0x10C48CD: ('je', '0x10c49f9'),
    0x10C49F9: ('push', '0x50'),
    0x10C4A23: ('call', '0x10c59e2'),
    0x10C59F4: ('mov', 'dword ptr [eax], 0x1b876e4'),
    0x108A78E: ('push', 'dword ptr [eax + 0x14]'),
    0x108A791: ('call', '0x108ac04'),
    0x108A799: ('ret', '4'),
    0x108A7A9: ('push', 'dword ptr [eax + 0x14]'),
    0x108A7AC: ('call', '0x108ac77'),
    0x108A7B4: ('ret', '4'),
}.items():
    assert instructions[address] == expected, hex(address)
assert struct.unpack_from('<II', data, 0x1B876E4 + 0x2C - IMAGE_BASE) == (0x108A781, 0x108A79C)
print('PASS item1A25 constructor10C59E2 installs vtable1B876E4; slots2C/30 wrap the same action selectors')
for start, end in ((0x131501E, 0x131507B), (0x13150A8, 0x13150D5)):
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
for address, expected in {
    0x1315037: ('imul', 'eax, dword ptr [ebp + 8], 0x3fc'),
    0x1315041: ('movsx', 'eax, word ptr [ecx + eax + 0x19ac]'),
    0x1315049: ('sub', 'eax, 0xae9'),
    0x131505B: ('call', '0xd06184'),
    0x1315070: ('movzx', 'eax, byte ptr [eax + 0x251]'),
    0x13150B6: ('call', '0x131501e'),
    0x13150BB: ('cmp', 'eax, dword ptr [ebp + 0xc]'),
}.items():
    assert instructions[address] == expected
import inspect_breche_item_models as items
catalog = items.TABLE.read_bytes()
assert hashlib.sha256(catalog).hexdigest() == '9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6'
count = struct.unpack_from('<I', catalog)[0]
assert len(catalog) == 8 + count * 708
assert items.checksum(catalog[4:-4]) == struct.unpack_from('<I', catalog, len(catalog)-4)[0]
categories = {89: [], 90: [], 114: []}
for j in range(count):
    row = bytes(v ^ b'\xfc\xcf\xab'[i % 3] for i, v in enumerate(catalog[4+j*708:4+(j+1)*708]))
    if row[0x251] in categories:
        categories[row[0x251]].append((struct.unpack_from('<I', row)[0], row[528:578].split(b'\0')[0]))
assert {k: len(v) for k, v in categories.items()} == {89: 19, 90: 20, 114: 19}
assert all(item // 512 == 5 for rows in categories.values() for item, name in rows)
assert categories[89][0][1] == b'[Bound] Elemental Magic Book'
assert categories[90][0][1] == b'[Bound] Blue Moon Orb'
assert categories[114][0][1] == b'Elixir of Performance'
print('PASS weapon helper converts model to catalog key and compares KindB byte251; categories89/90/114 are group5 book/orb/elixir records')
for start, end in ((0x10C2FD4, 0x10C30B1), (0x108A124, 0x108A184),
                   (0x1089CD3, 0x1089FE5)):
    block = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert block[-1].mnemonic == 'ret'
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in block})
for address, expected in {
    0x10C3025: ('call', '0x10c464b'),
    0x10C303B: ('call', 'dword ptr [eax + 4]'),
    0x108A140: ('and', 'dword ptr [eax + 0x14], 0'),
    0x10C3043: ('push', 'dword ptr [ebp + 0x10]'),
    0x10C3049: ('call', '0x1089cd3'),
    0x1089D0F: ('mov', 'dword ptr [eax + 0x14], ecx'),
    0x1089D15: ('add', 'eax, 0x3254'),
    0x1089D1D: ('mov', 'dword ptr [ecx + 0x18], eax'),
    0x1089D68: ('mov', 'dword ptr [eax + 0x1c], ecx'),
    0x1089D98: ('mov', 'dword ptr [eax + 0x34c], ecx'),
    0x10C3053: ('je', '0x10c307f'),
    0x10C305C: ('add', 'eax, 0x3e'),
    0x10C3077: ('call', '0x10c57b2'),
    0x10C309B: ('call', 'dword ptr [eax]'),
}.items():
    assert instructions[address] == expected, hex(address)
assert struct.unpack_from('<I', data, 0x1B876E4 + 4 - IMAGE_BASE)[0] == 0x108A124
print('PASS manager resets record, initializes character14/object18 and child1C owner, then inserts by character key; init-failure virtual cleanup exists')
for start, end in ((0x13F7126, 0x13F716A), (0x176BA3C, 0x176BA57),
                   (0x131510E, 0x1315133), (0x961566, 0x961576)):
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
for address, expected in {
    0x13F7138: ('call', '0x176ba3c'),
    0x13F7145: ('movzx', 'eax, word ptr [eax*2 + 0xb7aaf60]'),
    0x13F714D: ('and', 'eax, 1'),
    0x13F7152: ('push', '1'),
    0x13F7157: ('call', '0x131510e'),
    0x13F715F: ('push', '0'),
    0x13F7164: ('call', '0x131510e'),
    0x176BA42: ('and', 'eax, 0xff'),
    0x176BA47: ('shl', 'eax, 8'),
    0x176BA4D: ('and', 'ecx, 0xff'),
    0x131512C: ('mov', 'byte ptr [eax + 4], cl'),
    0x961570: ('movzx', 'eax, byte ptr [eax + 4]'),
    0x10C3B80: ('call', '0x961566'),
    0x10C3B8A: ('je', '0x10c3bc5'),
}.items():
    assert instructions[address] == expected, hex(address)
print('PASS state byte4 gate is updated from wrapped terrain-cell bit1; parallels native SafeZone update')
for start, end in ((0x9F71B0, 0x9F71D1), (0x12F1352, 0x12F135C),
                   (0x12522E6, 0x1252324), (0x1253AC1, 0x1253AF7)):
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
for address, expected in {
    0x9F71B9: ('call', '0x961566'),
    0x9F71C1: ('push', 'eax'),
    0x9F71C8: ('add', 'ecx, 0xc'),
    0x9F71CB: ('call', '0x131510e'),
    0x12F1352: ('push', '1'),
    0x12F1357: ('call', '0x131510e'),
    0x12522F8: ('cmp', 'dword ptr [eax + 0x3ac4], 0x82'),
    0x125230C: ('call', '0x131510e'),
    0x125231C: ('call', '0x131510e'),
    0x1253AE4: ('call', '0x131510e'),
    0x1253AF1: ('call', '0x131510e'),
}.items():
    assert instructions[address] == expected, hex(address)
print('PASS state is also copied/explicitly assigned; do not replace stored character state with terrain-only recomputation')
print('OPEN remaining writer semantics, full teardown and mounted gameplay; no speculative native clip activation')
