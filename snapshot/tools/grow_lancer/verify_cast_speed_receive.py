"""Pinned S21 speed receive path; not proof of native transport equivalence."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
assert struct.unpack_from('<I', data, 0x130505C + 0x82*4 - IMAGE_BASE)[0] == 0x1303939
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
decoded = {i.address:(i.mnemonic,i.op_str) for start,end in (
    (0x1303939,0x1303A1E),(0x1303D1B,0x1303D29),
    (0x1296442,0x1296493),(0xB56CFF,0xB5708E))
    for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
for address,pair in {
    0x130394D:('cmp','eax, 0xc1'),
    0x1303963:('movzx','eax, byte ptr [eax + 3]'),
    0x130397C:('cmp','eax, 0xc2'),
    0x1303992:('movzx','eax, byte ptr [eax + 4]'),
    0x1303A0C:('cmp','dword ptr [ebp - 0x290], 4'),
    0x1303A13:('je','0x1303d1b'),
    0x1303D1E:('call','0x1296442'),
    0x129644E:('mov','eax, dword ptr [0x1e04fbc]'),
    0x1296456:('mov','cx, word ptr [ecx + 4]'),
    0x129645A:('mov','word ptr [eax + 0x1b6], cx'),
    0x1296469:('mov','cx, word ptr [ecx + 8]'),
    0x129646D:('mov','word ptr [eax + 0x1c2], cx'),
    0x129648B:('call','0xb56cff'),
    0x1296492:('ret',''),
    0xB56DC0:('mov','ax, word ptr [eax + 0x1c2]'),
    0xB56DD5:('mov','ax, word ptr [eax + 0x1b6]'),
    0xB56FF5:('push','0x10be'),
    0xB56FFF:('call','0x9609dc'),
}.items(): assert decoded[address] == pair,hex(address)
handler = [pair for address,pair in decoded.items() if 0x1296442 <= address < 0x1296493]
assert [p[1] for p in handler if p[0] == 'call'] == ['0xad1437','0xb56cff']
root = Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE'
ws = (root/'WSclient.cpp').read_text(encoding='latin-1')
header = (root/'WSclient.h').read_text(encoding='latin-1')
import re
for target,source in [('AttackSpeed','SpeedAttack'),('MagicSpeed','MagicAttack')]:
    assert re.search(r'CharacterAttribute->'+target+r'\s*=\s*Data->'+source+r';',ws)
    assert re.search(r'\bDWORD\s+'+source+r'\s*;',header)
print('PASS S21 opcode82/sub04 selects1296442; words at4/8 write attributes1B6/1C2')
print('PASS handler calls display refresh B56CFF, NOT direct animation-speed registration')
print('PASS display consumes same fields and text key4286; text semantics not yet decoded')
print('PASS native receive has named DWORD SpeedAttack/MagicAttack; no wire-width equivalence assumed')
print('OPEN C2 dispatch uses sub-byte4 but selected handler has fixed offsets; do not claim valid C2 payload support')
print('OPEN field naming corroboration, manager bonuses/cap, native activation and runtime parity')
