"""Pinned S21 body registration, not equipped-model selection or visual parity."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)

    def decode(start, end):
        rows = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
        assert sum(i.size for i in rows) == end-start
        return {i.address: (i.mnemonic, i.op_str) for i in rows}

    def string(va):
        offset = va - IMAGE_BASE
        return data[offset:data.index(0, offset)].decode('ascii')

    body = decode(0x18CCBEC, 0x18CCDF7)
    assert body[0x18CCBF9] == ('cmp', 'dword ptr [ebp - 0x10], 0x10')
    assert body[0x18CCCAC] == ('movzx', 'eax, al')
    assert body[0x18CCCA6] == ('call', '0x14191b7')
    assert body[0x18CCCB1] == ('jne', '0x18ccd57')
    helper = decode(0x14191B7, 0x1419202)
    for call, compare, value in [(0x14191BF,0x14191C5,3), (0x14191CF,0x14191D5,4),
                                  (0x14191DF,0x14191E5,6), (0x14191EF,0x14191F5,7)]:
        assert helper[call] == ('call', '0x960fb1')
        assert helper[compare] == ('cmp', f'eax, {value}')
    assert helper[0x14191FA] == ('mov', 'al, 1')
    assert helper[0x14191FE] == ('xor', 'al, al')
    for address in (0x14191C8, 0x14191D8, 0x14191E8):
        assert helper[address] == ('je', '0x14191fa')
    assert helper[0x14191F8] == ('jne', '0x14191fe')
    assert helper[0x14191FC] == ('jmp', '0x1419200')
    assert decode(0x960FB1,0x960FBC)[0x960FB7] == ('and', 'eax, 0xf')
    assert string(0x1BB65E0) == '%s0%d.bmd'
    root = Path(r'D:\MU FICA Season 21\Data\Player')
    for start, tier, base in [(0x18CCC03,'',0x3703), (0x18CCCB7,'2',0x3713),
                               (0x18CCD57,'3',0x3723)]:
        for part, stem in enumerate(('Helm','Armor','Pant','Glove','Boot')):
            address = start + part*0x20
            assert body[address+3] == ('inc', 'eax')
            assert body[address+4] == ('push', 'eax')
            op, value = body[address+5]
            assert op == 'push' and string(int(value,16)) == stem+'Class'+tier
            assert string(int(body[address+10][1],16)).lower() == 'data\\player\\'
            assert body[address+18] == ('add', f'eax, {hex(base+part*0x50)}')
            assert body[address+24] == ('call', '0x18bbee0')
            if tier == '2':
                continue  # explicit helper excludes base7 before this block
            asset = root / (stem+'Class'+tier+'08.bmd')
            payload = asset.read_bytes()
            assert payload[:3] == b'BMD'
            print(f'{hex(base+part*0x50+7)} | {asset.name} | version={payload[3]} | '
                  f'{hashlib.sha256(payload).hexdigest()}')
    print('PASS15 registration call contracts; base7 skips Class2;10 asset hashes read only')
    print('OPEN: live body selector, higher tiers, texture dependencies, decode/load and native mapping')


if __name__ == '__main__':
    main()
