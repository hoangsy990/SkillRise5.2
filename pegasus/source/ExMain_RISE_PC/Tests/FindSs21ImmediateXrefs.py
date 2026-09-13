"""Hash-guarded exact-immediate xrefs in executable SS21 PE sections."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile


EXPECTED_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"


def main() -> int:
    if len(sys.argv) < 3:
        raise SystemExit("usage: FindSs21ImmediateXrefs.py EXE VALUE [VALUE ...]")
    path = pathlib.Path(sys.argv[1])
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise SystemExit(f"hash mismatch: {digest}")
    values = {int(value, 0) & 0xFFFFFFFF for value in sys.argv[2:]}
    pe = pefile.PE(data=data, fast_load=True)
    image_base = pe.OPTIONAL_HEADER.ImageBase
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    decoder.skipdata = True
    found = 0
    for section in pe.sections:
        if not section.IMAGE_SCN_MEM_EXECUTE:
            continue
        code = section.get_data()
        section_start = image_base + section.VirtualAddress
        chunk_size = 0x100000
        for offset in range(0, len(code), chunk_size):
            chunk = code[offset:offset + chunk_size]
            start = section_start + offset
            for instruction in decoder.disasm(chunk, start):
                if instruction.id == 0:
                    continue
                immediates = {
                    operand.imm & 0xFFFFFFFF
                    for operand in instruction.operands
                    if operand.type == capstone.x86.X86_OP_IMM
                }
                matched = sorted(values & immediates)
                if matched:
                    labels = ",".join(f"0x{value:X}" for value in matched)
                    print(f"{instruction.address:08X} [{labels}]: "
                          f"{instruction.mnemonic:<8} {instruction.op_str}")
                    found += 1
    print(f"matches={found}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
