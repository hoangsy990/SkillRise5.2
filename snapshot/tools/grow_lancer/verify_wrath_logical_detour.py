"""Follow actual main -> IGC -> main logical-buff path for424/425."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

main = DUMP.read_bytes()
igc = (DUMP.parent / '10860000.IGC.dll').read_bytes()
assert hashlib.sha256(main).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
assert hashlib.sha256(igc).hexdigest() == 'c6002fa6b6ff2bc7d41031ecf838aa00697ac24e7806eac45188ab528a25d3c8'
c = Cs(CS_ARCH_X86, CS_MODE_32)
def decode(data, base, start, end):
    block = list(c.disasm(data[start-base:end-base], start))
    assert sum(i.size for i in block) == end-start
    return {i.address: (i.mnemonic, i.op_str) for i in block}
entry = decode(main, IMAGE_BASE, 0x12892D3, 0x1289327)
assert entry[0x1289322] == ('jmp', '0x10886690')
gate = decode(igc, 0x10860000, 0x108866D0, 0x1088672A)
assert gate[0x108866D4] == ('lea', 'eax, [ecx - 0x71]')
assert gate[0x108866D7] == ('cmp', 'eax, 0x5b')
assert gate[0x108866DA] == ('ja', '0x10886727')
assert gate[0x10886727] == ('xor', 'al, al')
trampoline = decode(igc, 0x10860000, 0x10886690, 0x108866CB)
assert trampoline[0x108866B2] == ('je', '0x108866ba')
assert trampoline[0x108866C5] == ('jmp', 'dword ptr [0x109af178]')
assert struct.unpack_from('<I', igc, 0x109AF178-0x10860000)[0] == 0x128932B
continuation = decode(main, IMAGE_BASE, 0x128932B, 0x128935E)
assert continuation[0x1289331] == ('sub', 'eax, 0x1d')
assert continuation[0x1289350] == ('movzx', 'eax, byte ptr [eax + 0x1289c23]')
assert continuation[0x1289357] == ('jmp', 'dword ptr [eax*4 + 0x1289bb3]')
for key in (424, 425):
    assert key-0x71 > 0x5b
    index = main[0x1289C23-IMAGE_BASE+key-0x1d]
    target = struct.unpack_from('<I', main, 0x1289BB3-IMAGE_BASE+index*4)[0]
    assert index == 27 and target == 0x1289B9C
print('PASS:424/425 bypass IGC special cases, resume main and select epilogue; no extra logical branch here.')
print('Scope excludes physical effects, other buff callbacks and server semantics.')
removal = decode(main, IMAGE_BASE, 0x128558E, 0x12859EC)
assert removal[0x12859EB] == ('ret', '')
expected = {
    0x12855DA: ('cmp', 'dword ptr [ebp - 0x14], 0x82'),
    0x12855E1: ('jg', '0x12856d6'),
    0x12856D6: ('cmp', 'dword ptr [ebp - 0x14], 0x13c'),
    0x12856DD: ('jg', '0x128579f'),
    0x128579F: ('cmp', 'dword ptr [ebp - 0x14], 0x1c5'),
    0x12857A6: ('jg', '0x12857fb'),
    0x12857A8: ('cmp', 'dword ptr [ebp - 0x14], 0x1c5'),
    0x12857AF: ('je', '0x12859c3'),
    0x12857B5: ('cmp', 'dword ptr [ebp - 0x14], 0x13e'),
    0x12857BC: ('je', '0x1285940'),
    0x12857C2: ('cmp', 'dword ptr [ebp - 0x14], 0x141'),
    0x12857C9: ('je', '0x12859a4'),
    0x12857CF: ('cmp', 'dword ptr [ebp - 0x14], 0x1c2'),
    0x12857D6: ('je', '0x12859c3'),
    0x12857DC: ('cmp', 'dword ptr [ebp - 0x14], 0x1c3'),
    0x12857E3: ('je', '0x12859c3'),
    0x12857E9: ('cmp', 'dword ptr [ebp - 0x14], 0x1c4'),
    0x12857F0: ('je', '0x12859c3'),
    0x12857F6: ('jmp', '0x12859cf'),
    0x12859CF: ('push', 'dword ptr [ebp + 8]'),
    0x12859D9: ('call', '0x1311235'),
}
for address, instruction in expected.items():
    assert removal[address] == instruction, hex(address)
timer = decode(main, IMAGE_BASE, 0x1311235, 0x13112B2)
assert timer[0x1311244] == ('call', '0x1311d60')
assert timer[0x1311283] == ('je', '0x13112ac')
assert timer[0x131128E] == ('call', 'dword ptr [0x1b4c7d4]')
assert timer[0x13112A2] == ('call', '0x13125da')
key = decode(main, IMAGE_BASE, 0x1311D60, 0x1311D9D)
assert key[0x1311D70] == ('call', '0x113b5f8')
assert key[0x1311D75] == ('cmp', 'eax, -1')
assert key[0x1311D91] == ('mov', 'eax, dword ptr [eax + 4]')
assert key[0x1311D94] == ('add', 'eax, 0x3ed')
import pefile
pe = pefile.PE(data=main)
assert [(e.dll.lower(),i.name) for e in pe.DIRECTORY_ENTRY_IMPORT for i in e.imports
    if i.address == 0x1B4C7D4] == [(b'user32.dll',b'KillTimer')]
print('PASS: local-owner Wrath logical removal reaches shared timer cleanup; key=metadata field4+1005 after classifier gate.')
print('KillTimer import verified. No Wrath timer entry, duration, or timer registration inferred from generic cleanup.')
