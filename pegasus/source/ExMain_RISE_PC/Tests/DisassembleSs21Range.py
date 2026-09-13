"""Hash-guarded bounded x86 disassembly of the read-only SS21 client dump."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile


EXPECTED_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"


def main() -> int:
    if len(sys.argv) != 4:
        raise SystemExit("usage: DisassembleSs21Range.py EXE START_VA END_VA")
    path = pathlib.Path(sys.argv[1])
    start = int(sys.argv[2], 0)
    end = int(sys.argv[3], 0)
    if end <= start or end - start > 0x10000:
        raise SystemExit("range must be positive and at most 64 KiB")
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise SystemExit(f"hash mismatch: {digest}")
    pe = pefile.PE(data=data, fast_load=True)
    rva = start - pe.OPTIONAL_HEADER.ImageBase
    code = pe.get_data(rva, end - start)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.skipdata = False
    covered = 0
    for instruction in decoder.disasm(code, start):
        covered += instruction.size
        print(f"{instruction.address:08X}: {instruction.mnemonic:<8} {instruction.op_str}")
    print(f"covered={covered}/{len(code)}")
    return 0 if covered == len(code) else 2


if __name__ == "__main__":
    raise SystemExit(main())
