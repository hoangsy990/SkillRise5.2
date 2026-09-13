"""Pin a receive-side lead; does NOT identify opcode or skill/buff namespace."""
import hashlib
import struct
import re
from pathlib import Path
import xml.etree.ElementTree as ET
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
decoder.skipdata = False
start, end = 0x129E990, 0x129F006
instructions = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
assert instructions[-1].address + instructions[-1].size == end
decoded = {i.address: (i.mnemonic, i.op_str) for i in instructions}
for address, expected in {
    0x129E990: ('push', 'ebp'),
    0x129E9CB: ('movzx', 'eax, byte ptr [eax + 8]'),
    0x129E9CF: ('shl', 'eax, 8'),
    0x129E9D8: ('movzx', 'ecx, byte ptr [ecx + 9]'),
    0x129E9DC: ('add', 'eax, ecx'),
    0x129ECC1: ('movzx', 'eax, word ptr [ebp - 0x234]'),
    0x129ECC8: ('mov', 'dword ptr [ebp - 0x224], eax'),
    0x129EE43: ('cmp', 'dword ptr [ebp - 0x224], 0x113'),
    0x129EE4D: ('je', '0x129efc1'),
    0x129EFE7: ('push', '-7'),
    0x129F000: ('call', '0x17259cc'),
}.items():
    assert decoded[address] == expected, (hex(address), decoded.get(address))
print('PASS anchored receive branch: packet bytes8/9 -> compared value275 -> call17259CC argument -7')
assert struct.unpack_from('<I', data, 0x130505C-IMAGE_BASE+0x75*4)[0] == 0x1300320
dispatch = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x12FE380-IMAGE_BASE:0x12FE3EA-IMAGE_BASE], 0x12FE380)}
assert dispatch[0x12FE3C4] == ('mov', 'eax, dword ptr [ebp + 8]')
assert dispatch[0x12FE3C7] == ('mov', 'dword ptr [ebp - 0x3f8], eax')
assert dispatch[0x12FE3DD] == ('mov', 'eax, dword ptr [ebp - 0x3f8]')
assert dispatch[0x12FE3E3] == ('jmp', 'dword ptr [eax*4 + 0x130505c]')
block = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x1300320-IMAGE_BASE:0x1300374-IMAGE_BASE], 0x1300320)}
assert block[0x1300366] == ('push', 'dword ptr [ebp + 0xc]')
assert block[0x1300369] == ('call', '0x129e990')
assert data[0x1B99864-IMAGE_BASE:].split(b'\0',1)[0].decode('cp949') == '\ubc84\ud504\uc774\ud329\ud2b8 \uc804\ub2ec %d'
buffs = Path(r'D:\GameServer S21\Data\BuffEffectManager.xml').read_bytes()
assert hashlib.sha256(buffs).hexdigest() == '201452c00d9d5ce0db83820ef48ec4317a6654864c0be9176a6389694a4ad6b0'
tree = ET.fromstring(re.sub(rb'<!--.*?-->', b'', buffs, flags=re.S))
names = {x.get('Index'):x.get('Name') for x in tree.iter('Buff')}
assert names['216'] == 'Circle Shield' and names['275'] == 'Bleeding'
print('PASS opcode75 table entry/call, native buff-effect diagnostic string; buff216 Circle Shield,275 Bleeding')
print('OPEN full contact receiver integration and point renderer semantics; NOT Clash learning evidence')

# Continuous contact block, ending before unrelated buff333 branch.
contact = list(decoder.disasm(data[0x129F04C-IMAGE_BASE:0x129F60E-IMAGE_BASE], 0x129F04C))
assert contact[-1].address + contact[-1].size == 0x129F60E
assert [i.address for i in contact if i.mnemonic == 'call' and i.op_str == '0x15c6594'] == [0x129F28A,0x129F447,0x129F604]
assert [i.address for i in contact if i.mnemonic == 'call' and i.op_str == '0x143e57c'] == [0x129F14B]
cd = {i.address:(i.mnemonic,i.op_str) for i in contact}
for address in (0x129F269,0x129F426,0x129F5E3):
    assert cd[address] == ('push','3')
for address in (0x129F285,0x129F442,0x129F5FF):
    assert cd[address] == ('push','0x80e3')
for address, value in {0x1B4DEC4:100.,0x1B99910:135.,0x1B4E4DC:40.,
                       0x1B4E4E0:50.,0x1B4DF38:-100.,0x1B4FB1C:90.}.items():
    assert struct.unpack_from('<f',data,address-IMAGE_BASE)[0] == value
ctor = {i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(
    data[0x15DEA64-IMAGE_BASE:0x15DED7E-IMAGE_BASE],0x15DEA64)}
assert ctor[0x15DEA92] == ('cmp','dword ptr [ebp - 0x240], 3')
assert ctor[0x15DEA99] == ('je','0x15deca1')
assert ctor[0x15DECB7] == ('mov','dword ptr [eax + 0xa00], 0x64')
assert ctor[0x15DECC7] == ('mov','dword ptr [eax + 0x6c], 0xa')
print('PASS contact has exactly1 effect+3 joint calls; joints80E3/subtype3/scale40; subtype3 life100/tails10')
update = {i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(
    data[0x161303C-IMAGE_BASE:0x161332D-IMAGE_BASE],0x161303C)}
for address, expected in {
    0x161303F:('cmp','dword ptr [eax + 0xa00], 0x55'),
    0x1613046:('jle','0x16130ab'),
    0x16130B6:('addss','xmm0, dword ptr [0x1b4dec0]'),
    0x16130DB:('jb','0x16130f0'),
    0x16131FC:('jb','0x1613206'),
    0x1613201:('mov','byte ptr [eax], 0'),
    0x1613215:('jb','0x1613285'),
    0x1613251:('jbe','0x1613285'),
    0x1613265:('jb','0x1613285'),
    0x1613272:('subss','xmm0, dword ptr [0x1b4e4e0]'),
}.items():
    assert update[address] == expected
for address,value in {0x1B4DEC0:7.,0x1B4FCA4:35.,0x1B4E938:70.,
                      0x1B4E4D4:20.,0x1B4E4D0:10.,0x1B4E93C:120.}.items():
    assert struct.unpack_from('<f',data,address-IMAGE_BASE)[0] == value
print('PASS subtype3 velocity/contact/brake thresholds; full native integration still OPEN')
native = (Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8')
adapter = native.split('void CreateCircleShieldContact(',1)[1].split('void CreateCircleShieldRoot(',1)[0]
assert adapter.count('CreateEffect(') == 1 and adapter.count('CreateJoint(') == 3
assert adapter.count('3, &secondActor, 40.f, -1, 0, 0, -1, 0, -1)') == 3
for fragment in ('start[2] += 100.f;', 'Vector(100.f, 50.f, 0.f, local);',
                 'Vector(-100.f, 50.f, 0.f, local);', 'angle[2] -= 135.f;',
                 'angle[2] -= 90.f;', 'angle[2] += 135.f;',
                 'secondActor.Light, 1, &secondActor, -1, 0, 0)'):
    assert fragment in adapter
assert not any(x in adapter for x in ('PlayBuffer(', 'CurrentAction =', 'SendRequest', 'SendPacket'))
print('PASS native contact adapter source contract:1controller+3joints, actor2 ownership, no action/sound/send side effects')
