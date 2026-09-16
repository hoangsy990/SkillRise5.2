#!/usr/bin/env python3
"""Read-only check of the S21 first model-render map in an owner-pinned dump.

The Grow Lancer task's one authorized full-memory snapshot stays local: this
script neither launches the protected client nor exports process memory. The
result applies to this snapshot, not to later mutable runtime maps or every
other special-render helper in the native wrapper.
"""

from __future__ import annotations

import hashlib
import struct
from pathlib import Path

from minidump.minidumpfile import MinidumpFile


FULL_DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\artifacts\grow_lancer"
    r"\full_dump\s21-21072-a2dc6fe4f8124ea1a22827e774d6bb06.dmp"
)
FULL_DUMP_SHA256 = "374D1ECB06719389416938E0BE27A32596E91E0A5B5D5A7C6F8DF690C326EFED"
MAIN_IMAGE = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
MAIN_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
MAP_GLOBAL = 0x1FAEB00
EXPECTED_COUNT = 4950
CYLINDER_NATIVE_TYPE = 0x5D8
MAP_KEY_BIAS = 0xAE9


def digest(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest().upper()


def main() -> int:
    if digest(FULL_DUMP) != FULL_DUMP_SHA256 or digest(MAIN_IMAGE) != MAIN_SHA256:
        raise AssertionError("S21 offline snapshot or mapped main identity drifted")
    dump = MinidumpFile.parse(str(FULL_DUMP))
    if not dump.memory_segments_64:
        raise AssertionError("S21 full-memory stream absent")
    reader = dump.get_reader()
    image = MAIN_IMAGE.read_bytes()
    # Code agreement ties the two process snapshots to the same native
    # first-manager and Pierce-child build, without comparing mutable .data.
    for va, length in ((0x18917F3, 18), (0x1887DDB, 16),
                       (0x147D97C, 5), (0x15AE9A2, 12)):
        if reader.read(va, length) != image[va - 0x400000:va - 0x400000 + length]:
            raise AssertionError(f"S21 code anchor differs at {va:#x}")
    head, count = struct.unpack("<II", reader.read(MAP_GLOBAL, 8))
    if count != EXPECTED_COUNT:
        raise AssertionError(f"S21 first-manager map size drifted: {count}")
    root = struct.unpack("<I", reader.read(head + 4, 4))[0]
    pending = [(root, None, None)]
    seen: set[int] = set()
    keys: set[int] = set()
    while pending:
        node, lower, upper = pending.pop()
        if not node:
            continue
        row = reader.read(node, 24)
        if row[13]:
            continue  # std::map sentinel node, shared by empty children
        if node in seen:
            raise AssertionError("S21 first-manager map contains a node cycle")
        seen.add(node)
        left, _parent, right = struct.unpack_from("<III", row)
        key = struct.unpack_from("<i", row, 16)[0]
        if (lower is not None and key <= lower) or \
           (upper is not None and key >= upper) or key in keys:
            raise AssertionError("S21 first-manager map key/order drifted")
        keys.add(key)
        pending.append((left, lower, key))
        pending.append((right, key, upper))
    if len(keys) != count or min(keys) != 0 or max(keys) != 10759:
        raise AssertionError("S21 first-manager map traversal incomplete")
    candidate = CYLINDER_NATIVE_TYPE - MAP_KEY_BIAS
    if candidate in keys or any(key < 0 for key in keys):
        raise AssertionError("S21 cylinder unexpectedly enters first-manager key map")
    print(f"PASS: owner-pinned S21 first-manager map count={count}, key range=0..10759; "
          f"0x5D8-0xAE9={candidate} absent at this snapshot; main code anchors agree")
    print("OPEN: other map-independent special-render helpers and later runtime changes "
          "are not excluded by this snapshot")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
