"""Offline speed inputs and loaded text; never a live-cast/last-writer proof."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from minidump.minidumpfile import MinidumpFile
from inspect_breche_full_dump import SNAPSHOT, PIN
from disasm_s21_window import DUMP, IMAGE_BASE

binary = DUMP.read_bytes()
assert hashlib.sha256(binary).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
d = Cs(CS_ARCH_X86, CS_MODE_32)
pins = {i.address:(i.mnemonic,i.op_str) for start,end in (
    (0x9609DC,0x9609F9),(0x11D8FA3,0x11D90C2),
    (0x11D93C7,0x11D946A),(0x11D989B,0x11D98AC),
    (0x8E7C00,0x8E7C0C),(0xA0A583,0xA0A595),(0xB608F4,0xB60906),
    (0x9A23EC,0x9A2460),(0x997699,0x99772A),
    (0x11425C6,0x1142619),(0xB6077E,0xB6078F),
    (0xB56DA0,0xB56DE3),(0xB57040,0xB57054))
    for i in d.disasm(binary[start-IMAGE_BASE:end-IMAGE_BASE],start)}
for address,pair in {
    0x9609E9:('call','0x11d8fa3'),
    0x9609F0:('call','0x8e7c00'),
    0x11D8FE5:('add','ecx, 0x38'),
    0x11D90A6:('add','eax, 0xc'),
    0x11D93E2:('mov','eax, dword ptr [eax + 4]'),
    0x11D93FD:('movsx','eax, byte ptr [eax + 0xd]'),
    0x11D9411:('add','eax, 0x10'),
    0x11D9439:('mov','eax, dword ptr [eax + 8]'),
    0x11D98A7:('add','eax, 0x10'),
    0x8E7C00:('cmp','dword ptr [ecx + 0x14], 0x10'),
    0xA0A58D:('mov','eax, dword ptr [eax + 0x2d98]'),
    0xB608FE:('mov','eax, dword ptr [eax + 0x1d8]'),
    0x9A2415:('call','0x996b70'),
    0x9A2421:('mov','dword ptr [eax], 0x1b4e9a0'),
    0x9A242A:('mov','dword ptr [eax + 0x1d4], 7'),
    0x9A2437:('push','dword ptr [eax + 0x1d4]'),
    0x9A243D:('call','0x997699'),
    0x9A2446:('mov','dword ptr [ecx + 0x1d8], eax'),
    0x9976E4:('mov','eax, 0x111'),
    0x11425DA:('add','ecx, 0x38'),
    0x1142612:('add','eax, 4'),
    0xB6078A:('add','eax, 0x10'),
    0xB56DA0:('cmp','dword ptr [ebp - 0x150], 0'),
    0xB56DA9:('cmp','dword ptr [ebp - 0x150], 5'),
    0xB56DB2:('cmp','dword ptr [ebp - 0x150], 8'),
    0xB56DC0:('mov','ax, word ptr [eax + 0x1c2]'),
    0xB56DD5:('mov','ax, word ptr [eax + 0x1b6]'),
    0xB57047:('push','0x15'),
    0xB5704F:('call','0xb546ff'),
}.items(): assert pins[address] == pair,hex(address)
assert struct.unpack_from('<I',binary,0x99772D+7*4-IMAGE_BASE)[0] == 0x9976E4
with SNAPSHOT.open('rb') as stream:
    assert hashlib.file_digest(stream,'sha256').hexdigest() == PIN
reader = MinidumpFile.parse(str(SNAPSHOT)).get_reader()
def u32(address):
    return struct.unpack('<I',reader.read(address,4))[0]

head = u32(0x70A1B90 + 0x38)
for key,expected in ((64,'Tốc độ đánh: %d'),(4286,'%d / %d')):
    node = u32(head + 4)
    visited = set()
    for _ in range(64):
        assert node and node not in visited
        visited.add(node)
        row = reader.read(node,20)
        assert not row[13], 'missing text key'
        actual = struct.unpack_from('<i',row,16)[0]
        if actual == key:
            string = node + 0x10 + 0xC
            size,capacity = u32(string+0x10),u32(string+0x14)
            assert size <= capacity < 10000
            pointer = u32(string) if capacity >= 16 else string
            raw = reader.read(pointer,size)
            assert raw.decode('utf-8') == expected
            print(f'TextKey={key}; UTF8={raw.hex()}; Value={ascii(expected)}')
            break
        node = u32(node + (0 if key < actual else 8))
    else:
        raise ValueError('text tree depth exceeded')
attributes = u32(0x1E04FBC)
profile = u32(u32(0x1E04FB8) + 0x2D98)
assert u32(profile) == 0x1B4E9A0 and u32(profile + 0x1D4) == 7
values = tuple(struct.unpack('<H',reader.read(attributes+offset,2))[0]
               for offset in (0x1B6,0x1C2))
cap = u32(profile + 0x1D8)
assert values == (273,273) and cap == 273
print(f'PASS snapshot inputs={values}; upper-limit operand={cap}; snapshotSHA256={PIN}')
print('PASS class7 profile constructor9A23EC writes upper-limit273 via997699; snapshot vtable/class match this profile')
print('OPEN subsequent/indirect mutations; constructor default is not proof of which caller last registered animation speed')
property_head=u32(0x70A1D6C+0x38)
node=u32(property_head+4)
visited=set()
for _ in range(64):
    assert node and node not in visited
    visited.add(node)
    row=reader.read(node,20)
    assert not row[13]
    key=struct.unpack_from('<i',row,16)[0]
    if key==21:
        record=node+20
        assert u32(record)==21
        label=record+8
        size,capacity=u32(label+16),u32(label+20)
        assert size==20 and capacity>=size and capacity<1000
        raw=reader.read(u32(label) if capacity>=16 else label,size)
        assert raw.decode('utf-8')=='* Tốc độ đánh'
        print('PASS property21 loaded label='+ascii(raw.decode('utf-8')))
        break
    node=u32(node+(0 if 21<key else 8))
else: raise ValueError('property tree depth exceeded')
print('PASS display property21 selects source field1B6 for class7; classes0/5/8 select1C2')
print('Text4286 alone is only a numeric format; property record21 supplies the speed-label association')
