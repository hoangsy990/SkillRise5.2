"""Hash/report helper for Pegasus BMD v0A/v0C model dependencies."""

from __future__ import annotations

import hashlib
import struct
import sys
from pathlib import Path


XOR_KEY = (0xD1, 0x73, 0x52, 0xF6, 0xD2, 0x9A, 0xCB, 0x27,
           0x3E, 0xAF, 0x59, 0x31, 0x37, 0xB3, 0xE7, 0xA2)


def decode(path: Path) -> bytes:
    data = path.read_bytes()
    if data[:4] != b"BMD\x0c":
        if data[:4] != b"BMD\x0a":
            raise ValueError(f"{path}: unsupported BMD signature {data[:4]!r}")
        return data
    encrypted_size = struct.unpack_from("<I", data, 4)[0]
    encrypted = data[8:8 + encrypted_size]
    if len(encrypted) != encrypted_size:
        raise ValueError(f"{path}: truncated encrypted payload")
    previous = 0x5E
    decoded = bytearray(encrypted_size)
    for index, value in enumerate(encrypted):
        decoded[index] = ((value ^ XOR_KEY[index % len(XOR_KEY)]) - previous) & 0xFF
        previous = (value + 0x3D) & 0xFF
    return b"BMD\x0a" + decoded


def inspect(path: Path) -> None:
    data = decode(path)
    cursor = 36
    meshes, bones, actions = struct.unpack_from("<3h", data, cursor)
    cursor += 6
    mesh_rows = []
    textures: list[str] = []
    for mesh_index in range(meshes):
        vertices, normals, uvs, triangles, texture_index = struct.unpack_from(
            "<5h", data, cursor)
        bounds = None
        if vertices:
            positions = [struct.unpack_from('<3f', data, cursor + 10 + i * 16 + 4)
                         for i in range(vertices)]
            bounds = tuple((min(p[axis] for p in positions),
                            max(p[axis] for p in positions))
                           for axis in range(3))
        mesh_rows.append((mesh_index, vertices, normals, triangles,
                          texture_index, bounds))
        cursor += 10 + vertices * 16 + normals * 20 + uvs * 8 + triangles * 64
        raw_name = data[cursor:cursor + 32]
        cursor += 32
        textures.append(raw_name.split(b"\0", 1)[0].decode("latin1"))
    print(f"{path} sha256={hashlib.sha256(path.read_bytes()).hexdigest().upper()}")
    print(f"meshes={meshes} bones={bones} actions={actions}")
    for mesh_index, vertices, normals, triangles, texture_index, bounds in mesh_rows:
        print(f"mesh={mesh_index} vertices={vertices} normals={normals} "
              f"triangles={triangles} texture_index={texture_index} "
              f"texture={textures[mesh_index]} bounds={bounds}")


def main() -> int:
    if len(sys.argv) < 2:
        raise SystemExit("usage: InspectPegasusBmd.py MODEL [MODEL ...]")
    for value in sys.argv[1:]:
        inspect(Path(value))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
