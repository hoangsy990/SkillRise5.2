"""Identify pinned Pegasus trig helpers through their x87 fallback chains.

This proves helper identity only, not any caller's vector/visual correctness.
"""
import hashlib
import pathlib
import sys

import capstone
import pefile

if len(sys.argv) != 2:
    raise SystemExit('usage: VerifyPegasusTrigHelpers.py PEGASUS_EXE')
data = pathlib.Path(sys.argv[1]).read_bytes()
expected = '02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA'
if hashlib.sha256(data).hexdigest().upper() != expected:
    raise SystemExit('FAIL: pinned Pegasus executable hash mismatch')
pe = pefile.PE(data=data, fast_load=True)
decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
decoder.detail = True

def instruction(address):
    code = pe.get_data(address - pe.OPTIONAL_HEADER.ImageBase, 15)
    return next(decoder.disasm(code, address, count=1))

for entry, fallback_call, fallback, x87, mnemonic in (
    (0xAF45F0, 0xAF47A4, 0xB0B47F, 0xB0B49D, 'fsin'),
    (0xAF4050, 0xAF41F7, 0xB0AE9F, 0xB0AEBD, 'fcos'),
):
    call = instruction(fallback_call)
    if (call.mnemonic != 'call' or
            call.operands[0].type != capstone.x86.X86_OP_IMM or
            call.operands[0].imm != fallback or
            instruction(x87).mnemonic != mnemonic):
        raise SystemExit(f'FAIL: helper identity chain drifted at {entry:08X}')
    print(f'PASS: {entry:08X} -> fallback {fallback:08X} -> {mnemonic} '
          f'at {x87:08X}; caller axis/sign checks remain separate')

# Spiral cadence uses a different helper: ROUNDSD immediate9 is round down
# (mode1, suppressed precision exception), not a trigonometric function.
round_down = instruction(0xAF480F)
if (round_down.mnemonic != 'roundsd' or
        round_down.operands[-1].type != capstone.x86.X86_OP_IMM or
        round_down.operands[-1].imm != 9):
    raise SystemExit('FAIL: AF4800 floor-helper instruction identity drifted')
print('PASS: AF4800 uses ROUNDSD immediate9 (floor); not SIN')
