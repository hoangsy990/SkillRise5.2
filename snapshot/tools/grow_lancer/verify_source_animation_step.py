"""Pinned S21 frame identity and arithmetic; not scheduler or visual acceptance."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest().upper() == '6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
decoder.skipdata = False

def check(start, size, expected):
    instructions = {i.address: (i.mnemonic, i.op_str) for i in
                    decoder.disasm(data[start-IMAGE_BASE:start-IMAGE_BASE+size], start)}
    for address, instruction in expected.items():
        assert instructions.get(address) == instruction, (hex(address), instructions.get(address))

check(0x14115BB, 0x4A0, {
    0x14115BB: ('push', 'ebp'),
    0x14115BC: ('mov', 'ebp, esp'),
    0x1411623: ('shl', 'eax, 4'),
    0x141162C: ('movss', 'xmm0, dword ptr [ecx + eax + 4]'),
    0x1411632: ('movss', 'dword ptr [ebp - 0x14], xmm0'),
    0x141165A: ('cmp', 'eax, 3'),
    0x1411666: ('cmp', 'eax, 4'),
    0x1411684: ('cmp', 'eax, 0x9c'),
    0x1411692: ('cmp', 'eax, 0x131'),
    0x14116CB: ('cmp', 'eax, 0x92'),
    0x141170B: ('cmp', 'eax, 4'),
    0x141171B: ('cmp', 'eax, 0xa7'),
    0x14117E5: ('cmp', 'eax, 0x9f'),
    0x1411801: ('cmp', 'dword ptr [eax + 0x5c], 0xa94'),
    0x1411808: ('je', '0x141197e'),
    0x1411981: ('cmp', 'dword ptr [eax + 0x5c], 0x7fe'),
    0x14119A8: ('push', '0x3d'),
    0x14119BF: ('push', '0x48'),
    0x14119D6: ('push', '0xb2'),
    0x14119F0: ('push', '0x118'),
    0x1411A30: ('movss', 'xmm0, dword ptr [ebp - 0x14]'),
})
# Source action189 is not in any action-specific modifier branch above;
# MODEL_PLAYER bypasses model-specific modifiers. Status gates may prevent
# the animation call entirely; this does not assert those statuses are absent.
assert 189 not in (3, 4, 0x9c, 0x131, 0x92, 0xa7, 0x9f)
check(0x132D144, 0x45, {
    0x132D151: ('movss', 'xmm0, dword ptr [eax]'),
    0x132D155: ('addss', 'xmm0, dword ptr [ebp + 0x14]'),
    0x132D15D: ('movss', 'dword ptr [eax], xmm0'),
})
check(0x1411A2F, 0x2C, {
    0x1411A30: ('movss', 'xmm0, dword ptr [ebp - 0x14]'),
    0x1411A3D: ('add', 'eax, 0x2c'),
    0x1411A44: ('add', 'eax, 0xd4'),
    0x1411A4D: ('add', 'eax, 0xd0'),
    0x1411A56: ('call', '0x132d0cd'),
})
check(0x157A63C, 0x40, {
    0x157A63C: ('movss', 'xmm0, dword ptr [eax + 0xd0]'),
    0x157A661: ('comiss', 'xmm0, dword ptr [0x1b744d8]'),
})
check(0x132D300, 0x5E, {
    0x132D303: ('cvttss2si', 'eax, dword ptr [eax]'),
    0x132D30E: ('idiv', 'dword ptr [ebp - 0x10]'),
    0x132D311: ('cvtsi2ss', 'xmm0, edx'),
    0x132D321: ('subss', 'xmm2, xmm1'),
    0x132D325: ('addss', 'xmm0, xmm2'),
    0x132D32C: ('movss', 'dword ptr [eax], xmm0'),
    0x132D35B: ('ret', '0x18'),
})
check(0x1423159, 0x195, {
    0x142321A: ('cmp', 'eax, 0x6f'),
    0x1423226: ('cmp', 'eax, 0x147'),
    0x142322B: ('jl', '0x142328f'),
    0x1423296: ('cmp', 'eax, 0x9f'),
    0x14232A4: ('cmp', 'eax, 0xd5'),
    0x14232B2: ('cmp', 'eax, 0x139'),
    0x14232BC: ('call', '0x140af0e'),
    0x14232ED: ('ret', ''),
})
assert all(0x6f <= a < 0x147 and a not in (0x9f, 0xd5, 0x139)
           for a in range(185, 195))
print('PASS source185..194 ordinary animation-end branch calls140AF0E; private native284..293 must stop too')
print('PASS pinned source: caller frame D0 matches Shining owner frame; direct speed addition; integer-modulo plus fractional wrap')
print('PASS caller14115BB: positive action189 PlaySpeed unchanged for MODEL_PLAYER before status gates; no per-call FPS multiplier on this path')
check(0x1595834, 0xB5, {
    0x159587A: ('mov', 'dword ptr [ebp - 0xc], 0xc8'),
    0x1595894: ('mov', 'dword ptr [ebp - 0xc], 0x96'),
    0x15958B0: ('imul', 'eax, dword ptr [ebp - 8], 0x3d8'),
    0x15958D3: ('call', '0x14b76c0'),
    0x15958E1: ('call', '0x10f027e'),
    0x15958E8: ('ret', ''),
})
check(0x18F40BE, 0xA4, {
    0x18F4121: ('jne', '0x18f42da'),
    0x18F4140: ('call', '0x13f3fb0'),
    0x18F4158: ('call', '0x1595834'),
})
check(0x13F40ED, 0x3D, {
    0x13F4101: ('cmp', 'dword ptr [ebp - 0x14], 0x190'),
    0x13F411A: ('call', '0x13f3ef8'),
    0x13F4129: ('ret', ''),
})
check(0x13F3EF8, 0xB8, {
    0x13F3F80: ('call', '0x14356fb'),
    0x13F3F8D: ('call', '0x1424b34'),
    0x13F3F9A: ('call', '0x142f1e9'),
    0x13F3FAF: ('ret', ''),
})
print('PASS conditional scene path18F4140 character traversal precedes18F4158 effect traversal; record loops pinned')
check(0x1424B34, 0x233, {
    0x1424C26: ('jne', '0x1424c2d'),
    0x1424CD5: ('mov', 'dword ptr [eax + 0x74], ecx'),
    0x1424CE5: ('mov', 'word ptr [eax + 0xa8], cx'),
    0x1424D62: ('call', '0x1423ba4'),
})
check(0x1423BA4, 0x119, {
    0x1423BB7: ('jne', '0x1423c95'),
    0x1423C9B: ('call', '0x1422cf7'),
    0x1423CA8: ('call', '0x14115bb'),
})
check(0x18EE7E4, 0x1C1, {
    0x18EE972: ('cmp', 'dword ptr [ebp - 0x8ac], 3'),
    0x18EE98F: ('call', '0x18f40be'),
    0x18EE984: ('cmp', 'dword ptr [ebp - 0x8ac], 6'),
    0x18EE99F: ('call', '0x18f2ebf'),
})
check(0x18F2EBF, 0xE2A, {
    0x18F3BB1: ('call', '0x13f3fb0'),
    0x18F3C30: ('call', '0x1595834'),
    0x18F3CE8: ('ret', ''),
})
print('PASS character1424B34 ->1423BA4 ->14115BB; scene6 also traverses characters before effects')
check(0x18EE9A5, 0x141, {
    0x18EEAAF: ('sub', 'eax, 0x28'),
    0x18EEAB2: ('mov', 'dword ptr [ebp - 0x8a4], eax'),
    0x18EEAE1: ('jmp', '0x18ee826'),
})
check(0x18EF1FD, 0x61, {
    0x18EF202: ('sub', 'eax, dword ptr [ebp - 0x928]'),
    0x18EF208: ('mov', 'dword ptr [ebp - 0x89c], eax'),
    0x18EF20E: ('cmp', 'dword ptr [ebp - 0x89c], 0x28'),
    0x18EF215: ('jge', '0x18ef24c'),
    0x18EF22C: ('call', 'dword ptr [0x1b4c4a4]'),
    0x18EF242: ('mov', 'dword ptr [ebp - 0x89c], 0x28'),
    0x18EF252: ('add', 'eax, dword ptr [ebp - 0x89c]'),
    0x18EF258: ('mov', 'dword ptr [ebp - 0x93c], eax'),
})
print('PASS update catch-up subtracts40 and repeats; next accumulator combines remainder with capped-minimum frame duration')
def u32(offset):
    return struct.unpack_from('<I', data, offset)[0]

def cstring(offset):
    return data[offset:data.index(b'\0', offset)].decode('ascii')

# This dump is memory-mapped: import RVA is an offset directly, not a file RVA
# translated through disk section headers. OFT retains import-name entries.
descriptor = u32(u32(0x3c) + 24 + 104)
imports = {}
for _ in range(4096):
    if not u32(descriptor + 12):
        break
    thunk = u32(descriptor)
    first = u32(descriptor + 16)
    library = cstring(u32(descriptor + 12)).lower()
    for index in range(65536):
        name_rva = u32(thunk + index * 4) if thunk else 0
        if not name_rva:
            break
        if not name_rva & 0x80000000:
            imports[IMAGE_BASE + first + index * 4] = (library, cstring(name_rva + 2))
    descriptor += 20
assert imports[0x1B4C4A0] == ('kernel32.dll', 'GetTickCount')
assert imports[0x1B4C4A4] == ('kernel32.dll', 'Sleep')
assert imports[0x1B4C9A8] == ('winmm.dll', 'timeGetTime')
check(0x18EF05D, 0x16, {
    0x18EF05D: ('mov', 'eax, dword ptr [0xbeacf44]'),
    0x18EF062: ('mov', 'dword ptr [ebp - 0x928], eax'),
    0x18EF068: ('call', 'dword ptr [0x1b4c4a0]'),
    0x18EF06E: ('mov', 'dword ptr [0xbeacf44], eax'),
})
print('PASS mapped import names: GetTickCount clock, Sleep wait, timeGetTime statistics; update quantum40 milliseconds /25Hz')
print('OPEN actual controller samples and FPS-dependent native equivalence; do not widen windows from clock proof alone')
