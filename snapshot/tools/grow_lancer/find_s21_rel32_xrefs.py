from pathlib import Path
import struct
import sys


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x400000


def main() -> int:
    if len(sys.argv) < 2:
        print("usage: find_s21_rel32_xrefs.py TARGET [TARGET ...]", file=sys.stderr)
        return 2
    data = DUMP.read_bytes()
    targets = {int(value, 0) for value in sys.argv[1:]}
    matches = {target: [] for target in targets}
    for offset in range(len(data) - 5):
        opcode = data[offset]
        if opcode not in (0xE8, 0xE9):
            continue
        destination = IMAGE_BASE + offset + 5 + struct.unpack_from(
            "<i", data, offset + 1
        )[0]
        if destination in targets:
            matches[destination].append((IMAGE_BASE + offset, opcode))
    for target in sorted(targets):
        print(f"{target:#x}")
        for address, opcode in matches[target]:
            print(f"  {address:#x} {'call' if opcode == 0xE8 else 'jmp'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
