"""Hash-guarded x86 callsite contexts filtered by exact immediates."""

from __future__ import annotations

import hashlib
import pathlib
import sys

import capstone
import pefile


EXPECTED_SHA256 = "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"


def main() -> int:
    if len(sys.argv) < 4:
        raise SystemExit(
            "usage: FindPegasusCallContexts.py EXE CALL_TARGET CONTEXT_COUNT [IMMEDIATE ...]"
        )
    path = pathlib.Path(sys.argv[1])
    target = int(sys.argv[2], 0) & 0xFFFFFFFF
    context_count = int(sys.argv[3], 0)
    required = {int(value, 0) & 0xFFFFFFFF for value in sys.argv[4:]}
    if context_count <= 0 or context_count > 64:
        raise SystemExit("CONTEXT_COUNT must be in 1..64")

    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise SystemExit(f"hash mismatch: {digest}")

    pe = pefile.PE(data=data, fast_load=True)
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    decoder.detail = True
    decoder.skipdata = True
    matches = 0

    for section in pe.sections:
        if not section.IMAGE_SCN_MEM_EXECUTE:
            continue
        start = pe.OPTIONAL_HEADER.ImageBase + section.VirtualAddress
        instructions = list(decoder.disasm(section.get_data(), start))
        for index, instruction in enumerate(instructions):
            if instruction.id != capstone.x86.X86_INS_CALL or not instruction.operands:
                continue
            operand = instruction.operands[0]
            if operand.type != capstone.x86.X86_OP_IMM or (operand.imm & 0xFFFFFFFF) != target:
                continue
            begin = max(0, index - context_count)
            context = instructions[begin : index + 1]
            immediates = {
                op.imm & 0xFFFFFFFF
                for item in context
                if item.id != 0
                for op in item.operands
                if op.type == capstone.x86.X86_OP_IMM
            }
            if required and not (required & immediates):
                continue
            print(f"CALLSITE {instruction.address:08X}")
            for item in context:
                print(f"  {item.address:08X}: {item.mnemonic:<8} {item.op_str}")
            matches += 1
    print(f"matches={matches}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
