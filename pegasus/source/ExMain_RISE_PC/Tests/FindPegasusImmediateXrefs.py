"""Hash-guarded exact-immediate xrefs in executable PE sections."""
from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile

EXPECTED_SHA256 = "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"


def main() -> int:
    if len(sys.argv) < 3:
        raise SystemExit("usage: FindPegasusImmediateXrefs.py EXE [--absolute-memory] VALUE [VALUE ...]")
    path = pathlib.Path(sys.argv[1])
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise SystemExit(f"hash mismatch: {digest}")
    arguments = sys.argv[2:]
    absolute_memory = "--absolute-memory" in arguments
    values = {int(value, 0) & 0xFFFFFFFF for value in arguments
              if value != "--absolute-memory"}
    if not values:
        raise SystemExit("at least one target value is required")
    pe = pefile.PE(data=data, fast_load=True)
    image_base = pe.OPTIONAL_HEADER.ImageBase
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    # Whole executable sections contain alignment/data islands; skip them while
    # retaining detail for decoded instructions.
    decoder.skipdata = True
    found = 0
    for section in pe.sections:
        if not section.IMAGE_SCN_MEM_EXECUTE:
            continue
        code = section.get_data()
        start = image_base + section.VirtualAddress
        for instruction in decoder.disasm(code, start):
            if instruction.id == 0:
                continue
            immediates = {
                operand.imm & 0xFFFFFFFF
                for operand in instruction.operands
                if operand.type == capstone.x86.X86_OP_IMM
            }
            matched = sorted(values & immediates)
            # Optional absolute globals only: never treat a structure offset
            # with a base/index register as a reference to a global address.
            memory_matches = sorted(values & {
                operand.mem.disp & 0xFFFFFFFF
                for operand in instruction.operands
                if absolute_memory and operand.type == capstone.x86.X86_OP_MEM
                and operand.mem.base == 0 and operand.mem.index == 0
            })
            if matched or memory_matches:
                labels = ",".join(f"0x{value:X}" for value in matched)
                if memory_matches:
                    labels += ("," if labels else "") + ",".join(
                        f"mem:0x{value:X}" for value in memory_matches)
                print(f"{instruction.address:08X} [{labels}]: "
                      f"{instruction.mnemonic:<8} {instruction.op_str}")
                found += 1
    print(f"matches={found}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
