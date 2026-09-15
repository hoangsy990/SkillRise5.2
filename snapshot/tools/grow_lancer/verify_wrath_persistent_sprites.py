"""Pin the first eleven sprite submissions, not the entire persistent branch."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

d = DUMP.read_bytes()
assert hashlib.sha256(d).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
c = Cs(CS_ARCH_X86, CS_MODE_32)
start, end = 0x13DB9D7, 0x13EBFDE
items = list(c.disasm(d[start-IMAGE_BASE:end-IMAGE_BASE], start))
assert sum(i.size for i in items) == end-start
entry = {i.address: (i.mnemonic, i.op_str) for i in items}
for address, expected in {
    0x13EB9DA: ('push', '0x1a8'),
    0x13EB9E2: ('add', 'ecx, 0x35ac'),
    0x13EB9E8: ('call', '0x130c8a5'),
    0x13EB9F2: ('jne', '0x13eba12'),
    0x13EB9F4: ('push', '0x1a9'),
    0x13EB9FC: ('add', 'ecx, 0x35ac'),
    0x13EBA02: ('call', '0x130c8a5'),
    0x13EBA0C: ('je', '0x13ecb52'),
}.items():
    assert entry[address] == expected, (hex(address), entry[address])
print('PASS: membership424 OR425 enters one persistent branch; neither skips the entire branch.')
for address, expected in {
    0x13E7CA2: ('mov', 'eax, dword ptr [eax + 0x5c]'),
    0x13E7CA5: ('mov', 'dword ptr [ebp - 0x9ac], eax'),
    0x13E7DF2: ('cmp', 'dword ptr [ebp - 0x9ac], 0xa94'),
    0x13E7DFC: ('je', '0x13e7e03'),
    0x13E7DFE: ('jmp', '0x13f1b17'),
}.items():
    assert entry[address] == expected
print('PASS: source player-model switch enters the branch containing Wrath; native insertion must stay in MODEL_PLAYER.')
for address, expected in {
    0x13DBA0A: ('movzx', 'eax, byte ptr [eax + 0x3a]'),
    0x13DBA0E: ('cmp', 'eax, 2'),
    0x13DBA11: ('jne', '0x13dba2c'),
    0x13DBA16: ('movss', 'xmm0, dword ptr [0x1b509c0]'),
    0x13DBA1E: ('comiss', 'xmm0, dword ptr [eax + 0xdc]'),
    0x13DBA25: ('jbe', '0x13dba2c'),
    0x13DBA27: ('jmp', '0x13f2456'),
}.items():
    assert entry[address] == expected
assert d[0x1B509C0-IMAGE_BASE:0x1B509C4-IMAGE_BASE] == struct.pack('<f', 1.e-6)
# Writer identified by byte-pattern lead, then decoded from enclosing prologue.
writer = list(c.disasm(d[0x173EF78-IMAGE_BASE:0x17439E2-IMAGE_BASE], 0x173EF78))
assert sum(i.size for i in writer) == 0x17439E2-0x173EF78
wp = {i.address: (i.mnemonic, i.op_str) for i in writer}
for address, expected in {
    0x1743994: ('movzx', 'eax, byte ptr [eax + 0x3a]'),
    0x1743998: ('cmp', 'eax, 1'),
    0x174399B: ('jne', '0x17439e2'),
    0x17439A3: ('movss', 'xmm0, dword ptr [eax + 0xdc]'),
    0x17439AB: ('subss', 'xmm0, dword ptr [0x1b4df04]'),
    0x17439B9: ('movss', 'dword ptr [eax + 0xdc], xmm0'),
    0x17439C7: ('movss', 'xmm0, dword ptr [0x1b4df04]'),
    0x17439CF: ('comiss', 'xmm0, dword ptr [eax + 0xdc]'),
    0x17439D6: ('jbe', '0x17439e2'),
    0x17439DE: ('mov', 'byte ptr [eax + 0x3a], 2'),
}.items():
    assert wp[address] == expected
assert d[0x1B4DF04-IMAGE_BASE:0x1B4DF08-IMAGE_BASE] == struct.pack('<f', .1)
print('PASS: source state1 alpha fade -.1 then state2 matches native Teleport transition; render cutoff is1e-6, not .1.')
branch = [i for i in items if i.address >= 0x13EB9DA]
bones, sprites = [], []
for n, i in enumerate(branch):
    if i.mnemonic != 'call': continue
    if i.op_str == '0x132ec63':
        pushes = [j.op_str for j in branch[n-8:n] if j.mnemonic == 'push']
        assert len(pushes) == 4 and pushes[0] == '0'
        bones.append(int(pushes[1], 0))
    elif i.op_str == '0x172760a':
        assert branch[n-1].mnemonic == 'push' and branch[n-1].op_str == '0x8078'
        sprites.append(i.address)
assert bones == [4,11,34,25,35,26,10,4,17,18,20]
assert len(sprites) == 11
for address, scalar in [(0x1B50294,.23), (0x1B4DF08,.2),
                        (0x1B4DD48,1.), (0x1B50130,37.),
                        (0x1B4DF0C,.4), (0x1B4DF14,.5),
                        (0x1B4DF18,.8), (0x1B4E4D4,20.)]:
    assert d[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f', scalar)
print('PASS: first11 lightmarks submissions; bone arguments', bones)
print('Remaining terrain/particle layers, bone helper mapping, cadence and runtime acceptance OPEN.')
terrain = list(c.disasm(d[0x13EBFDE-IMAGE_BASE:0x13EC3F7-IMAGE_BASE], 0x13EBFDE))
assert sum(i.size for i in terrain) == 0x13EC3F7-0x13EBFDE
draws = []
for n, i in enumerate(terrain):
    if i.mnemonic == 'call' and i.op_str == '0x1765df1':
        assert terrain[n-1].mnemonic == 'push'
        draws.append(int(terrain[n-1].op_str, 0))
assert draws == [0x8078, 0x7fdd, 0x7fdd, 0x8147]
for address, scalar in [(0x1B4E930,8.), (0x1B4E6D4,.05),
                        (0x1B4E6D8,.15), (0x1B4DF04,.1),
                        (0x1B50118,2.8), (0x1B4E864,4.5), (0x1B4E4CC,5.)]:
    assert d[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f', scalar)
print('PASS: four following terrain resource arguments and selected scales/RGB constants; fourth pulse helper still OPEN.')
pulse = {i.address: (i.mnemonic, i.op_str) for i in c.disasm(
    d[0x18F930D-IMAGE_BASE:0x18F93DF-IMAGE_BASE], 0x18F930D)}
assert pulse[0x18F9332] == ('divss', 'xmm0, dword ptr [ebp + 8]')
assert pulse[0x18F9355] == ('idiv', 'ecx')
assert pulse[0x18F9379] == ('jbe', '0x18f93a2')
assert pulse[0x18F9386] == ('call', '0xa01178')
assert pulse[0x18F93AD] == ('call', '0xa01178')
wrapper = {i.address: (i.mnemonic, i.op_str) for i in c.disasm(
    d[0xA01178-IMAGE_BASE:0xA01197-IMAGE_BASE], 0xA01178)}
assert wrapper[0xA01188] == ('call', '0x1a29a90')
assert d[0x1A36CBD-IMAGE_BASE:0x1A36CBF-IMAGE_BASE] == bytes.fromhex('d9ff') # FCOS fallback
for address, scalar in [(0x1B50164,-8.), (0x1B4E8B0,4.), (0x1B4E474,2.),
                        (0x1B4E6DC,.25), (0x1B4F284,.65), (0x1B50100,.85)]:
    assert d[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f', scalar)
print('PASS: selected pulse division/remainder/cosine branch and Wrath parameters; native Render22 not equivalent.')
