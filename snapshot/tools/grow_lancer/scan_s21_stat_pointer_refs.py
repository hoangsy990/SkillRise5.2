"""Read-only candidate xrefs to actual S21 global stat pointer; not writer proof."""

import hashlib
import struct
import argparse

from disasm_s21_window import DUMP, IMAGE_BASE


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--near-speed", action="store_true")
    args = parser.parse_args()
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == (
        "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    )
    needle = struct.pack("<I", 0x1E04FBC)
    positions = []
    start = 0
    while True:
        offset = data.find(needle, start)
        if offset < 0:
            break
        positions.append(offset)
        start = offset + 1
    if args.near_speed:
        fields = (struct.pack("<I", 0x1B6), struct.pack("<I", 0x1C2))
        positions = [offset for offset in positions if any(
            token in data[offset + 4:offset + 200] for token in fields)]
    print("PASS hash-pinned global stat pointer byte-reference inventory count=", len(positions))
    for offset in positions:
        print(f"candidateVA=0x{offset + IMAGE_BASE:X} preceding={data[max(offset-8, 0):offset].hex()} following={data[offset+4:offset+12].hex()}")
    print("Scope: byte occurrences may be data/overlapping code; decode owner basic blocks before classification")


if __name__ == "__main__":
    main()
