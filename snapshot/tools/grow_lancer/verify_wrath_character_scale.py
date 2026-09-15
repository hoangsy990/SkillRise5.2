"""Pin S21 Wrath scale override, not packet/lifecycle completeness."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
instructions = list(decoder.disasm(data[0x141043E-IMAGE_BASE:0x14108C2-IMAGE_BASE], 0x141043E))
assert instructions[0].address == 0x141043E
assert instructions[-1].address == 0x14108C1 and instructions[-1].mnemonic == 'ret'
assert sum(i.size for i in instructions) == 0x484
decoded = {i.address: (i.mnemonic, i.op_str) for i in instructions}
expected = {
    0x141058F: ('call', '0x9dfb65'),
    0x141059A: ('je', '0x1410634'),
    0x14106C3: ('call', '0x9b4504'),
    0x14106C8: ('cmp', 'eax, 5'),
    0x14106CB: ('jne', '0x14106e5'),
    0x141044D: ('je', '0x1410454'),
    0x141044F: ('jmp', '0x14108c0'),
    0x141062F: ('jmp', '0x14108c0'),
    0x14106E0: ('jmp', '0x14108c0'),
    0x1410879: ('push', '0x1a8'),
    0x1410881: ('add', 'ecx, 0x35ac'),
    0x1410887: ('call', '0x130c8a5'),
    0x1410891: ('jne', '0x14108ad'),
    0x1410893: ('push', '0x1a9'),
    0x14108A1: ('call', '0x130c8a5'),
    0x14108AB: ('je', '0x14108c0'),
    0x14108B0: ('movss', 'xmm0, dword ptr [0x1b58d70]'),
    0x14108B8: ('movss', 'dword ptr [eax + 0x32f4], xmm0'),
}
for address, instruction in expected.items():
    assert decoded[address] == instruction, (hex(address), decoded[address])
assert 0x32F4 - 0x3254 == 0xA0
assert data[0x1B58D70-IMAGE_BASE:0x1B58D74-IMAGE_BASE] == struct.pack('<f', 1.1)
print('PASS: complete linear decode and selected branch pins; buff424/425 assigns character OBJECT+A0=1.1f.')
print('Early exits bypass override. Caller lifecycle, buff removal and native scale integration remain OPEN.')
for start, end, call in [(0x140FD3F, 0x141043E, 0x1410412),
                         (0x1410942, 0x1410C10, 0x1410C08)]:
    block = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in block) == end-start
    assert block[-1].mnemonic == 'ret'
    calls = {i.address: (i.mnemonic, i.op_str) for i in block}
    assert calls[call] == ('call', '0x141043e')
print('PASS: two complete caller linear decodes contain scale refresh; not proof of buff-remove dispatch.')
def pinned_block(start, end):
    block = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in block) == end-start
    assert block[-1].mnemonic == 'ret'
    return {i.address: (i.mnemonic, i.op_str) for i in block}
remove = pinned_block(0x1305838, 0x13058C4)
assert remove[0x1305885] == ('call', '0x1285a3a')
assert remove[0x1305894] == ('call', '0x1284d00')
assert remove[0x13058AF] == ('call', '0x130c769')
assert remove[0x13058BB] == ('call', '0x128558e')
physical = pinned_block(0x1285A3A, 0x1286488)
for address, instruction in {
    0x1285D99: ('cmp', 'dword ptr [ebp - 0x10], 0x1a8'),
    0x1285DA0: ('je', '0x12861ee'),
    0x1285DA6: ('cmp', 'dword ptr [ebp - 0x10], 0x1a9'),
    0x1285DAD: ('je', '0x12861ee'),
    0x12861F1: ('movss', 'xmm0, dword ptr [0x1b4eda4]'),
    0x12861F9: ('movss', 'dword ptr [eax + 0xa0], xmm0'),
    0x1286209: ('call', '0x14b65d8'),
}.items():
    assert physical[address] == instruction
assert data[0x1B4EDA4-IMAGE_BASE:0x1B4EDA8-IMAGE_BASE] == struct.pack('<f', 0.9)
print('PASS: Wrath physical removal sets absolute0.9 before membership removal; no native buff activation implied.')
activation = pinned_block(0x12FD717, 0x12FD882)
assert activation[0x12FD7BB] == ('call', '0x1289dd1')
assert activation[0x12FD7CB] == ('call', '0x1284d00')
assert activation[0x12FD7ED] == ('call', '0x130c63e')
assert activation[0x12FD86D] == ('call', '0x12892d3')
add_physical = pinned_block(0x1289DD1, 0x128D07F)
for address, instruction in {
    0x128A2B4: ('cmp', 'dword ptr [ebp - 0x128], 0x1a8'),
    0x128A2BE: ('je', '0x128c0b5'),
    0x128A2C4: ('cmp', 'dword ptr [ebp - 0x128], 0x1a9'),
    0x128A2CE: ('je', '0x128c0b5'),
    0x128C0BB: ('call', '0x9dfb65'),
    0x128C0C6: ('jne', '0x128c0db'),
    0x128C0CB: ('movss', 'xmm0, dword ptr [0x1b58d70]'),
    0x128C0D3: ('movss', 'dword ptr [eax + 0xa0], xmm0'),
    0x128C0E3: ('call', '0x14b65d8'),
}.items():
    assert add_physical[address] == instruction
world = pinned_block(0x9DFB65, 0x9DFB88)
assert world[0x9DFB68] == ('cmp', 'dword ptr [ebp + 8], 0x12')
assert world[0x9DFB6E] == ('cmp', 'dword ptr [ebp + 8], 0x17')
assert world[0x9DFB74] == ('cmp', 'dword ptr [ebp + 8], 0x35')
assert world[0x9DFB7A] == ('cmp', 'dword ptr [ebp + 8], 0x61')
print('PASS: activation order/scale branch; source world exception IDs18..23,53,97 must be mapped explicitly.')
