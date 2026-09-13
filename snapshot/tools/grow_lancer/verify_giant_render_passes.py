"""Pinned Giant GL renderer call evidence, not full dispatch or visual parity."""
import hashlib
import struct
import pefile
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
ins = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(data[0xA5D5F6-IMAGE_BASE:0xA5DB15-IMAGE_BASE], 0xA5D5F6))
assert ins[-1].address == 0xA5DB14 and ins[-1].mnemonic == 'ret'
calls = [i.address for i in ins if i.mnemonic == 'call' and i.op_str == '0x968cb3']
assert calls == [0xA5D699,0xA5D6F1,0xA5D749,0xA5D7A1,0xA5D82D,
                 0xA5D87E,0xA5D91E,0xA5D9BE,0xA5DA65,0xA5DB0C]
pins = {0xA5D7E1:'0x84ea', 0xA5D832:'0x84eb', 0xA5D826:'0x42',
        0xA5D828:'2', 0xA5D877:'0x42', 0xA5D879:'3',
        0xA5DA0F:'0x7f1f', 0xA5DAB6:'0x7f1f',
        0xA5DA5E:'0x44', 0xA5DB05:'0x44'}
decoded = {i.address:(i.mnemonic,i.op_str) for i in ins}
for address, operand in pins.items():
    assert decoded[address] == ('push', operand), hex(address)
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
extra = {}
for start, end in ((0xA1C016,0xA1C027), (0x18F930D,0x18F93DF),
                   (0x13268D4,0x1326903), (0x1A36C9F,0x1A36D32),
                   (0x1887DF2,0x1887E60), (0x1887EBB,0x1887EDA),
                   (0x1893296,0x189346D)):
    extra.update((i.address,(i.mnemonic,i.op_str)) for i in
                 decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
for address, expected in {
    0xA1C016:('push','0xa5d5f6'), 0xA1C01B:('push','0x741'),
    0xA1C020:('call','0xa1dce6'),
    0x18F933F:('cvttss2si','eax, xmm0'),
    0x18F9350:('cvttss2si','ecx, xmm0'),
    0x18F9355:('idiv','ecx'),
    0x18F9357:('cvtsi2ss','xmm0, edx'),
    0x13268D4:('call','dword ptr [0x1b4c9a8]'),
    0x13268FB:('movss','dword ptr [0x70b53d8], xmm0'),
    0x1A36CBD:('fcos',''), 0x1A36CF4:('fcos',''),
    0x1887E08:('add','eax, 0x5c'),
    0x1887E15:('call','0xa1ebd1'),
    0x1887E4E:('call','dword ptr [eax]'),
    0x1887E5C:('jne','0x1887ebb'),
    0x1887ED0:('call','0x1893296'),
    0x1893352:('je','0x18933af'),
    0x18933A4:('call','0x968c58'),
    0x18933AA:('jmp','0x189346b'),
    0x18933C4:('je','0x189346b'),
    0x18933DF:('movss','dword ptr [eax], xmm0'),
    0x1893465:('call','0x968c58'),
}.items():
    assert extra[address] == expected
assert struct.unpack_from('<f', data, 0x1B50150-IMAGE_BASE)[0] == -4.0
assert struct.unpack_from('<2d', data, 0x1CA4320-IMAGE_BASE) == (0.0,4294967296.0)
pe = pefile.PE(data=data, fast_load=True)
pe.parse_data_directories(directories=[1])
assert [(entry.dll.lower(), symbol.name) for entry in pe.DIRECTORY_ENTRY_IMPORT
        for symbol in entry.imports if symbol.address == 0x1B4C9A8] == [(b'winmm.dll', b'timeGetTime')]
pe.close()
print('PASS cosine scalar fallback and timeGetTime float clock writer; not exact CRT numerical parity')
print('PASS model741 registration pair and signed/integer pulse reduction; speed argument -4')
root = Path(__file__).resolve().parents[2]
native = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/CustomItemRenderMesh.cpp').read_text(encoding='utf-8-sig')
helper = native.split('float Render22(',1)[1].split('bool BMD::runtime_make_render',1)[0]
assert 'float work = a1 / 100.f;' in helper
assert 'if (v6 != 0.f)' in helper and 'float v5 = 0.0f;' in helper
assert 'timeGetTime()' in helper
# Counterexample to treating this existing helper as the source function.
source_divisor = int(struct.unpack_from('<f',data,0x1BC83FC-IMAGE_BASE)[0] / -4.0)
native_divisor = int(6283.185546875 * (-4.0 / 100.0))
assert (source_divisor, native_divisor) == (-1570, -251)
# At reduced phase zero S21 uses -cos(0); native skips that assignment.
assert ((-1.0 + 1.0) * .5, (0.0 + 1.0) * .5) == (0.0, .5)
print('PASS native-helper mismatch evidence: divisor -1570 vs -251, zero-phase output 0 vs .5')
post = list(decoder.disasm(data[0x1893296-IMAGE_BASE:0x189346D-IMAGE_BASE],0x1893296))
assert post[-1].mnemonic == 'ret' and post[-1].address == 0x189346C
assert [i.address for i in post if i.mnemonic == 'call' and i.op_str == '0x968c58'] == [0x18933A4,0x1893465]
print('PASS complete post-render callback / two conditional body draws; no automatic RGB restoration')
objects = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzObject.cpp').read_text(encoding='utf-8-sig')
# Select the actual late overlay by its exact ordered body pair, not any freeze test.
pattern = '''if (g_isCharacterBuff(o, eDeBuff_Freeze))
                {
                    b->RenderBody(RENDER_TEXTURE, o->Alpha, -2, 1.f, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, o->HiddenMesh);
                }
                else if (g_isCharacterBuff(o, eDeBuff_BlowOfDestruction))
                {
                    Vector(0.3f, 0.5f, 1.f, b->BodyLight);
                    b->RenderBody(RENDER_TEXTURE, o->Alpha, -2, 1.f, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV, o->HiddenMesh);
                }'''
assert pattern in objects
print('PASS native late-overlay source pair; S21 additional membership mappings remain OPEN')
print('PASS anchored complete function / ten mesh-wrapper calls / explicit extra texture arguments')
print('OPEN native activation, client status translation, exact CRT parity and runtime visual parity')
