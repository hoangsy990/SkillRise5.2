#!/usr/bin/env python3
"""Read little-endian float constants from the pinned S21 memory image."""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("image", type=Path)
    parser.add_argument("addresses", nargs="+")
    parser.add_argument("--image-base", default="0x400000")
    parser.add_argument("--u32", action="store_true")
    parser.add_argument("--cstring", action="store_true")
    args = parser.parse_args()

    data = args.image.read_bytes()
    image_base = int(args.image_base, 0)
    for raw_address in args.addresses:
        address = int(raw_address, 0)
        offset = address - image_base
        if args.cstring:
            end = data.find(b"\0", offset)
            if end < 0:
                raise ValueError(f"unterminated string: {raw_address}")
            print(f"0x{address:08X} {data[offset:end].decode('cp949', errors='strict')}")
            continue
        raw = data[offset : offset + 4]
        if len(raw) != 4:
            raise ValueError(f"address outside image: {raw_address}")
        if args.u32:
            value = struct.unpack("<I", raw)[0]
            print(f"0x{address:08X} 0x{value:08X} {raw.hex().upper()}")
        else:
            value = struct.unpack("<f", raw)[0]
            print(f"0x{address:08X} {value:.9g} {raw.hex().upper()}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
