"""Pinned sprite visibility producer/pass evidence, not full owner cleanup proof."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
decoded = {}
for start,end in ((0x1726FCF,0x1727015),(0x1727015,0x1727079),
                  (0x1727079,0x1727119),(0x96139E,0x9613AD),
                  (0x13167BB,0x13167E0),(0x9B3D94,0x9B3DAC),
                  (0x9B436B,0x9B43B8),(0x9B3343,0x9B3380),
                  (0x966734,0x966788),(0x968A5C,0x968A84),
                  (0x9679DC,0x9679EE),(0x967954,0x967969),
                  (0x960742,0x96075E),(0x969C9B,0x969CB1),
                  (0x962905,0x96290F),(0x9631E0,0x96320D),
                  (0x96320D,0x96322D),(0x1316DE7,0x1316E95),
                  (0x13169F0,0x1316DE7),(0x18CC6A9,0x18CC90C),
                  (0x1726E56,0x1726F99),(0x13F93C1,0x1407950),
                  (0x13F8ED7,0x13F8F6E)):
    decoded.update({i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start)})
expected = {
    0x1726FE5:('cmp','dword ptr [ebp - 4], 0x3e8'),
    0x1726FFE:('call','0x96139e'),
    0x1727008:('je','0x1727011'),
    0x172700D:('mov','byte ptr [eax + 0xb], 1'),
    0x9613A8:('movzx','eax, byte ptr [eax]'),
    0x1727085:('movzx','eax, byte ptr [eax + 0xb]'),
    0x1727098:('addss','xmm0, dword ptr [0x1b4df04]'),
    0x17270DF:('subss','xmm0, dword ptr [0x1b4df04]'),
    0x1727054:('mov','eax, dword ptr [eax + 0x34c]'),
    0x1727064:('call','0x13167bb'),
    0x172706C:('and','dword ptr [eax + 0x34c], 0'),
    0x13167C8:('mov','byte ptr [eax], cl'),
    0x13167D7:('call','0x9b3d94'),
    0x9B3D9E:('add','ecx, 0x3d0'),
    0x9B3DA4:('call','0x9b436b'),
    0x9B334B:('mov','eax, dword ptr [eax + 4]'),
    0x9B334E:('and','dword ptr [eax], 0'),
    0x9B3365:('je','0x9b337e'),
    0x9B3375:('call','0x966734'),
    0x9B43A0:('mov','dword ptr [eax], ecx'),
    0x9B43AC:('mov','dword ptr [eax + 4], ecx'),
    0x9B43B2:('and','dword ptr [eax + 4], 0'),
    0x966759:('add','eax, 8'),
    0x966767:('call','0x96704f'),
    0x966774:('call','0x966673'),
    0x968A66:('call','0x9679dc'),
    0x9679E6:('call','0x967954'),
    0x96795E:('add','ecx, 4'),
    0x967961:('call','0x960742'),
    0x96074F:('je','0x96075c'),
    0x960756:('call','0x963437'),
    0x969CA1:('shl','eax, 5'),
    0x969CA8:('call','0x946140'),
    0x96290A:('sub','eax, dword ptr [ebp + 8]'),
    0x96321A:('call','0x962905'),
    0x963224:('call','0x9631e0'),
    0x9631FE:('lock xadd','dword ptr [eax], esi'),
    0x963205:('mov','eax, dword ptr [ebp - 8]'),
    0x1316E41:('mov','byte ptr [ebp - 0x18], al'),
    0x1316E48:('mov','byte ptr [ebp - 0x17], al'),
    0x1316E50:('movss','dword ptr [ebp - 0x14], xmm0'),
    0x1316E6E:('add','ecx, 0x3d0'),
    0x1316E74:('call','0x13189bd'),
    0x1316A9A:('subss','xmm0, dword ptr [ebp + 8]'),
    0x1316AA5:('movss','dword ptr [eax], xmm0'),
    0x1316AE2:('call','0x131892e'),
    0x18CC712:('cmp','dword ptr [ebp - 0x20], 0x10'),
    0x18CC729:('cmp','dword ptr [ebp - 0x24], 0x10'),
    0x18CC73F:('add','eax, 0xbe9d2b8'),
    0x18CC74A:('mov','eax, dword ptr [eax + 4]'),
    0x18CC75D:('call','0x9b3d94'),
    0x18CC8C2:('call','0x1316de7'),
    0x18CC8DD:('call','0x13167e1'),
    0x18CC8E6:('mov','eax, dword ptr [eax + 0x354]'),
    0x1726E6C:('cmp','dword ptr [ebp - 8], 0x3e8'),
    0x1726E79:('imul','eax, dword ptr [ebp - 8], 0x3d8'),
    0x1726E80:('add','eax, 0xa4c1130'),
}
for address,instruction in expected.items():
    assert decoded.get(address)==instruction,(hex(address),decoded.get(address),instruction)
character_calls = (0x1400C87,0x140514C,0x14052F2,0x1405529,0x1405692,
                   0x1406095,0x14066F2,0x1407048,0x1407661)
for address in character_calls:
    assert decoded[address] == ('call','0x1316de7')
    assert decoded[address-6] == ('add','ecx, 0x3254')
    assert decoded[address-12] == ('mov','ecx, dword ptr [ebp - 0x1754]')
assert decoded[0x1400AD5] == ('call','0x13f8ed7')
assert decoded[0x1400ADD] == ('mov','dword ptr [ebp - 0x1754], eax')
assert decoded[0x13F8EEA] == ('cmp','dword ptr [ebp - 8], 0x190')
assert decoded[0x13F8F0C] == ('add','eax, 0x3254')
for address,value in ((0x1B4DF04,.1),(0x1B4DF08,.2),(0x1B4DD48,1.)):
    assert abs(struct.unpack_from('<f',data,address-IMAGE_BASE)[0]-value)<1e-7
root = Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE'
sprite = (root/'zzzeffectsprite.cpp').read_text(encoding='utf-8')
check = sprite.split('void CheckSprites()',1)[1]
assert 'if(o->Live)' in check and 'o->Visible = true;' in check
assert 'o->AnimationFrame += o->Visible ? .1f : -.1f;' in sprite
assert 'if( byRenderOneMore == 0 || byRenderOneMore == 2 )' in sprite
scene = (root/'ZzzScene.cpp').read_text(encoding='utf-8')
assert 'RenderEffects();\n\tRenderBlurs();\n\tCheckSprites();' in scene
print(f'PASS: {len(expected)} pinned instructions, three scalars and native visibility/order guards.')
print('PASS: container cleanup walks nodes, destroys payload+8, follows payload+4 pointer cleanup and resets links/count.')
print('PASS: atomic negative fetch-add returns prior count; timed payload insertion/update call sites pinned.')
print('PASS:18CC6A9 caller traverses a16x16 linked grid, distinct from1726E56 fixed1000 sprite pool. No Circle attribution from that caller.')
print('PASS: nine13F93C1 caller sites pass character-local+3254, first producer uses13F8ED7 character-pool lookup. Five other insertion callers unclassified.')
print('OPEN: payload semantics,concrete virtual targets and Circle-specific container writers; no bare Live=false equivalence claim.')
print('Numeric caller xrefs and source guards do not prove full scene scheduling or GPU parity.')
