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
SPECIAL_MANAGER_GLOBAL = 0x1E057D8
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
    # A separate map-independent manager runs after the key-map branch.
    # Its second live virtual callback handles 0x5D8 directly. The mapped
    # image contains the code, while this full dump resolves the heap-owned
    # four-slot virtual dispatch table actually installed in this snapshot.
    managers = struct.unpack("<4I", reader.read(SPECIAL_MANAGER_GLOBAL, 16))
    if not all(managers):
        raise AssertionError("S21 special-render manager slot is empty")
    callbacks = tuple(struct.unpack("<I", reader.read(
        struct.unpack("<I", reader.read(manager, 4))[0] + 0x1C, 4))[0]
        for manager in managers)
    if callbacks != (0xAA8859, 0xAABC91, 0xAA33CA, 0xAA30CE):
        raise AssertionError(f"S21 virtual render callbacks drifted: {callbacks}")
    for va, expected in (
        (0xAA99BD, bytes.fromhex("68d7050000")),
        (0xAABCA8, bytes.fromhex("81785cd7050000")),
        (0x1B527B4, b"fire_arrow\0"),
        (0xAACD0C, bytes.fromhex("81785cd8050000")),
        (0xAACD82, bytes.fromhex("f30f1005d8530b07f30f5905c8e4b401")),
        (0xAACD98, bytes.fromhex("e89fd2ebff")),
        (0x96A040, bytes.fromhex("f30f5a45085151f20f110424e86f8c0b015959d9")),
        (0x15A925E, bytes.fromhex("f30f1005d8530b07f30f5905c8e4b401")),
        (0x15A9274, bytes.fromhex("e8c30d3cff")),
        (0xAACDA1, bytes.fromhex(
            "f30f1045d4f30f580548ddb401f30f5905dce6b401f30f580508dfb401")),
        (0xAACEA2, bytes.fromhex("6a426a008b4d0ce805beeb")),
    ):
        offset = va - 0x400000
        if image[offset:offset + len(expected)] != expected or \
                reader.read(va, len(expected)) != expected:
            raise AssertionError(f"S21 cylinder special draw code drifted at {va:#x}")
    factors = tuple(struct.unpack("<f", reader.read(va, 4))[0]
        for va in (0x1B4E4C8, 0x1B4DD48, 0x1B4E6DC, 0x1B4DF08))
    if any(abs(got - wanted) > 1e-6 for got, wanted in
           zip(factors, (0.005, 1.0, 0.25, 0.2))):
        raise AssertionError(f"S21 cylinder light wave drifted: {factors}")
    print(f"PASS: owner-pinned S21 first-manager map count={count}, key range=0..10759; "
          f"0x5D8-0xAE9={candidate} absent at this snapshot; main code anchors agree")
    print("PASS: live S21 special-manager callback 0xAABC91 handles 0x5D8; "
          "light *= (sin(WorldTime*.005)+1)*.25+.2; mesh0 flag=0x42 texture|bright")
    print("PASS: adjacent callback model 0x5D7 is loader-named fire_arrow, not a Slayer child")
    print("OPEN: later runtime map/manager changes and ingame frame parity "
          "are not excluded by this snapshot")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
