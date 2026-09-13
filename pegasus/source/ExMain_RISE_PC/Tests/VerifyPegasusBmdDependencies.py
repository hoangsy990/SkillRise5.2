"""Verify every staged Pegasus BMD can resolve its embedded sibling textures."""

from __future__ import annotations

import sys
from pathlib import Path

from InspectPegasusBmd import decode


def embedded_textures(path: Path) -> list[str]:
    import struct

    data = decode(path)
    cursor = 36
    meshes, _bones, _actions = struct.unpack_from("<3h", data, cursor)
    cursor += 6
    textures: list[str] = []
    for _ in range(meshes):
        vertices, normals, uvs, triangles, _texture = struct.unpack_from(
            "<5h", data, cursor
        )
        cursor += 10 + vertices * 16 + normals * 20 + uvs * 8 + triangles * 64
        raw_name = data[cursor : cursor + 32]
        cursor += 32
        textures.append(raw_name.split(b"\0", 1)[0].decode("latin1"))
    return textures


def encoded_name(texture: str) -> str:
    source = Path(texture)
    extension = source.suffix.lower()
    encoded_extension = {".jpg": ".ozj", ".jpeg": ".ozj", ".tga": ".ozt", ".bmp": ".ozb"}.get(
        extension, extension
    )
    return source.stem + encoded_extension


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: VerifyPegasusBmdDependencies.py RECOVERED_DATA_ROOT")
    root = Path(sys.argv[1]).resolve()
    missing: list[str] = []
    checked = 0
    for model in sorted(root.rglob("*.bmd")):
        sibling_names = {entry.name.lower() for entry in model.parent.iterdir() if entry.is_file()}
        for texture in embedded_textures(model):
            checked += 1
            expected = encoded_name(texture)
            if expected.lower() not in sibling_names:
                missing.append(f"{model.relative_to(root)} -> {texture} ({expected})")
    if missing:
        print("FAIL: unresolved Pegasus BMD embedded textures:")
        for entry in missing:
            print(f"  {entry}")
        return 1
    print(f"PASS: {checked} embedded texture references resolve beside staged BMDs")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
