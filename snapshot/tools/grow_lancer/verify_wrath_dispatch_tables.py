"""Check the two dump table entries used by Wrath aura; not a chain verifier."""
from pathlib import Path
import struct

DUMP = Path(r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe")
BASE = 0x400000


def main():
    data = DUMP.read_bytes()
    slot = data[0x1594BE0 - BASE + (0x5EC - 0x4D3)]
    first = struct.unpack_from("<I", data, 0x1594A6C - BASE + slot * 4)[0]
    second = struct.unpack_from("<I", data, 0x1595660 - BASE + (0x5EC - 0x5E1) * 4)[0]
    if (slot, first, second) != (0x5C, 0x1574472, 0x157939E):
        raise RuntimeError(f"Unexpected tables: {slot:#x}, {first:#x}, {second:#x}")
    print(f"PASS: 0x5EC first table slot {slot:#x} -> {first:#x}; secondary -> {second:#x}")
    print("Scope: table values only; does not prove intervening control flow, stack initialization or visual parity.")


if __name__ == "__main__":
    main()
