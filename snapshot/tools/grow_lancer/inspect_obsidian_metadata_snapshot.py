"""Read only the pinned offline skill metadata tree; no process access."""
import hashlib
import json
import struct
import argparse
from minidump.minidumpfile import MinidumpFile
from inspect_breche_full_dump import SNAPSHOT, PIN


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--include-internal', action='store_true',
                        help='also verify internal280/281 categories pinned from this snapshot')
    args = parser.parse_args()
    with SNAPSHOT.open('rb') as source:
        assert hashlib.file_digest(source, 'sha256').hexdigest() == PIN
    dump = MinidumpFile.parse(str(SNAPSHOT))
    assert dump.memory_segments_64
    reader = dump.get_reader()
    def u32(address):
        return struct.unpack('<I', reader.read(address, 4))[0]
    # 963291 supplies manager+108; 95ED56 walks root=head+4,
    # nil byte+D, key+10. 960B61/9632DF return inline value=node+14.
    manager = 0x70A218C
    head = u32(manager + 0x108)
    root = u32(head + 4)
    results = []
    expected_categories = {skill: int(skill in (272, 273, 278)) for skill in range(271, 280)}
    expected_categories.update({280: 0, 281: 0})
    for skill in range(271, 282 if args.include_internal else 280):
        node, seen, path = root, set(), []
        for _ in range(64):
            assert node and node not in seen, 'invalid/cyclic tree'
            seen.add(node)
            row = reader.read(node, 20)
            if row[13]:
                raise ValueError(f'missing metadata key{skill}; do not substitute fallback')
            key = struct.unpack_from('<i', row, 16)[0]
            path.append(key)
            if key == skill:
                record = node + 20
                fields = reader.read(record, 0x94)
                assert fields[0] == 0
                assert struct.unpack_from('<I', fields, 4)[0] == skill
                if skill in expected_categories:
                    assert struct.unpack_from('<I', fields, 0x90)[0] == expected_categories[skill]
                results.append(dict(key=skill, path=path, address=hex(record),
                                    byte0=fields[0], dword4=struct.unpack_from('<I', fields, 4)[0],
                                    dword90=struct.unpack_from('<I', fields, 0x90)[0]))
                break
            node = struct.unpack_from('<I', row, 0 if skill < key else 8)[0]
        else:
            raise ValueError('tree depth exceeded')
    print(json.dumps(dict(snapshotSha256=PIN, rows=results,
                          scope='offline metadata only; call-site key provenance still required'), indent=2))


if __name__ == '__main__':
    main()
