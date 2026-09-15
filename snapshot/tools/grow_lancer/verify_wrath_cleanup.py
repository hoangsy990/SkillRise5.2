"""Offline S21 cleanup contract; does not claim native/runtime completion."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)

def block(start, end):
    instructions = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in instructions) == end-start
    assert instructions[-1].mnemonic == 'ret'
    return {i.address: i for i in instructions}

def pins(instructions, expected):
    for address, pair in expected.items():
        i = instructions[address]
        assert (i.mnemonic, i.op_str) == pair, hex(address)

selector = block(0x10EF919, 0x10EFB4D)
# Evaluate only the actually reached comparison path. Reject any new opcode;
# this is not a general x86 emulator or a runtime observation.
for effect_type in (424, 425):
    pc, comparison = 0x10EF927, None
    for _ in range(40):
        i = selector[pc]
        if i.mnemonic == 'cmp':
            assert i.op_str.startswith('dword ptr [ebp - 4], ')
            comparison = effect_type - int(i.op_str.split(', ')[1], 0)
        elif i.mnemonic in ('jg', 'je', 'jmp'):
            assert comparison is not None
            if i.mnemonic == 'jmp' or (i.mnemonic == 'jg' and comparison > 0) or (i.mnemonic == 'je' and comparison == 0):
                pc = int(i.op_str, 0)
                continue
        elif i.mnemonic == 'xor':
            assert pc == 0x10EFB47 and i.op_str == 'eax, eax'
            break
        else:
            raise AssertionError((hex(pc), i.mnemonic, i.op_str))
        pc += i.size
    else:
        raise AssertionError('selector path did not terminate')

pins(block(0x14B65D8, 0x14B6619), {
    0x14B65E6: ('call', '0x10ef919'),
    0x14B65ED: ('je', '0x14b6604'),
    0x14B6612: ('call', '0x14b66db'),
})
pins(block(0x14B66DB, 0x14B679A), {
    0x14B6739: ('je', '0x14b6772'),
    0x14B673E: ('mov', 'eax, dword ptr [eax + 0x5c]'),
    0x14B6741: ('cmp', 'eax, dword ptr [ebp + 8]'),
    0x14B6744: ('jne', '0x14b6772'),
    0x14B6746: ('cmp', 'dword ptr [ebp + 0x10], -1'),
    0x14B674A: ('je', '0x14b6757'),
    0x14B6752: ('cmp', 'ecx, dword ptr [eax + 0x60]'),
    0x14B6755: ('jne', '0x14b6772'),
    0x14B675A: ('mov', 'eax, dword ptr [eax + 0x34c]'),
    0x14B6760: ('cmp', 'eax, dword ptr [ebp + 0xc]'),
    0x14B6763: ('jne', '0x14b6772'),
    0x14B6768: ('call', '0x14b6820'),
    0x14B677A: ('jne', '0x14b6792'),
    0x14B678A: ('call', '0x10eff55'),
})
pins(block(0x10EFF55, 0x10EFFD9), {
    0x10EFF8C: ('call', '0x96139e'),
    0x10EFF9B: ('mov', 'eax, dword ptr [eax + 0x5c]'),
    0x10EFF9E: ('cmp', 'eax, dword ptr [ebp + 8]'),
    0x10EFFA1: ('jne', '0x10effcf'),
    0x10EFFA3: ('cmp', 'dword ptr [ebp + 0x10], -1'),
    0x10EFFA7: ('je', '0x10effb4'),
    0x10EFFAF: ('cmp', 'ecx, dword ptr [eax + 0x60]'),
    0x10EFFB2: ('jne', '0x10effcf'),
    0x10EFFB7: ('mov', 'eax, dword ptr [eax + 0x34c]'),
    0x10EFFBD: ('cmp', 'eax, dword ptr [ebp + 0xc]'),
    0x10EFFC0: ('jne', '0x10effcf'),
    0x10EFFC5: ('call', '0x14b6820'),
})
pins(block(0x14B6820, 0x14B6926), {
    0x14B6834: ('call', '0x13163fb'),
    0x14B687F: ('jmp', '0x14b691b'),
    0x14B691E: ('call', '0x1315e97'),
})
print('PASS: 424/425 selector returns zero; fallback and secondary pools filter exact type/owner, optional subtype.')
print('PASS: secondary pool is searched only when fallback finds no match. No owner-wide child deletion justified.')
print('OPEN: actual 424/425 effect allocation, full reset semantics and native lifecycle/runtime integration.')
