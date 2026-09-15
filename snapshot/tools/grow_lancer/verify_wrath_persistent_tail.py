"""Pinned S21 Wrath selected four-particle branch; not runtime parity."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
start, end = 0x13DB9D7, 0x13ECB52
items = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
assert sum(i.size for i in items) == end-start
pins = {i.address: (i.mnemonic, i.op_str) for i in items}
for address, expected in {
    0x13EC809: ('call', '0x1a26ce1'),
    0x13EC80F: ('push', '3'),
    0x13EC812: ('idiv', 'ecx'),
    0x13EC821: ('je', '0x13ec842'),
    0x13EC82A: ('je', '0x13ec949'),
    0x13EC837: ('je', '0x13eca50'),
    0x13EC83D: ('jmp', '0x13ecb52'),
    0x13EC944: ('jmp', '0x13ecb52'),
    0x13ECA4B: ('jmp', '0x13ecb52'),
}.items():
    assert pins[address] == expected, (hex(address), pins[address])

for lo, hi, textures, subtypes in [
    (0x13EC842, 0x13EC944, [0x8086,0x8086,0x8089,0x8089], [13,14,13,14]),
    (0x13EC949, 0x13ECA4B, [0x8087,0x8087,0x8089,0x8089], [14,15,13,14]),
    (0x13ECA50, 0x13ECB52, [0x8086,0x8086,0x8089,0x8089], [13,14,13,14]),
]:
    block = [i for i in items if lo <= i.address < hi]
    calls = [n for n,i in enumerate(block) if (i.mnemonic,i.op_str)==('call','0x1724176')]
    assert len(calls) == 4
    previous = 0
    for k,n in enumerate(calls):
        args = [(i.mnemonic,i.op_str) for i in block[previous:n]]
        assert args[-1] == ('push', hex(textures[k]))
        assert ('push', hex(subtypes[k])) in args
        assert ('add', 'eax, 0x3254') in args  # character-owned OBJECT
        assert ('add', 'ecx, 0x164') in args  # incoming OBJECT angle
        assert ('movss', 'xmm0, dword ptr [ebp - 0x998]') in args
        local = '0x58' if k % 2 == 0 else '0x19c'
        assert args[-3] == ('lea', f'eax, [ebp - {local}]')
        previous = n + 2
print('PASS: full linear decode to Wrath branch end; three mutually exclusive four-particle branches pinned.')
print('OPEN: outer cadence, particle constructor/update/render/destruction, bone compatibility and native runtime parity.')

for texture, expected in [(0x8081,0x16663ce),(0x8082,0x16663ce),
                          (0x8083,0x16663ce),(0x8086,0x16673c6),
                          (0x8087,0x164b3ff),(0x8089,0x166c453)]:
    selector = data[0x16a9789 + texture - 0x8012 - IMAGE_BASE]
    target = struct.unpack_from('<I', data, 0x16a96a1 + selector*4 - IMAGE_BASE)[0]
    assert target == expected
for subtype in (13,14):
    assert struct.unpack_from('<I',data,0x16a9c9d+subtype*4-IMAGE_BASE)[0] == 0x16685f0
ctor = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(
    data[0x16685f0-IMAGE_BASE:0x16687f5-IMAGE_BASE],0x16685f0))
assert sum(i.size for i in ctor) == 0x205
cp = {i.address:(i.mnemonic,i.op_str) for i in ctor}
for address, expected in {
    0x16685f6: ('push','5'), 0x16685fd: ('add','esi, 0x1b'),
    0x1668614: ('push','0x48'), 0x1668619: ('add','edx, 0x34'),
    0x1668628: ('mulss','xmm0, dword ptr [ebp + 0x1c]'),
    0x1668653: ('mov','ecx, 0x168'),
    0x1668684: ('push','0xe'), 0x1668689: ('add','edx, 0x14'),
    0x16687f0: ('jmp','0x166a5c6'),
}.items():
    assert cp[address] == expected
print('PASS: six texture dispatch table entries; 8086 subtype13/14 constructor selected random parameters and tail.')

def pin_block(lo, hi, expected):
    block = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(data[lo-IMAGE_BASE:hi-IMAGE_BASE],lo))
    assert sum(i.size for i in block) == hi-lo
    found = {i.address:(i.mnemonic,i.op_str) for i in block}
    for address, instruction in expected.items():
        assert found[address] == instruction, (hex(address),found[address])

pin_block(0x164cad4,0x164cc7c,{
    0x164cadf:('cmp','dword ptr [eax + 0x14], 0xe'),
    0x164cae3:('je','0x164cafa'),
    0x164caf0:('cmp','dword ptr [eax + 0x14], 0xf'),
    0x164caf4:('jne','0x164cc7c'),
    0x164cb00:('push','0xc'),0x164cb07:('add','esi, 8'),
    0x164cb78:('push','0x1e'),0x164cb7d:('add','edx, 0x14'),
    0x164cbe3:('push','0xf'),0x164cbe8:('add','edx, 0xf'),
    0x164cc77:('jmp','0x164d1e4'),
})
pin_block(0x166d4d4,0x166d5c5,{
    0x166d4df:('cmp','dword ptr [eax + 0x14], 0xd'),
    0x166d4e3:('je','0x166d4fa'),
    0x166d4f0:('cmp','dword ptr [eax + 0x14], 0xe'),
    0x166d4f4:('jne','0x166d5c5'),
    0x166d505:('mov','dword ptr [eax + 0x48], 0xf'),
    0x166d5c0:('jmp','0x166f197'),
})
pin_block(0x16dc17e,0x16dc6d1,{
    0x16dc189:('cmp','dword ptr [eax + 0x50], 0'),
    0x16dc19d:('call','0x96139e'),
    0x16dc1b4:('mov','byte ptr [eax + 8], 0'),
    0x16dc1c3:('and','dword ptr [eax + 0x48], 0'),
    0x16dc1d7:('cmp','dword ptr [eax + 0x48], 0xf'),
    0x16dc456:('addss','xmm0, dword ptr [eax + 0x6c]'),
    0x16dc49b:('push','0x1d'),0x16dc4b8:('call','0x132ec63'),
    0x16dc4d3:('push','0x26'),0x16dc4f0:('call','0x132ec63'),
    0x16dc6cc:('jmp','0x16de8bd'),
})
print('PASS: selected 8087/8089 constructors and 8086 owner/bone-follow update pins; renderer/shared cleanup still OPEN.')

pin_block(0x16bb079,0x16bb2e8,{
    0x16bb084:('cmp','dword ptr [eax + 0x14], 0xe'),
    0x16bb095:('cmp','dword ptr [eax + 0x14], 0xf'),
    0x16bb0b9:('push','0x14'),0x16bb0be:('add','edx, 0xa'),
    0x16bb0c5:('mulss','xmm0, dword ptr [0x1b4e308]'),
    0x16bb0f5:('movss','dword ptr [ebp - 0x6864], xmm0'),
    0x16bb133:('push','0x1d'),0x16bb16b:('push','0x26'),
    0x16bb1bd:('mulss','xmm0, dword ptr [0x1b4e4d0]'),
    0x16bb2e3:('jmp','0x16bbb1a'),
})
pin_block(0x16e18f7,0x16e1b92,{
    0x16e1902:('cmp','dword ptr [eax + 0x14], 0xd'),
    0x16e1913:('cmp','dword ptr [eax + 0x14], 0xe'),
    0x16e1953:('push','0x1d'),0x16e198b:('push','0x26'),
    0x16e1a43:('subss','xmm0, dword ptr [0x1b4e6d4]'),
    0x16e1a73:('subss','xmm0, dword ptr [0x1b9ebc8]'),
    0x16e1b8d:('jmp','0x16e37c5'),
})
for address,value in [(0x1b4e308,.001),(0x1b4e4d0,10.),
                      (0x1b9ebc8,0.0666666030883789)]:
    assert data[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f',value)
print('PASS: 8087/8089 selected updates; 8087 modifies stack scalar, not directly particle Alpha.')

for subtype in (13,14):
    assert struct.unpack_from('<I',data,0x16410e8+4*subtype-IMAGE_BASE)[0] == 0x1635d58
pin_block(0x16210c6,0x1621147,{
    0x16210dc:('je','0x1624c92'),
    0x162112c:('je','0x1635d2a'),
    0x162113c:('je','0x1627a2a'),
})
for lo,hi,call in [(0x1635d58,0x1635e01,0x1635df4),
                   (0x1627b36,0x1627bda,0x1627bd2),
                   (0x1624df8,0x1624ea1,0x1624e94)]:
    pin_block(lo,hi,{lo:('push','4'),call:('call','0x18e8b0c')})
print('PASS: selected renderer routes and billboard calls; stored RGB used without extra alpha multiply in these blocks.')

for base,resource,string_va,name in [
    (0x18bd7ec,0x8086,0x1bb90f8,b'Effect\\firehik_mono01.jpg'),
    (0x18bd80c,0x8087,0x1bb9114,b'Effect\\firehik_mono02.jpg'),
    (0x18bd84c,0x8089,0x1bb914c,b'Effect\\firehik_mono03.jpg'),
]:
    pin_block(base,base+0x20,{
        base:('push','0'),base+2:('push','1'),
        base+4:('push','0x2900'),base+9:('push','0x2601'),
        base+14:('push',hex(resource)),base+19:('push',hex(string_va)),
        base+24:('call','0x18f8fa2'),
    })
    assert data[string_va-IMAGE_BASE:string_va-IMAGE_BASE+len(name)+1] == name+b'\0'
pin_block(0xcc561f,0xcc5634,{0xcc562d:('mov','byte ptr [eax + 0x214], 3')})
pin_block(0x1620b3a,0x1620b5c,{
    0x1620b47:('cmp','eax, 3'),0x1620b4c:('call','0x18e7137'),
})
print('PASS: mono registration paths LINEAR/CLAMP; successful JPEG loader component3 and additive renderer branch pinned. Not a live metadata snapshot.')

assert struct.unpack_from('<I',data,0x16a9c75-IMAGE_BASE)[0] == 0x16663fc
assert struct.unpack_from('<I',data,0x1723bd9-IMAGE_BASE)[0] == 0x16d8f22
pin_block(0x16663fc,0x1666534,{
    0x1666407:('mov','dword ptr [eax + 0x48], 5'),
    0x1666414:('mov','ecx, 0x168'),
    0x166644d:('movss','dword ptr [eax + 0x6c], xmm0'),
    0x166652f:('jmp','0x1666e6f'),
})
pin_block(0x16d8f22,0x16d9081,{
    0x16d8f40:('subss','xmm0, dword ptr [0x1b4e6d8]'),
    0x16d8f69:('jbe','0x16d8f7a'),
    0x16d8f76:('mov','byte ptr [eax + 8], 0'),
    0x16d907c:('jmp','0x16d9b7e'),
})
pin_block(0x1631343,0x16313ec,{
    0x1631343:('push','4'),0x16313df:('call','0x18e8b0c'),
})
assert data[0x1b4e6d8-IMAGE_BASE:0x1b4e6dc-IMAGE_BASE] == struct.pack('<f',.15)
print('PASS: scatter subtype0 constructor/update tables, five-tick lifetime, fade and storedRGB billboard pins.')
for base,resource in [(0x18bd74c,0x8081),(0x18bd76c,0x8082),(0x18bd78c,0x8083)]:
    pin_block(base,base+0x20,{
        base+4:('push','0x2901'),base+9:('push','0x2601'),
        base+14:('push',hex(resource)),base+24:('call','0x18f8fa2'),
    })
print('PASS: scatter registration sampler LINEAR/REPEAT, not mono CLAMP.')
for base,resource in [(0x18bd68c,0x8078),(0x18bd1cc,0x7ef7),
                      (0x18be4f8,0x7fdd),(0x18d21d3,0x8147)]:
    pin_block(base,base+0x20,{
        base+4:('push','0x2900'),base+9:('push','0x2601'),
        base+14:('push',hex(resource)),base+24:('call','0x18f8fa2'),
    })
print('PASS: four ground/sprite resource registrations LINEAR/CLAMP.')
pin_block(0x13f3ba4,0x13f3ef8,{
    0x13f3be6:('cmp','dword ptr [ebp - 0xc], 0x190'),
    0x13f3bf6:('call','0x9610cb'),0x13f3bfd:('call','0x9a45d3'),
    0x13f3c08:('add','eax, 0x3254'),
    0x13f3e67:('call','0x96139e'),
    0x13f3e76:('movzx','eax, byte ptr [eax + 0xb]'),
    0x13f3e94:('push','0'),0x13f3e9c:('call','0x13db9d7'),
    0x13f3ea6:('push','1'),0x13f3eae:('call','0x13db9d7'),
})
pin_block(0x13db9d7,0x13dba7c,{
    0x13dba25:('jbe','0x13dba2c'),0x13dba2c:('push','0x1b'),
    0x13dba37:('call','0x130c8a5'),
    0x13dba4e:('call','0x968b22'),0x13dba55:('call','0xef2a81'),
    0x13dba5a:('mov','dword ptr [ebp - 0x97c], eax'),
})
print('PASS: full400-slot render caller and initial model lookup/gates. Outer frame/pass scheduling not proven.')
pin_block(0x18f60ef,0x18f6d93,{
    0x18f682c:('call','0x13f3ba4'),
    0x18f693b:('call','0x1726fcf'),
    0x18f6a80:('call','0x1726e56'),0x18f6a8c:('call','0x1620984'),
    0x18f6b48:('call','0x1726fcf'),
    0x18f6b81:('call','0x1726e56'),0x18f6b8e:('call','0x1620984'),
})
pin_block(0x1726fcf,0x1727015,{
    0x1726fe5:('cmp','dword ptr [ebp - 4], 0x3e8'),
    0x1726ffe:('call','0x96139e'),
    0x172700d:('mov','byte ptr [eax + 0xb], 1'),
})
print('PASS: scene render ordering and live-sprite visibility producer; second sprite/particle pass has no repeated character call in this function.')
