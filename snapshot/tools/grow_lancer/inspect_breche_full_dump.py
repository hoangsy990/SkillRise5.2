"""Pinned OFFLINE cache snapshot; no process access or cast-timing claims."""
from pathlib import Path
import hashlib
import json
import struct
import argparse
from minidump.minidumpfile import MinidumpFile

SNAPSHOT = Path(r'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\artifacts\grow_lancer\full_dump\s21-21072-a2dc6fe4f8124ea1a22827e774d6bb06.dmp')
PIN = '374d1ecb06719389416938e0be27a32596e91e0a5b5d5a7c6f8df690c326efed'


def main(model_id=0x5FD):
    if model_id not in (0x5FD, 0x5FA):
        raise ValueError('Only evidence-scoped invisible roots are supported')
    with SNAPSHOT.open('rb') as file:
        if hashlib.file_digest(file, 'sha256').hexdigest() != PIN:
            raise ValueError('Snapshot changed')
    dump = MinidumpFile.parse(str(SNAPSHOT))
    reader = dump.get_reader()
    if not dump.memory_segments_64:
        raise ValueError('Expected full-memory stream')
    def u32(address):
        return struct.unpack('<I', reader.read(address, 4))[0]
    # 968B9D dereferences singleton; EF2A81 supplies manager+4 to map find.
    manager = u32(0x1E04C8C)
    head = u32(manager + 4)
    node = u32(head + 4)
    seen = set()
    path = []
    for _ in range(64):
        if not node or node in seen:
            raise ValueError('Null/cyclic tree node')
        seen.add(node)
        row = reader.read(node, 24)
        if row[13]:
            raise ValueError(f'Root {model_id:X} absent in pinned snapshot')
        key = struct.unpack_from('<i', row, 16)[0]
        path.append(key)
        if key == model_id:
            model = struct.unpack_from('<I', row, 20)[0]
            data = reader.read(model, 0xD0)
            bones, meshes, actions = struct.unpack_from('<hhh', data, 0x22)
            if (bones, meshes, actions) != (0, 0, 0):
                raise ValueError('Root model counts changed')
            print(json.dumps(dict(snapshot=str(SNAPSHOT), sha256=PIN,
                manager=hex(manager), mapCount=u32(manager+8), searchKeys=path,
                modelId=hex(model_id), rootModel=hex(model), bones=bones, meshes=meshes, actions=actions,
                scope=f'{model_id:X} cached model has no geometry/actions at this snapshot; not effect timing or parity'), indent=2))
            return
        node = struct.unpack_from('<I', row, 0 if model_id < key else 8)[0]
    raise ValueError('Tree depth exceeded')


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--model', type=lambda value: int(value, 0), default=0x5FD,
                        choices=(0x5FD, 0x5FA))
    main(parser.parse_args().model)
