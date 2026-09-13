"""Hash-guarded bounded x86 disassembly of the read-only Pegasus client."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile

EXPECTED_SHA256 = "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"


def main() -> int:
    if len(sys.argv) != 4:
        raise SystemExit("usage: DisassemblePegasusRange.py EXE START_VA END_VA")
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
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.skipdata = True
    covered = 0
    for insn in md.disasm(code, start):
        covered += insn.size
        print(f"{insn.address:08X}: {insn.mnemonic:<8} {insn.op_str}")
    print(f"covered={covered}/{len(code)}")
    return 0 if covered == len(code) else 2


if __name__ == "__main__":
    raise SystemExit(main())
