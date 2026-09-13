"""Read-only focused disassembly leads for the hash-pinned Pegasus x86 client.

This intentionally reports instruction-shaped references only. It does not
claim function bounds or portable field semantics.
"""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile

EXPECTED_SHA256 = "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"


def main() -> int:
    path = pathlib.Path(sys.argv[1] if len(sys.argv) > 1 else r"D:\Pegasus\MU-x86.exe")
    skills = [int(value, 0) for value in sys.argv[2:]] or [241]
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise SystemExit(f"hash mismatch: {digest}")
    pe = pefile.PE(data=data, fast_load=True)
    image_base = pe.OPTIONAL_HEADER.ImageBase
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    md.skipdata = True
    hits: dict[int, list[tuple[int, str, str]]] = {skill: [] for skill in skills}
    for section in pe.sections:
        if not section.IMAGE_SCN_MEM_EXECUTE:
            continue
        code = section.get_data()
        address = image_base + section.VirtualAddress
        # Capstone can retain excessive native memory when one very large
        # section is passed to a single iterator. Scan bounded overlapping
        # windows and only accept instructions beginning in each core window.
        window = 1 << 20
        overlap = 16
        for offset in range(0, len(code), window):
            core_end = min(offset + window, len(code))
            part = code[offset:min(core_end + overlap, len(code))]
            for insn in md.disasm(part, address + offset):
                if insn.address >= address + core_end:
                    break
                if insn.id == 0:  # Capstone skipdata pseudo-instruction.
                    continue
                immediates = {op.imm for op in insn.operands
                              if op.type == capstone.x86.X86_OP_IMM}
                for skill in immediates.intersection(hits):
                    hits[skill].append((insn.address, insn.mnemonic, insn.op_str))
    for skill in skills:
        print(f"skill={skill} instruction_refs={len(hits[skill])}")
        for address, mnemonic, operands in hits[skill]:
            print(f"{address:08X}: {mnemonic:<8} {operands}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
