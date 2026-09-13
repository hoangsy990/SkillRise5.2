"""Pinned source lower/upper frame gates; regression against mistaken zero."""
from pathlib import Path
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
ins = {i.address:(i.mnemonic,i.op_str) for i in Cs(CS_ARCH_X86,CS_MODE_32).disasm(
    data[0x157DA18-IMAGE_BASE:0x157DA5E-IMAGE_BASE], 0x157DA18)}
for address, expected in {
    0x157DA26:('comiss','xmm0, dword ptr [0x1b4dd48]'),
    0x157DA2D:('jbe','0x157df45'),
    0x157DA39:('movss','xmm0, dword ptr [0x1b58d74]'),
    0x157DA48:('jb','0x157df45'),
    0x157DA51:('cmp','dword ptr [eax + 0x84], 0'),
    0x157DA58:('jne','0x157df45'),
}.items(): assert ins[address] == expected
assert struct.unpack_from('<f',data,0x1B4DD48-IMAGE_BASE)[0] == 1.0
assert struct.unpack_from('<f',data,0x1B58D74-IMAGE_BASE)[0] == 6.5
header = Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerClashTick.h'
assert 'counter == 0 && frame > 1.0f && frame <= 6.5f' in header.read_text()
print('PASS pinned Clash emission (1.0,6.5], counter0; not gameplay timing acceptance')
block = list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(
    data[0x157D996-IMAGE_BASE:0x157DF4A-IMAGE_BASE],0x157D996))
assert (block[-1].address,block[-1].mnemonic,block[-1].op_str) == (0x157DF45,'jmp','0x15945f1')
for local, addresses in (('0x11a08',[0x157D9BD,0x157D9C3]),
                         ('0x11a10',[0x157D9EC,0x157D9F2])):
    uses = [i for i in block if local in i.op_str]
    assert [i.address for i in uses] == addresses
    assert [i.mnemonic for i in uses] == ['mov','cmp']
    assert all(not (i.mnemonic=='lea' and local in i.op_str) for i in block)
print('PASS selected Clash block: both cached results stored/tested only, no later direct local use')
print('Scope: does not exclude side effects inside lookup or arbitrary external access')
runtime = header.with_name('GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8-sig')
body = runtime.split('case kClashControllerModel:',2)[2].split('case kClashFrontModel:',1)[0]
assert body.index('if (!CanPlayAction(*effect.Owner, 293))') < body.index('if (ClashEmissionDue(')
assert 'if (!CanPlayAction(*effect.Owner, 293))\n            return;' in body
assert body.index('if (!CanPlayAction(*effect.Owner, 293))') < body.index('effect.AttackPoint[0] = 1;')
print('PASS native Clash rechecks mapped owner action before emission without consuming counter')
