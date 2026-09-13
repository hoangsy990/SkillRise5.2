from pathlib import Path
import sys

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x400000


def main() -> int:
    if len(sys.argv) not in (2, 3):
        print("usage: disasm_s21_window.py VA [SIZE]", file=sys.stderr)
        return 2

    va = int(sys.argv[1], 0)
    size = int(sys.argv[2], 0) if len(sys.argv) == 3 else 0x100
    start = va - IMAGE_BASE
    if start < 0:
        raise ValueError("VA precedes image base")

    data = DUMP.read_bytes()[start:start + size]
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.skipdata = False
    for instruction in decoder.disasm(data, va):
        print(
            f"0x{instruction.address:08X}: "
            f"{instruction.mnemonic:<8} {instruction.op_str}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
