"""Locate ASCII strings and instruction-shaped address references in Pegasus."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile

EXPECTED_SHA256 = "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"


def main() -> int:
    if len(sys.argv) < 3:
        raise SystemExit("usage: FindPegasusStringXrefs.py EXE ASCII [ASCII ...]")
    path = pathlib.Path(sys.argv[1])
    needles = [value.encode("ascii") for value in sys.argv[2:]]
    data = path.read_bytes()
    if hashlib.sha256(data).hexdigest().upper() != EXPECTED_SHA256:
        raise SystemExit("hash mismatch")
    pe = pefile.PE(data=data, fast_load=True)
    base = pe.OPTIONAL_HEADER.ImageBase
    targets: dict[int, str] = {}
    for needle in needles:
        cursor = 0
        while True:
            offset = data.find(needle, cursor)
            if offset < 0:
                break
            start = offset
            while start > 0 and 0x20 <= data[start - 1] < 0x7f:
                start -= 1
            stop = offset + len(needle)
            while stop < len(data) and 0x20 <= data[stop] < 0x7f:
                stop += 1
            full = data[start:stop].decode("ascii")
            va = base + pe.get_rva_from_offset(start)
            targets[va] = full
            print(f"string {full!r} file={start:#x} va={va:#x}")
            cursor = offset + 1
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    for section in pe.sections:
        if not section.IMAGE_SCN_MEM_EXECUTE:
            continue
        for insn in md.disasm(section.get_data(), base + section.VirtualAddress):
            if insn.id == 0:
                continue
            immediates = {op.imm for op in insn.operands
                          if op.type == capstone.x86.X86_OP_IMM}
            for target in immediates.intersection(targets):
                print(f"xref {targets[target]!r} {insn.address:08X}: "
                      f"{insn.mnemonic} {insn.op_str}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
