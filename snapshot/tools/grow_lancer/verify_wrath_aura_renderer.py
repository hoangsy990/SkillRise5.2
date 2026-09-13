"""Selected base Wrath aura render arguments, not GPU acceptance."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    ins={i.address:i for i in decoder.disasm(data[0xA49554-IMAGE_BASE:0xA497AD-IMAGE_BASE],0xA49554)}
    pins={
        0xA49676:('push','0'),0xA49689:('push','0x42'),
        0xA4968B:('push','0'),0xA49690:('call','0x968cb3'),
        0xA4978B:('push','0'),0xA4979E:('push','0x42'),
        0xA497A0:('push','0'),0xA497A5:('call','0x968cb3'),
        0xA496AD:('add','ecx, 0xec'),0xA496E7:('add','ecx, 0xec'),
        0xA49721:('add','ecx, 0xec'),
    }
    for address,pair in pins.items():
        assert (ins[address].mnemonic,ins[address].op_str)==pair,hex(address)
    # Subtype1 replaces BodyLight from effect RGB*alpha; it does not multiply
    # the old model light as the earlier prose could incorrectly suggest.
    for address in (0xA496BB,0xA496F5,0xA4972F):
        assert ins[address].op_str=='xmm0, dword ptr [ecx + 0xdc]'
    root=Path(__file__).resolve().parents[2]
    source=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    begin=source.index('if (effect.Type == kWrathAuraModel || effect.Type == kWrathTravelModel)')
    body=source[begin:source.index('if (effect.Type == kCircleShieldModel)',begin)]
    for axis in range(3):
        assert f'model.BodyLight[{axis}] = effect.Light[{axis}] * effect.Alpha;' in body
    assert '0, effect.Alpha, travel ? 0.0f : effect.BlendMeshTexCoordU' in body
    print('PASS aura0/1 mesh0 flags42 blend0 alpha/UV and subtype1 RGB replacement')
    print('Scope excludes subtype2, GPU, model transform and buff/pool runtime')


if __name__=='__main__':main()
