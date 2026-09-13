"""Pinned incremental-alpha contract, not native rendered acceptance."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    code={i.address:i for i in Cs(CS_ARCH_X86,CS_MODE_32).disasm(
        data[0x1578657-IMAGE_BASE:0x15786D7-IMAGE_BASE],0x1578657)}
    for address,pair in {
        0x157865F:('divss','xmm0, dword ptr [0x1b4e474]'),
        0x157867E:('jbe','0x15786a8'),
        0x1578693:('addss','xmm0, dword ptr [eax + 0xdc]'),
        0x15786C3:('subss','xmm1, xmm0'),
        0x15786D2:('jmp','0x15945f1'),
    }.items():
        assert (code[address].mnemonic,code[address].op_str)==pair
    root=Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE'
    runtime=(root/'RISE/GrowLancerEffectRuntime.cpp').read_text()
    helper=(root/'RISE/GrowLancerMagicPinTick.h').read_text()
    assert 'effect.Alpha += effect.LifeTime > 5.0f ? 0.2f : -0.2f;' in helper
    assert 'UpdateMagicPin01Tick(effect);' in runtime
    assert 'o->Type == rise::growlancer::kMagicPin01Model ||' in (root/'ZzzEffect.cpp').read_text(encoding='utf-8')
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    for start,end,address,pair in (
        (0x157890B,0x15789DD,0x157891C,('call','0x96a03c')),
        (0x96A03C,0x96A05B,0x96A04C,('call','0x1a22cc0')),
        (0x1A36D8F,0x1A36E22,0x1A36DAD,('fsin',''))):
        instructions={i.address:i for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start)}
        assert (instructions[address].mnemonic,instructions[address].op_str)==pair
    for address,value in ((0x1B4E4D8,30.),(0x1B4E4C8,.005),
                          (0x1B4DF18,.8),(0x1B500F8,.75)):
        assert abs(struct.unpack_from('<f',data,address-IMAGE_BASE)[0]-value)<.000001
    assert '-static_cast<float>(std::sin(30.0)) * 0.005f' in helper
    assert 'UpdateMagicPinAuxTick(effect);' in runtime
    print('PASS pinned incremental alpha and private whole-tick dispatch integration')
    print('Does not execute native update, renderer or pool.')

if __name__=='__main__': main()
