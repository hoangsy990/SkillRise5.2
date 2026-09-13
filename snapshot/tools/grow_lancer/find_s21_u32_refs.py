from pathlib import Path
import struct
import sys


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x400000


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: find_s21_u32_refs.py VALUE", file=sys.stderr)
        return 2
    value = int(sys.argv[1], 0)
    needle = struct.pack("<I", value)
    data = DUMP.read_bytes()
    offset = 0
    found = 0
    while True:
        offset = data.find(needle, offset)
        if offset < 0:
            break
        prefix = data[offset - 1] if offset else None
        suffix = " push-imm32" if prefix == 0x68 else ""
        print(f"0x{IMAGE_BASE + offset:08X}{suffix}")
        found += 1
        offset += 1
    print(f"count={found}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
