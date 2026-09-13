"""Do not equate native damage opcode11 with S21's pinned dispatch table."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
assert struct.unpack_from('<I', data, 0x130505C + 0x11*4 - IMAGE_BASE)[0] == 0x1305041
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
tail = list(decoder.disasm(data[0x1305041-IMAGE_BASE:0x130505C-IMAGE_BASE], 0x1305041))
assert (tail[-1].address, tail[-1].mnemonic) == (0x130505B, 'ret')
assert [(i.mnemonic, i.op_str) for i in tail if i.mnemonic == 'call'] == [('call', '0x19b96f4')]
root = Path(__file__).resolve().parents[2]
# Checks below use ASCII tokens only; preserve legacy source bytes losslessly.
ws = (root/'ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp').read_text(encoding='latin-1')
assert 'case 0x11: ReceiveAttackDamage(ReceiveBuffer);' in ws
begin = ws.index('void ReceiveAttackDamage(')
assert ws.index('SetPlayerShock(c, Damage);', begin) > ws.index('if (Success)', begin)
character = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp').read_text(encoding='latin-1')
begin = character.index('void SetPlayerShock(')
end = character.index('void SetPlayerDie(', begin)
shock = character[begin:end]
for text in ('if (c->Dead) return;', 'if (Hit > 0)',
             'SetAction(&c->Object, PLAYER_SHOCK);', 'c->Movement = false;',
             'SetAction(&c->Object, MONSTER01_SHOCK);'):
    assert text in shock
print('PASS native opcode11 damage path and native conditional shock implementation exist')
print('PASS pinned S21 table opcode11 selects return-only tail, not a damage handler')
assert struct.unpack_from('<I', data, 0x130505C + 0x23*4 - IMAGE_BASE)[0] == 0x12FEB6B
route = {}
for begin, end in ((0x12FEB6B, 0x12FEB73), (0x128FEE9, 0x1290053),
                   (0x140AA8F, 0x140AAA6)):
    route.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[begin-IMAGE_BASE:end-IMAGE_BASE], begin)})
for address, pair in {
    0x12FEB6E: ('call', '0x128fe90'),
    0x128FEEC: ('movzx', 'eax, byte ptr [eax + 3]'),
    0x128FEF6: ('movzx', 'ecx, byte ptr [ecx + 4]'),
    0x128FF1C: ('and', 'eax, 0x7fff'),
    0x128FF27: ('call', '0x13f5f39'),
    0x128FF7D: ('mov', 'eax, dword ptr [eax + 8]'),
    0x128FF86: ('movzx', 'eax, byte ptr [eax + 0xd]'),
    0x128FF8A: ('and', 'eax, 0xf'),
    0x1290040: ('je', '0x1290053'),
    0x1290042: ('push', 'dword ptr [ebp - 0x58]'),
    0x1290045: ('push', 'dword ptr [ebp - 0x64]'),
    0x1290048: ('push', 'dword ptr [ebp - 0x60]'),
    0x129004B: ('call', '0x140a9c0'),
    0x140AA8F: ('cmp', 'dword ptr [ebp + 0x10], 0xc'),
    0x140AA93: ('je', '0x140aaa6'),
    0x140AA95: ('cmp', 'dword ptr [ebp + 0x10], 0xf'),
    0x140AA99: ('je', '0x140aaa6'),
    0x140AA9B: ('cmp', 'dword ptr [ebp + 0xc], 0'),
    0x140AA9F: ('jg', '0x140aaa6'),
}.items(): assert route[address] == pair, hex(address)
print('PASS S21 opcode23 ->128FE90 key/damage/type decoder ->conditional140A9C0 reaction')
print('PASS reaction gate allows types12/15 or positive damage after action exclusions')
reaction = {}
for begin, end in ((0x140ABC4, 0x140AC15), (0x141B12C, 0x141B167),
                   (0x1327D72, 0x1327DE8)):
    reaction.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[begin-IMAGE_BASE:end-IMAGE_BASE], begin)})
for address, pair in {
    0x140ABC4: ('push', '0x148'),
    0x140ABD2: ('call', '0x1327d72'),
    0x140ABDC: ('mov', 'byte ptr [eax + 0x38], 0'),
    0x140ABF5: ('call', '0x141b12c'),
    0x140AC03: ('push', '5'),
    0x140AC0E: ('call', '0x1327d72'),
    0x141B136: ('cmp', 'dword ptr [ebp - 4], 0x2fb'),
    0x141B141: ('cmp', 'dword ptr [ebp + 0xc], 8'),
    0x141B147: ('cmp', 'dword ptr [ebp + 0xc], 4'),
    0x141B153: ('cmp', 'dword ptr [ebp + 0xc], 3'),
    0x141B159: ('cmp', 'dword ptr [ebp + 0xc], 4'),
    0x1327DAB: ('je', '0x1327de6'),
    0x1327DD4: ('mov', 'word ptr [eax + 0x2a], cx'),
    0x1327DDE: ('movss', 'dword ptr [eax + 0xd0], xmm0'),
}.items(): assert reaction[address] == pair
defines = (root/'ExMain_RISE_PC/Main5.2_RISE/_define.h').read_text(encoding='latin-1')
assert '#define MONSTER01_SHOCK    5' in defines
print('PASS ordinary S21 monster shock5 agrees with native; excludes attack3/4, special monster763 additionally excludes actions>=8')
print('PASS default S21 player branch requests328 and clears movement; repeated identical action does not reset frame in setter')
print('OPEN player clip equivalence, auxiliary branches, special monster coverage and GrowLancer damage producer/timing')
