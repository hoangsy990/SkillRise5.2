"""Selected pinned Obsidian alpha and native whole-tick inclusion."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    ins={i.address:i for i in decoder.disasm(data[0x1578113-IMAGE_BASE:0x1578288-IMAGE_BASE],0x1578113)}
    pins={
        0x1578156:('sar','eax, 1'),0x157815C:('subss','xmm0, xmm1'),
        0x1578179:('jle','0x15781a8'),0x157818B:('divss','xmm0, dword ptr [ebp - 0x118a8]'),
        0x15781B0:('divss','xmm0, dword ptr [ebp - 0x118a8]'),
        0x15781CB:('jmp','0x15945f1'),0x15781F1:('cmp','eax, 0x13d'),
        0x1578244:('addss','xmm0, dword ptr [eax + 0xdc]'),
        0x1578274:('subss','xmm1, xmm0'),0x1578283:('jmp','0x15945f1'),
    }
    for address,pair in pins.items():
        assert (ins[address].mnemonic,ins[address].op_str)==pair,hex(address)
    root=Path(__file__).resolve().parents[2]
    source=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start=source.index('case kObsidianRotatingModel:',source.index('void UpdateEffect'))
    body=source[start:source.index('case kClashControllerModel:',start)]
    assert 'UpdateObsidianRotatingTick(effect)' in body and 'SymmetricAlpha' not in body
    assert 'UpdateObsidianAuraTick(effect)' in body
    helper=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerObsidianTick.h').read_text()
    assert '(e.LifeTime>7.f ? 15.f-e.LifeTime : e.LifeTime)/8.f*.9f' in helper
    assert 'if (effect.Alpha <' not in body and 'if (effect.Alpha >' not in body
    move=(root/'ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp').read_text(encoding='utf-8')
    for name in ('kObsidianRotatingModel','kObsidianAuraModel'):
        assert f'o->Type == rise::growlancer::{name} ||' in move
    print('PASS pinned integer-half denominator, unclamped aura alpha and private tick inclusion')
    print('Scope excludes native execution, second action317 mapping, particles and visual acceptance')


if __name__=='__main__':main()
