"""Verify selected S21 action branches and source clips, not native parity."""
import hashlib
import struct
import pefile
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE
import compare_player_actions as clips
import inspect_breche_item_models as items


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    pe = pefile.PE(data=data, fast_load=True)
    pe.parse_data_directories(directories=[pefile.DIRECTORY_ENTRY['IMAGE_DIRECTORY_ENTRY_IMPORT']])
    clock_imports = [(entry.dll.lower(), symbol.name)
                     for entry in pe.DIRECTORY_ENTRY_IMPORT for symbol in entry.imports
                     if symbol.address == 0x1B4C4A0]
    assert clock_imports == [(b'kernel32.dll', b'GetTickCount')]
    pe.close()
    # Model only the decoded DWORD add / unsigned jae; not a live timer test.
    expires = lambda saved, now: ((saved + 3000) & 0xFFFFFFFF) < now
    assert not expires(1000, 3999)
    assert not expires(1000, 4000)
    assert expires(1000, 4001)
    assert expires(0xFFFFFF00, 0xFFFFFF01)  # Source wraps the deadline addition.
    assert not expires(0xFFFFFF00, 2744)
    assert expires(0xFFFFFF00, 2745)
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    for table, setup in ((0x1B879D0, 0x1098DEE), (0x1B87A14, 0x1099B2F),
                         (0x1B87A58, 0x109A909)):
        assert struct.unpack_from('<I', data, table-IMAGE_BASE+0x38)[0] == setup
        assert struct.unpack_from('<I', data, table-IMAGE_BASE+0x10)[0] == 0x1097AFE
    assert struct.unpack_from('<f', data, 0x1B50100-IMAGE_BASE)[0] == struct.unpack('<f', struct.pack('<f', .85))[0]
    instructions = {}
    for start, end in ((0x10E46D5, 0x10E46F5), (0x10E34E2, 0x10E36D7), (0x1327DE8, 0x1327E57),
                       (0x9C54AA, 0x9C5537), (0x10C352E, 0x10C35DC),
                       (0x10C64EC, 0x10C64FC), (0x1314FAE, 0x1314FC1),
                       (0xD064B4, 0xD064FC), (0xD052BA, 0xD05397),
                       (0x108A1BE, 0x108A430), (0x96139E, 0x9613AD),
                       (0x10C34A5, 0x10C34ED), (0x140DC35, 0x140DCC4),
                       (0x12C5540, 0x12C55FD), (0x12CB59E, 0x12CB5CA),
                       (0x133E87B, 0x133E8E5), (0x1424C07, 0x1424C54),
                       (0x13F3EF8, 0x13F3FB0), (0x133607C, 0x13360DE),
                       (0x133E84C, 0x133E87B), (0x961566, 0x961576),
                       (0x13164A0, 0x1316565), (0x132F433, 0x132F46D),
                       (0x132F54F, 0x132F56F), (0x1097AFE, 0x1097C15),
                       (0x1098DEE, 0x1098E55), (0x1099B2F, 0x1099B96),
                       (0x109A909, 0x109A970), (0x1098102, 0x1098137)):
        instructions.update((i.address, (i.mnemonic, i.op_str)) for i in
                            decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    pins = {
        0x1098E00: ('mov', 'dword ptr [eax + 0x10], 0x73f'),
        0x1099B41: ('mov', 'dword ptr [eax + 0x10], 0x740'),
        0x109A91B: ('mov', 'dword ptr [eax + 0x10], 0x741'),
        0x1098E1C: ('mov', 'dword ptr [ecx + 0x20], eax'),
        0x1099B5D: ('mov', 'dword ptr [ecx + 0x20], eax'),
        0x109A937: ('mov', 'dword ptr [ecx + 0x20], eax'),
        0x1098117: ('movss', 'dword ptr [eax + 0xa0], xmm0'),
        0x1097B2A: ('push', '0'),
        0x1097B3C: ('push', '1'),
        0x1097B49: ('call', '0x13164a0'),
        0x1097B4E: ('push', 'eax'),
        0x1097B55: ('call', '0x132f433'),
        0x1316520: ('cmp', 'dword ptr [eax + 0x318], 0'),
        0x131652D: ('add', 'eax, 0x70b5400'),
        0x131653B: ('add', 'eax, dword ptr [ecx + 0x318]'),
        0x131655F: ('xor', 'eax, eax'),
        0x132F447: ('cmp', 'dword ptr [ebp + 8], 0'),
        0x132F44D: ('jmp', '0x132f560'),
        0x132F458: ('je', '0x132f54f'),
        0x132F558: ('call', '0xd31aa2'),
        0x13360BB: ('push', 'dword ptr [ebp + 0x1c]'),
        0x13360BE: ('call', '0x968b22'),
        0x13360C5: ('call', '0xef2a81'),
        0x13360CA: ('mov', 'dword ptr [ebp - 0xb7b8], eax'),
        0x133E84C: ('cmp', 'dword ptr [ebp + 0x1c], 0x5c0'),
        0x133E853: ('je', '0x133e862'),
        0x133E855: ('cmp', 'dword ptr [ebp + 0x1c], 0x5c1'),
        0x133E85C: ('jne', '0x133e8e5'),
        0x133E86F: ('call', '0x961566'),
        0x133E879: ('jne', '0x133e8d6'),
        0x961570: ('movzx', 'eax, byte ptr [eax + 4]'),
        0x13F3F0A: ('call', '0x96139e'),
        0x13F3F14: ('je', '0x13f3fae'),
        0x13F3F8D: ('call', '0x1424b34'),
        0x1424C14: ('call', '0xef2a81'),
        0x1424C26: ('jne', '0x1424c2d'),
        0x1424C28: ('jmp', '0x142ed12'),
        0x1424C30: ('cmp', 'dword ptr [eax + 0x5c], 0xa94'),
        0x1424C37: ('jne', '0x1424c54'),
        0x1424C44: ('push', 'dword ptr [ebp + 8]'),
        0x1424C4E: ('call', '0x10c34a5'),
        0x133E87E: ('movsx', 'eax, byte ptr [eax + 0x38]'),
        0x133E885: ('jne', '0x133e8d6'),
        0x133E891: ('call', '0x10c2f89'),
        0x133E89B: ('je', '0x133e8c4'),
        0x133E8A7: ('call', '0x10c34a5'),
        0x133E8B1: ('jne', '0x133e8c4'),
        0x133E8BB: ('mov', 'word ptr [eax + 0xa8], cx'),
        0x133E8CD: ('mov', 'word ptr [eax + 0xa8], cx'),
        0x10E46D5: ('push', '0x13d'), 0x10E46DA: ('push', '0xbf'),
        0x10E46F0: ('call', '0x1327de8'),
        0x10E34E8: ('mov', 'eax, dword ptr [eax + 0xc]'),
        0x10E3567: ('cmp', 'dword ptr [ebp - 0xf568], 0x111'),
        0x10E3571: ('je', '0x10e455f'),
        0x10E360D: ('cmp', 'dword ptr [ebp - 0xf568], 0x2b5'),
        0x10E3617: ('je', '0x10e455f'),
        0x10E46E5: ('push', 'dword ptr [eax + 0xc]'),
        0x12C5540: ('mov', 'eax, dword ptr [ebp - 0x1950]'),
        0x12C55D1: ('sub', 'eax, 0xc9'),
        0x12C55EF: ('movzx', 'eax, byte ptr [eax + 0x12ce8ca]'),
        0x12C55F6: ('jmp', 'dword ptr [eax*4 + 0x12ce85a]'),
        0x12CB5B6: ('push', 'dword ptr [ebp - 0x1950]'),
        0x12CB5C2: ('call', '0x1327de8'),
        0x9C5525: ('mov', 'eax, dword ptr [0x1e04d80]'),
        0x9C5536: ('ret', ''),
        0x1327E0D: ('call', '0x10c352e'),
        0x1327E25: ('jne', '0x1327e42'),
        0x1327E27: ('cmp', 'dword ptr [ebp + 0x14], -1'),
        0x1327E2D: ('push', 'dword ptr [ebp + 0x14]'),
        0x1327E42: ('push', 'dword ptr [ebp + 0x10]'),
        0x10C3552: ('call', '0x10c34ed'),
        0x10C3565: ('movsx', 'eax, word ptr [eax + 0x3e]'),
        0x10C35AA: ('call', '0x10c64ec'),
        0x10C64F6: ('movzx', 'eax, byte ptr [eax + 0x34]'),
        0x10C35B1: ('push', '0x40'),
        0x1314FB8: ('movzx', 'eax, word ptr [eax + 0x322e]'),
        0xD064E6: ('movzx', 'eax, byte ptr [eax + 0x251]'),
        0xD064ED: ('cmp', 'eax, dword ptr [ebp + 0xc]'),
        0xD064F9: ('ret', '8'),
        0xD052FA: ('call', '0x97900e'),
        0xD05301: ('push', '0x2c4'),
        0xD05367: ('call', '0x19e97c0'),
        0xD0537B: ('call', '0xd07635'),
        0x108A3DC: ('movzx', 'eax, byte ptr [eax + 0x31e8]'),
        0x108A3FA: ('call', '0x96139e'),
        0x9613A8: ('movzx', 'eax, byte ptr [eax]'),
        0x108A402: ('and', 'eax, dword ptr [ebp - 8]'),
        0x108A408: ('mov', 'byte ptr [ecx + 0x34], al'),
        0x10C34C4: ('add', 'esi, 0xbb8'),
        0x10C34D2: ('jae', '0x10c34de'),
        0x10C34D7: ('mov', 'byte ptr [eax + 0x31e8], 0'),
        0x140DCAE: ('mov', 'byte ptr [eax + 0x31e8], 1'),
        0x140DCBE: ('mov', 'dword ptr [ecx + 0x31ec], eax'),
    }
    for address, expected in pins.items():
        assert instructions[address] == expected, (hex(address), instructions[address])
    selector = data[0x12CE8CA - IMAGE_BASE + 273 - 201]
    assert selector == 23
    assert struct.unpack_from('<I', data, 0x12CE85A - IMAGE_BASE + selector * 4)[0] == 0x12CB59E
    source = Path(r'D:\MU FICA Season 21\Data\Player\Player.bmd')
    payload, version = clips.decode_container(source, clips.load_converter())
    skeleton = clips.parse_zero_mesh_skeleton(payload)
    assert (version, skeleton.bone_count, skeleton.action_count) == ('v0F', 60, 410)
    expected_hashes = {
        191: '7322FFFFBE60A7A6F85D8CA2521E440B40A8C0896D57236BAF2EB34144C5F79D',
        317: '9641BE400ABD1039026335A2AED8B9CB96C0E27325B89138FE7E6C72779CC5F7',
    }
    for action, expected in expected_hashes.items():
        assert skeleton.key_counts[action] == 10
        assert not skeleton.lock_positions[action]
        assert skeleton.action_hashes[action] == expected
    table = items.TABLE.read_bytes()
    assert hashlib.sha256(table).hexdigest() == '9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6'
    count = struct.unpack_from('<I', table)[0]
    assert len(table) == 8 + count * 708
    assert items.checksum(table[4:-4]) == struct.unpack_from('<I', table, len(table)-4)[0]
    selected = set()
    for ordinal in range(count):
        encoded = table[4+ordinal*708:4+(ordinal+1)*708]
        row = bytes(value ^ b'\xfc\xcf\xab'[i % 3] for i, value in enumerate(encoded))
        if row[0x251] == 64:
            identity = divmod(struct.unpack_from('<I', row)[0], 512)
            assert identity not in selected
            selected.add(identity)
    assert selected == {(group, index) for group in (13, 16)
                        for index in (231, 232, 326, 327, 328, 329, 330, 331)}
    print('PASS selected alternate-action branch pins; distinct source clips191/317, 10 keys each')
    print('PASS pinned item table checksum and exact16 category64 identities')
    print('PASS pinned GetTickCount IAT identity and six unsigned deadline boundary cases; no native activation')
    print('OPEN: predicate semantic identity, native state mapping and live animation parity')


if __name__ == '__main__':
    main()
