"""Pinned selected Shockwave1 prefix/consumer evidence, not global liveness proof."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    def decode(start, end):
        return {i.address: i for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
    prefix = decode(0x16AA743, 0x16AA7D2)
    assert prefix[0x16AA75F].op_str == 'eax, dword ptr [eax + 0x50]'
    assert prefix[0x16AA762].op_str == 'eax, 0x158'
    assert prefix[0x16AA773].op_str == 'eax, 0x54'
    assert prefix[0x16AA7AE].op_str == '0x17649da'
    zeros = decode(0x1641638, 0x1641692)
    movement = decode(0x13272A0, 0x1327352)
    assert movement[0x13272B7].op_str == '0xd30b1a'
    assert movement[0x13272C9].op_str == '0xd3189d'
    assert sum(i.mnemonic == 'addss' for i in movement.values()) == 3
    rotate = decode(0xD3189D, 0xD31A0E)
    assert sum(i.mnemonic == 'mulss' for i in rotate.values()) == 9
    assert sum(i.mnemonic == 'addss' for i in rotate.values()) == 6
    assert not any(i.mnemonic == 'call' for i in rotate.values())
    for address in (0xD31916, 0xD31990, 0xD31A06):
        assert rotate[address].op_str == 'dword ptr [ecx + eax], xmm0'
    for address in (0x164164F, 0x164166D, 0x164168B):
        assert zeros[address].mnemonic == 'xorps'
    for address in (0x1641645, 0x1641663, 0x1641681):
        assert zeros[address].op_str == 'eax, 0x70'
    update = decode(0x1705379, 0x17054F3)
    assert update[0x1705399].op_str == 'dword ptr [eax + 0x50], 0'
    render_select = decode(0x16219C7, 0x1621A1A)
    # subtype1 is neither7 nor3..6 and therefore uses the default sprite.
    assert render_select[0x1621A15].op_str == '0x1621cf1'
    render = decode(0x1621CF1, 0x1621D9A)
    assert render[0x1621D42].op_str == 'eax, 0x38'
    assert render[0x1621D74].op_str == 'eax, 0x20'
    assert render[0x1621D8D].op_str == '0x18e8b0c'
    for body in (update, render):
        assert not any('0x54' in i.op_str for i in body.values())
        assert not any(i.mnemonic == 'mov' and i.op_str == 'eax, dword ptr [eax + 0x50]' for i in body.values())
    print('PASS prefix owner-position snapshot and zero velocity; selected Shockwave1 update/render do not consume snapshot')
    print('PASS zero-velocity movement structure: rotate then add, no translation term in vector rotation')
    print('Does not prove all external consumers, nonfinite-angle behavior, pool liveness or GPU parity')


if __name__ == '__main__':
    main()
