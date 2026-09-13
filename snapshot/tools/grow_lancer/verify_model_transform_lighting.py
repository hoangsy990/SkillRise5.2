"""Pinned transform argument roles, not full geometry or lighting parity."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
instructions = {}
for begin, end in ((0x1887852, 0x1887965), (0x968D0E, 0x968D6C),
                   (0x132E6A1, 0x132E7C8)):
    block = list(decoder.disasm(data[begin-IMAGE_BASE:end-IMAGE_BASE], begin))
    assert block[-1].address + block[-1].size == end
    instructions.update({i.address: (i.mnemonic, i.op_str) for i in block})
for address, pair in {
    0x1887861: ('movss', 'xmm0, dword ptr [eax + 0xe8]'),
    0x1887872: ('movss', 'xmm0, dword ptr [eax + 0xe4]'),
    0x1887880: ('movss', 'xmm0, dword ptr [0x1b4df0c]'),
    0x18878CF: ('call', '0x968d0e'),
    0x18878DB: ('movss', 'xmm0, dword ptr [eax + 0xe8]'),
    0x18878EC: ('movss', 'xmm0, dword ptr [eax + 0xe4]'),
    0x1887945: ('call', '0x968d0e'),
    0x968D16: ('movss', 'xmm0, dword ptr [ebp + 0x28]'),
    0x968D21: ('movss', 'xmm0, dword ptr [ebp + 0x24]'),
    0x968D2C: ('movss', 'xmm0, dword ptr [ebp + 0x20]'),
    0x968D4D: ('push', '0x70b7980'),
    0x968D52: ('push', '0x8213d48'),
    0x968D57: ('push', '0x797e908'),
    0x968D62: ('call', '0x132df73'),
    0x132E724: ('call', '0xd3189d'),
    0x132E732: ('movzx', 'eax, byte ptr [eax + 0x50]'),
    0x132E738: ('je', '0x132e914'),
    0x132E79B: ('mulss', 'xmm0, dword ptr [ebp + 0x30]'),
    0x132E7A0: ('addss', 'xmm0, dword ptr [ebp + 0x2c]'),
    0x132E7AD: ('movss', 'xmm0, dword ptr [ebp + 0x34]'),
    0x132E7B9: ('jbe', '0x132e7c8'),
}.items():
    assert instructions[address] == pair, hex(address)
assert data[0x1B4DF0C-IMAGE_BASE:0x1B4DF10-IMAGE_BASE] == struct.pack('<f', 0.4)
root = Path(__file__).resolve().parents[2]
native = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp').read_bytes()
assert b'Luminosity = DotProduct(tn, LightPosition) * 0.8f + 0.4f;' in native
assert b'if (Luminosity < 0.2f) Luminosity = 0.2f;' in native
print('PASS S21 transform wrapper: object E4 is normal-light multiplier, E8 is minimum; bias literal0.4')
print('PASS native CPU transform uses multiplier0.8/minimum0.2/bias0.4')
reset = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x1316340-IMAGE_BASE:0x1316385-IMAGE_BASE], 0x1316340)}
for address, pair in {
    0x1316360: ('movss', 'xmm0, dword ptr [0x1b4df18]'),
    0x1316368: ('movss', 'dword ptr [eax + 0xe4], xmm0'),
    0x1316373: ('movss', 'xmm0, dword ptr [0x1b4df08]'),
    0x131637B: ('movss', 'dword ptr [eax + 0xe8], xmm0'),
}.items(): assert reset[address] == pair
for address, value in ((0x1B4DF18, 0.8), (0x1B4DF08, 0.2)):
    assert data[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f', value)
create = list(decoder.disasm(data[0x143E719-IMAGE_BASE:0x143E724-IMAGE_BASE], 0x143E719))
assert (create[-1].mnemonic, create[-1].op_str) == ('call', '0x1315e97')
assert struct.unpack_from('<I', data, 0x14B3224 + (0x5E7-0x4F7)*4 - IMAGE_BASE)[0] == 0x14808E9
print('PASS common effect reset initializes E4=0.8/E8=0.2, matching native CPU defaults')
selected = list(decoder.disasm(data[0x14808E9-IMAGE_BASE:0x1480D4A-IMAGE_BASE], 0x14808E9))
assert (selected[-1].address, selected[-1].mnemonic, selected[-1].op_str) == (0x1480D45, 'jmp', '0x14b2e0c')
assert all('0xe4' not in i.op_str and '0xe8' not in i.op_str for i in selected)
# Bound the epilogue at RET, not the following jump-table data.
epilogue = list(decoder.disasm(data[0x14B2E0C-IMAGE_BASE:0x14B2E36-IMAGE_BASE], 0x14B2E0C))
assert (epilogue[-1].address, epilogue[-1].mnemonic) == (0x14B2E33, 'ret')
assert all('0xe4' not in i.op_str and '0xe8' not in i.op_str for i in epilogue)
print('PASS complete selected5E7 constructor and return epilogue have no direct E4/E8 overrides')
blend = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x1332CBB-IMAGE_BASE:0x1332D6F-IMAGE_BASE], 0x1332CBB)}
for address, pair in {
    0x1332CC4: ('movsx', 'eax, word ptr [eax + 2]'),
    0x1332CC8: ('cmp', 'eax, dword ptr [ebp + 0x20]'),
    0x1332CCB: ('jne', '0x1332d6f'),
    0x1332D1D: ('mulss', 'xmm0, dword ptr [ebp + 0x24]'),
    0x1332D39: ('mulss', 'xmm0, dword ptr [ebp + 0x24]'),
    0x1332D55: ('mulss', 'xmm0, dword ptr [ebp + 0x24]'),
    0x1332D66: ('mov', 'byte ptr [ebp - 0x79], 0'),
}.items(): assert blend[address] == pair
import compare_player_actions as clips
asset = Path(r'D:\MU FICA Season 21\Data\Effect\magicpin03_new.bmd')
assert hashlib.sha256(asset.read_bytes()).hexdigest() == '6f3bfa98a5b8bc1a792e92130b72142bc3fb84c9b882aa4ba3da7ce883da9b67'
payload, version = clips.decode_container(asset, clips.load_converter())
assert version == 'v0F'
assert hashlib.sha256(payload).hexdigest() == '415fe7bdd368c02f3a4141be2426a8587940006003de0148d645ff5de3020465'
assert struct.unpack_from('<3h', payload, 32) == (1, 1, 1)
assert struct.unpack_from('<5h', payload, 38) == (36, 26, 39, 48, 0)
start = native.index(b'else if (BlendMesh <= -2 || m->Texture == BlendMesh)')
end = native.index(b'else if ((RenderFlag & RENDER_TEXTURE)', start)
branch = native[start:end]
assert b'EnableLight = false;' in branch
assert b'BodyLight[0] * BlendMeshLight' in branch
print('PASS pinned MagicPin03 mesh texture slot0; matching blend branch multiplies BodyLight and disables normal lighting in both engines')
selector = list(decoder.disasm(data[0x1332024-IMAGE_BASE:0x1332092-IMAGE_BASE], 0x1332024))
masks = [int(i.op_str.split(', ')[1], 0) for i in selector if i.mnemonic == 'and']
assert masks == [4, 0x200, 0x800, 0x1000, 0x4000, 0x10000, 0x20000, 0x80000, 0x100000, 8, 0x8000]
assert all(0x42 & mask == 0 for mask in masks)
assert all(i.op_str == '0x1332092' for i in selector[:-1] if i.mnemonic == 'jne')
assert (selector[-1].mnemonic, selector[-1].op_str) == ('je', '0x1332cbb')
route = {}
for begin, end in ((0x1331F47, 0x1331F53), (0x1332F30, 0x1332F45),
                   (0x133301E, 0x133302A), (0x13330E9, 0x13330EE)):
    route.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[begin-IMAGE_BASE:end-IMAGE_BASE], begin)})
for address, pair in {
    0x1331F4A: ('and', 'eax, 1'),
    0x1331F4D: ('je', '0x1332024'),
    0x1332F3C: ('cmp', 'dword ptr [ebp - 0x98], 2'),
    0x1332F43: ('je', '0x1332f97'),
    0x133301E: ('movzx', 'eax, byte ptr [ebp - 0x79]'),
    0x1333024: ('je', '0x13330e9'),
    0x13330E9: ('jmp', '0x133357f'),
}.items(): assert route[address] == pair
assert 0x42 & 1 == 0
print('PASS flags42 selects blend branch; resulting texture mode skips per-normal color submission when local79 is cleared')
start = native.index(b'else if (blendMeshIndex <= -2 || m->Texture == blendMeshIndex)')
end = native.index(b'else if ((renderFlags & RENDER_TEXTURE)', start)
assert b'enableLight = false;' in native[start:end]
assert b'BodyLight[0] * blendMeshAlpha' in native[start:end]
shader = (root / 'ExMain_RISE_PC/Main5.2_RISE/New_ModelBMD.cpp').read_bytes()
start = shader.index(b'if (blendMesh <= -2 || newMesh.Texture == blendMesh)')
end = shader.index(b'if (command.EnableLight)', start)
for statement in (b'command.Color.x *= blendMeshLight;', b'command.Color.y *= blendMeshLight;',
                  b'command.Color.z *= blendMeshLight;', b'command.Alpha = 1.0f;',
                  b'command.EnableLight = false;'):
    assert statement in shader[start:end]
config = (root / 'ExMain_RISE_PC/Main5.2_RISE/Defined_Global.h').read_bytes()
assert b'#define jdk_shader_local330' in config
gl = (root / 'ExMain_RISE_PC/Main5.2_RISE/MU_OpenGL.cpp').read_bytes()
assert b'if (!GLEW_VERSION_3_3)' in gl and b'if (!OGL330MODEL::Init())' in gl
assert b'return g_shaderEnabled;' in gl
assert b'GMMeshShader->AddMeshCommand(this, meshIndex' in native
assert b'GMMeshShader->FlushAllMesh();' in native
print('PASS actual RenderMesh fallback and shader command both preserve blend RGB multiplication/normal-light bypass')
print('Source enables shader capability, runtime activation requires GL3.3+successful init; this verifier does not observe live backend')
print('Scope: selected color preparation; shader execution/visual parity and unrelated engine side effects remain OPEN')
