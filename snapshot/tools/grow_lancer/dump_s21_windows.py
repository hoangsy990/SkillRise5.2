from pathlib import Path
import sys

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x400000


def main() -> int:
    if len(sys.argv) < 3 or len(sys.argv) % 2 == 0:
        print("usage: dump_s21_windows.py START END [START END ...]", file=sys.stderr)
        return 2
    data = DUMP.read_bytes()
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.skipdata = False
    for n in range(1, len(sys.argv), 2):
        start = int(sys.argv[n], 0)
        end = int(sys.argv[n + 1], 0)
        if start < IMAGE_BASE or end <= start or end - IMAGE_BASE > len(data):
            raise ValueError(f"invalid mapped window {start:#x}..{end:#x}")
        print(f"## {start:#x}..{end:#x}")
        code = data[start - IMAGE_BASE : end - IMAGE_BASE]
        for instruction in decoder.disasm(code, start):
            print(
                f"{instruction.address:08X}  "
                f"{instruction.mnemonic:<8} {instruction.op_str}"
            )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
