"""Read pinned offline BuffEffect rows; never attach to or modify a process."""
import hashlib
import json
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from minidump.minidumpfile import MinidumpFile
from inspect_breche_full_dump import SNAPSHOT, PIN
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    with SNAPSHOT.open('rb') as source:
        assert hashlib.file_digest(source, 'sha256').hexdigest() == PIN
    baseline = DUMP.read_bytes()
    assert hashlib.sha256(baseline).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    snapshot = MinidumpFile.parse(str(SNAPSHOT))
    assert snapshot.memory_segments_64
    reader = snapshot.get_reader()
    # Cross-snapshot code equality before using addresses/layout from main dump.
    for start, end in ((0x113B50E, 0x113B572), (0x113B5F8, 0x113B611),
                       (0x113C254, 0x113C2E5), (0x113CBA7, 0x113CBAF),
                       (0x113E263, 0x113E28C), (0x113EF75, 0x113EF83),
                       (0x113B27E, 0x113B48B), (0xC888AB, 0xC8899A),
                       (0x8E7C00, 0x8E7C0C), (0x9605E2,0x960602),
                       (0x94B987,0x94B999), (0x94FF34,0x94FFE8)):
        assert reader.read(start, end-start) == baseline[start-IMAGE_BASE:end-IMAGE_BASE], hex(start)
    block = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(
        baseline[0xC888AB-IMAGE_BASE:0xC8899A-IMAGE_BASE], 0xC888AB))
    assert sum(i.size for i in block) == 0xEF and block[-1].mnemonic == 'ret'
    decoded = {i.address: (i.mnemonic, i.op_str) for i in block}
    for address, expected in {
        0xC888DB: ('call', '0x113b5f8'),
        0xC8890F: ('cmp', 'dword ptr [ebp - 0x18], 1'),
        0xC88913: ('jne', '0xc8891b'),
        0xC88915: ('mov', 'byte ptr [ebp - 0x14], 1'),
        0xC8891B: ('mov', 'byte ptr [ebp - 0x14], 0'),
        0xC88922: ('push', '0x1b6cbc0'),
        0xC88933: ('cmp', 'dword ptr [ebp - 0x18], 2'),
        0xC88946: ('push', '0x1b6cbcc'),
    }.items():
        assert decoded[address] == expected
    for address, label in ((0x1B6CBC0, b'isDebuff\0'), (0x1B6CBCC, b'isCashBuff\0')):
        assert baseline[address-IMAGE_BASE:address-IMAGE_BASE+len(label)] == label
        assert reader.read(address, len(label)) == label
    def u32(address):
        return struct.unpack('<I', reader.read(address, 4))[0]
    def string_text(address):
        size, capacity = u32(address + 0x10), u32(address + 0x14)
        assert size <= capacity < 4096, 'unbounded string'
        pointer = u32(address) if capacity >= 16 else address
        raw = reader.read(pointer, size + 1)
        assert raw[-1] == 0 and b'\0' not in raw[:-1]
        return raw[:-1].decode('utf-8')
    manager = 0x70A22F8
    head = u32(manager + 0x38)
    count = u32(manager + 0x3C)
    assert head and 0 < count <= 460
    results = []
    for buff in (1, 56, 424, 425):
        node, seen, path = u32(head + 4), set(), []
        for _ in range(64):
            assert node and node not in seen, 'null/cyclic node'
            seen.add(node)
            row = reader.read(node, 20)
            assert not row[13], f'missing buff {buff}; no fallback substitution'
            key = struct.unpack_from('<i', row, 16)[0]
            path.append(key)
            if key == buff:
                record = node + 20
                fields = reader.read(record, 0x44)
                assert struct.unpack_from('<i', fields)[0] == buff
                assert struct.unpack_from('<i', fields, 0x28)[0] == (1 if buff == 56 else 0)
                if buff in (424, 425):
                    assert struct.unpack_from('<4i', fields) == (buff, 123, 255, 255)
                    assert struct.unpack_from('<3i', fields, 0x28) == (0, 0, 1)
                    expected_name = 'Thịnh Nộ' if buff == 424 else 'Tăng Cường Thịnh Nộ'
                    assert string_text(record + 0x10) == expected_name
                begin, finish, capacity = struct.unpack_from('<3I', fields, 0x34)
                assert begin <= finish <= capacity and (finish-begin) % 24 == 0
                assert (capacity-begin) % 24 == 0 and capacity-begin <= 24*64
                descriptions = [string_text(p) for p in range(begin,finish,24)]
                if buff in (424,425):
                    assert descriptions == ['Tăng sức tấn công và sức chiến đấu. Phòng thủ giảm.']
                results.append(dict(buff=buff, path=path, record=hex(record),
                    name=string_text(record + 0x10),
                    descriptions=descriptions,
                    firstFourDwords=struct.unpack_from('<4i', fields),
                    classifier=struct.unpack_from('<i', fields, 0x28)[0],
                    column6=struct.unpack_from('<i', fields, 0x2C)[0],
                    column7=struct.unpack_from('<i', fields, 0x30)[0],
                    column9=struct.unpack_from('<i', fields, 0x40)[0]))
                break
            node = struct.unpack_from('<I', row, 0 if buff < key else 8)[0]
        else:
            raise ValueError('tree depth exceeded')
    print(json.dumps(dict(snapshotSha256=PIN, manager=hex(manager), head=hex(head),
        count=count, rows=results, scope='offline rows, not runtime or enum semantics'), indent=2))


if __name__ == '__main__':
    main()
