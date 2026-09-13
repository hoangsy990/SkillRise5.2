"""Source optional three-slot bonus and class7 catalog, not native activation."""
import hashlib
import struct
import xml.etree.ElementTree as ET
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from minidump.minidumpfile import MinidumpFile
from inspect_breche_full_dump import SNAPSHOT, PIN
from disasm_s21_window import DUMP, IMAGE_BASE

b = DUMP.read_bytes()
assert hashlib.sha256(b).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
d = Cs(CS_ARCH_X86,CS_MODE_32)
pins = {i.address:(i.mnemonic,i.op_str) for start,end in (
    (0xC5B7F5,0xC5B8B3),(0x1118C35,0x1118D78),
    (0xC5D149,0xC5D164),(0xB23464,0xB234A8),
    (0xB60918,0xB60931),(0x10FC071,0x10FC0BA),
    (0xC4FF95,0xC4FFEA),(0xC54784,0xC54827),
    (0x961588,0x9615AC),(0xC568A1,0xC568B2))
    for i in d.disasm(b[start-IMAGE_BASE:end-IMAGE_BASE],start)}
for address,pair in {
    0xC5B840:('cmp','dword ptr [ebp - 4], 3'),
    0xC5B854:('cmp','dword ptr [eax + 0x18], 0'),
    0xC5B86D:('mov','eax, dword ptr [eax + 0x10]'),
    0xC5B87E:('call','0x1118c35'),
    0xC5B883:('mov','eax, dword ptr [eax + 0x18]'),
    0xC5B88C:('cmp','eax, dword ptr [ebp - 0x18]'),
    0xC5B8A4:('add','ecx, dword ptr [eax + 0x1c]'),
    0xC5B8B0:('ret','4'),
    0xC5D157:('imul','eax, dword ptr [ebp + 8], 0x1c'),
    0x1118CB3:('shl','eax, 9'),
    0x1118CB9:('add','eax, dword ptr [ecx + 0xc]'),
    0xB23472:('imul','eax, dword ptr [ebp + 8], 0x24'),
    0xB234A3:('add','eax, 0x10'),
    0x1118D4D:('mov','eax, dword ptr [eax + 0x10]'),
    0xC5B80C:('mov','eax, dword ptr [eax + 0x14]'),
    0xB60925:('add','ecx, 0x2c'),
    0x10FC0B4:('xor','eax, eax'),
    0x961592:('mov','eax, dword ptr [eax + 0x1f8]'),
    0x9615A4:('mov','eax, dword ptr [eax + 0x298]'),
    0xC4FFA9:('add','ecx, 0xa0'),
    0xC4FFD7:('xor','eax, eax'),
    0xC4FFE3:('mov','eax, dword ptr [eax + 4]'),
    0xC5479F:('mov','eax, dword ptr [eax + 4]'),
    0xC547BA:('movsx','eax, byte ptr [eax + 0xd]'),
    0xC547CE:('add','eax, 0x10'),
    0xC547F6:('mov','eax, dword ptr [eax + 8]'),
    0xC5480E:('mov','eax, dword ptr [eax]'),
    0xC568AD:('add','eax, 0x10'),
}.items(): assert pins[address] == pair,hex(address)
with SNAPSHOT.open('rb') as f:
    assert hashlib.file_digest(f,'sha256').hexdigest() == PIN
r = MinidumpFile.parse(str(SNAPSHOT)).get_reader()
def u32(p): return struct.unpack('<I',r.read(p,4))[0]
head=u32(0x70A3910+0x38)
node=u32(head+4)
seen=set()
for _ in range(64):
    assert node and node not in seen
    seen.add(node)
    row=r.read(node,20)
    assert not row[13]
    key=struct.unpack_from('<i',row,16)[0]
    if key==7:
        start,end=u32(node+20),u32(node+24)
        assert end-start==72
        rows=[struct.unpack('<9i',r.read(p,36)) for p in range(start,end,36)]
        break
    node=u32(node+(0 if 7<key else 8))
else: raise ValueError('class tree depth exceeded')
assert rows == [(28,0,20,193,2065,1235,2,1,31),(30,1,20,234,2089,1236,1,15,33)]
xml=Path(r'D:\GameServer S21\Data\Items\ItemList.xml').read_bytes()
assert hashlib.sha256(xml).hexdigest() == '61711d9e6b0e6f14028191aa739aad4a3b0c5911cdb3468a3bc61b988b12af78'
section=ET.fromstring(xml).find("Section[@Index='20']")
for row,name in zip(rows,('[Skill] Oversting of Saturation','[Skill] Wild Breath of Gale')):
    item=section.find(f"Item[@Index='{row[3]}']")
    assert item.get('Name')==name and int(item.get('SkillIndex'))==row[4]
    print(f'PASS {name}: item20/{row[3]}, skill{row[4]} ->lookup replacement{row[5]}, bonus type{row[6]} value{row[7]}')
assert all(row[4] not in range(271,280) for row in rows)
print('PASS source loops three occupied slots, compares requested bonus type, sums matched values; absent slots skipped')
print('PASS class7 catalog does not directly remap base skills271..279; type1 value15 is conditional, not a universal bonus')
print('OPEN slot ownership/equip conditions and native acquisition; separate skill property3F0 map value has missing-key zero path')

# Observe the optional managers in the same hash-pinned capture, not live state.
ui=u32(0x1E09CAC)
skill_manager=u32(ui+0x1F8)
slot_manager=u32(ui+0x298)
assert skill_manager and slot_manager
slots=u32(slot_manager+0x7C)
assert slots
assert [u32(slots+i*28+24) for i in range(3)] == [0,0,0]
skill_head=u32(skill_manager+0xA0)
for skill in range(271,280):
    node=u32(skill_head+4)
    visited=set()
    for _ in range(64):
        assert node and node not in visited
        visited.add(node)
        row=r.read(node,20)
        if row[13]:
            break
        key=struct.unpack_from('<i',row,16)[0]
        assert key != skill, f'snapshot now contains base skill{skill}'
        node=u32(node+(0 if skill<key else 8))
    else: raise ValueError('skill tree depth exceeded')
print('PASS captured optional slots all empty and base271..279 absent from bonus-record map; source skips both bonus additions for these base skills in this capture')
print('Not a claim that every runtime character has zero bonus; do not disable advanced-skill bonuses globally')
