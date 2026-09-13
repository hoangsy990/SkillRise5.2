"""Pinned controller transform/owner callsites; does not execute native pools."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    instructions = {i.address: i for i in decoder.disasm(
        data[0x1578288-IMAGE_BASE:0x1578657-IMAGE_BASE], 0x1578288)}
    pins = {
        0x1578295: ('cmp', 'eax, 0xbc'),
        0x1578334: ('mov', 'ecx, dword ptr [ebp + 8]'),
        0x1578337: ('add', 'ecx, 0x164'),
        0x15783AB: ('mov', 'ecx, dword ptr [ebp + 8]'),
        0x15783AE: ('add', 'ecx, 0x164'),
        0x15783EF: ('add', 'ecx, 0x158'),
        0x1578425: ('add', 'ecx, 0x158'),
        0x157845A: ('add', 'ecx, 0x158'),
        0x1578652: ('jmp', '0x15945f1'),
    }
    for address in (0x1578320, 0x1578495, 0x1578581, 0x1578613):
        pins[address] = ('push', 'dword ptr [ebp + 8]')
    for address, expected in pins.items():
        i = instructions[address]
        assert (i.mnemonic, i.op_str) == expected, hex(address)
    root = Path(__file__).resolve().parents[2]
    source = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start = source.index('case kMagicPinControllerModel:', source.index('void UpdateEffect'))
    body = source[start:source.index('case kMagicPin01Model:', start)]
    assert body.count('&effect, -1, 0, kMagicPinSkill') == 4
    assert 'effect.Owner->Angle' not in body and 'effect.Owner->Light' not in body
    assert 'TransformFromObject(effect, 0.0f, 20.0f, 30.0f,' in body
    move = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp').read_text(encoding='utf-8')
    assert 'o->Type == rise::growlancer::kMagicPinControllerModel ||' in move
    print('PASS pinned controller transform and four child-owner callsites; private controller whole-tick inclusion')
    child = list(decoder.disasm(data[0x15786D7-IMAGE_BASE:0x15788B6-IMAGE_BASE],0x15786D7))
    assert (child[-1].address,child[-1].mnemonic,child[-1].op_str)==(0x15788B1,'jmp','0x15945f1')
    assert all('0x34c' not in i.op_str for i in child)
    ci={i.address:(i.mnemonic,i.op_str) for i in child}
    for address,pair in {
        0x15787A4:('mov','ecx, dword ptr [ebp + 8]'),
        0x15787A7:('add','ecx, 0x164'),
        0x15787B3:('call','0xd30b1a'),
        0x15787CF:('call','0xd3189d'),
        0x15787E5:('mov','ecx, dword ptr [ebp + 8]'),
        0x15787E8:('add','ecx, 0x158'),
    }.items(): assert ci[address]==pair
    start=source.index('case kMagicPin03Model:',source.index('void UpdateEffect'))
    child_native=source[start:source.index('case kMagicPinRootModel:',start)]
    assert 'Owner' not in child_native
    assert 'AngleMatrix(effect.Angle, matrix);' in child_native
    assert 'VectorAdd(effect.Position, rotated, effect.Position);' in child_native
    print('PASS selected5E7 body uses self angle/position, no direct owner field; common engine prefix excluded')
    prefix={i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(
        data[0x14B77AC-IMAGE_BASE:0x14B7818-IMAGE_BASE],0x14B77AC)}
    for address,pair in {
        0x14B77AF:('cmp','dword ptr [eax + 0x34c], 0'),
        0x14B77B6:('je','0x14b780c'),
        0x14B77BB:('mov','eax, dword ptr [eax + 0x34c]'),
        0x14B77C1:('add','eax, 0x158'),
        0x14B77CA:('add','ecx, 0x1d0'),
        0x14B77D0:('call','0xd2df9e'),
        0x14B77FA:('call','0x17649da'),
        0x14B7809:('fstp','dword ptr [eax + 8]'),
    }.items(): assert prefix[address]==pair
    assert all('0x1d0' not in i.op_str for i in child)
    print('PASS common prefix DOES read non-null owner into snapshot1D0/terrainZ; selected5E7 does not directly consume snapshot')
    render=list(decoder.disasm(data[0xA49029-IMAGE_BASE:0xA49189-IMAGE_BASE],0xA49029))
    tail=list(decoder.disasm(data[0x15945F1-IMAGE_BASE:0x1594709-IMAGE_BASE],0x15945F1))
    assert (render[-1].address,render[-1].mnemonic)==(0xA49188,'ret')
    assert (tail[-1].address,tail[-1].mnemonic)==(0x1594708,'ret')
    assert all(not any(field in i.op_str for field in ('0x1d0','0x34c')) for i in render+tail)
    ri={i.address:(i.mnemonic,i.op_str) for i in render+tail}
    for address,pair in {
        0xA49173:('mov','ecx, dword ptr [ebp + 8]'),
        0xA49176:('call','0x968cb3'),
        0x15945FD:('cmp','dword ptr [ebp - 0x12464], 0x413'),
        0x1594609:('cmp','dword ptr [ebp - 0x12464], 0x7ef7'),
        0x159462B:('jg','0x159463b'),
        0x1594630:('call','0x14b6820'),
        0x159465F:('jmp','0x15946bb'),
        0x15946E9:('dec','eax'),
    }.items(): assert ri[address]==pair
    print('PASS complete5E7 callback/common tail have no direct snapshot/owner fields; mesh wrapper receives model')
    outer = {}
    for begin, end in ((0x15BD5DC, 0x15BD617), (0x176D621, 0x176D65D),
                       (0x188640C, 0x18864FC)):
        outer.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
            data[begin-IMAGE_BASE:end-IMAGE_BASE], begin)})
    for address, pair in {
        0x15BD5E2: ('cmp', 'dword ptr [eax + 0x5c], 0x40f'),
        0x15BD5F1: ('cmp', 'dword ptr [eax + 0x5c], 0x7a6'),
        0x15BD60A: ('call', '0x176d621'),
        0x176D630: ('call', '0x1886382'),
        0x176D63D: ('jne', '0x176d641'),
        0x176D653: ('call', '0x1887b8b'),
        0x1886419: ('call', '0xef2a81'),
        0x188645D: ('call', '0x1885eb0'),
        0x188648F: ('add', 'ecx, 0x158'),
        0x18864A4: ('add', 'ecx, 0x78'),
        0x18864B5: ('add', 'ecx, 0x158'),
        0x18864CA: ('add', 'ecx, 0x78'),
        0x18864DB: ('add', 'ecx, 0x158'),
        0x18864F0: ('add', 'ecx, 0x78'),
    }.items(): assert outer[address] == pair, hex(address)
    print('PASS outer route gates draw on preparation success; initial model origin uses effect Position158, not snapshot1D0')
    lighting = list(decoder.disasm(data[0x1885EB0-IMAGE_BASE:0x1886382-IMAGE_BASE], 0x1885EB0))
    assert (lighting[-1].address, lighting[-1].mnemonic) == (0x1886381, 'ret')
    assert all(not any(field in i.op_str for field in ('0x1d0', '0x34c')) for i in lighting)
    owner_gate = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[0x18872E6-IMAGE_BASE:0x188737E-IMAGE_BASE], 0x18872E6)}
    for address, pair in {
        0x18872E9: ('cmp', 'dword ptr [eax + 0x34c], 0'),
        0x18872F6: ('push', '0x3d'),
        0x1887313: ('push', '0x48'),
        0x1887330: ('push', '0xb2'),
        0x1887350: ('push', '0x118'),
        0x1887373: ('xorps', 'xmm0, xmm0'),
        0x1887376: ('movss', 'dword ptr [eax + 0xd0], xmm0'),
    }.items(): assert owner_gate[address] == pair
    native_prepare = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzObject.cpp').read_bytes()
    assert b'g_isCharacterBuff(o->Owner, eDeBuff_Stun) || g_isCharacterBuff(o->Owner, eDeBuff_Sleep)' in native_prepare
    print('PASS full lighting helper has no direct snapshot/owner access; common preparation separately tests four owner statuses')
    # Selected 0x5E7 bypasses both type-specific origins and both map-specific
    # blocks regardless of map: the latter also require type 0 or 0x1E.
    selected_route = {}
    for begin, end in ((0x188650E, 0x188651E), (0x18865E9, 0x18865F9),
                       (0x18866C4, 0x18866E7), (0x1886C96, 0x1886CB3)):
        selected_route.update({i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
            data[begin-IMAGE_BASE:end-IMAGE_BASE], begin)})
    for address, pair in {
        0x1886511: ('cmp', 'dword ptr [eax + 0x5c], 0x7e4'),
        0x1886518: ('jne', '0x18865e9'),
        0x18865EC: ('cmp', 'dword ptr [eax + 0x5c], 0x7e3'),
        0x18865F3: ('jne', '0x18866c4'),
        0x18866CB: ('je', '0x18866da'),
        0x18866D4: ('jne', '0x1886c96'),
        0x18866DD: ('cmp', 'dword ptr [eax + 0x5c], 0'),
        0x18866E1: ('jne', '0x1886c96'),
        0x1886CA0: ('jne', '0x18872e6'),
        0x1886CA9: ('cmp', 'dword ptr [eax + 0x5c], 0x1e'),
        0x1886CAD: ('jne', '0x18872e6'),
    }.items(): assert selected_route[address] == pair
    print('PASS selected5E7 reaches owner-status gate on every map after unmodified initial origin')
    print('Scope excludes later preparation branches, status translation, animation sampling, actual ownership/pool reuse and visual acceptance')


if __name__ == '__main__':
    main()
