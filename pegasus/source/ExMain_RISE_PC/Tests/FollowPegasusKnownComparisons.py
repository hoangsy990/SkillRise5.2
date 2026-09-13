"""Read-only traversal of consecutive known-register model comparisons.

Stops on any unsupported instruction; never assumes external state/call results.
Usage: SCRIPT EXE START_VA REGISTER VALUE
"""
import hashlib
import pathlib
import sys
import capstone
import pefile

if len(sys.argv) != 5:
    raise SystemExit(__doc__)
data = pathlib.Path(sys.argv[1]).read_bytes()
if hashlib.sha256(data).hexdigest().upper() != (
        '02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA'):
    raise SystemExit('hash mismatch')
pe = pefile.PE(data=data, fast_load=True)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
pc = int(sys.argv[2], 0)
registers = {sys.argv[3]: int(sys.argv[4], 0)}
comparison = None

def value(ins, op):
    if op.type == capstone.x86.X86_OP_IMM:
        return op.imm
    if op.type == capstone.x86.X86_OP_REG:
        return registers.get(ins.reg_name(op.reg))
    return None

for _ in range(200):
    ins = next(md.disasm(pe.get_data(pc-pe.OPTIONAL_HEADER.ImageBase, 15), pc, 1))
    print(f'{pc:08X}: {ins.mnemonic} {ins.op_str}')
    next_pc = pc + ins.size
    ops = ins.operands
    if ins.mnemonic == 'mov' and ops[0].type == capstone.x86.X86_OP_REG:
        known = value(ins, ops[1])
        if known is None:
            break
        registers[ins.reg_name(ops[0].reg)] = known
    elif ins.mnemonic == 'cmp':
        left, right = (value(ins, op) for op in ops)
        if left is None or right is None:
            break
        comparison = (left, right)
    elif ins.mnemonic == 'jmp' and ops[0].type == capstone.x86.X86_OP_IMM:
        next_pc = ops[0].imm
    elif ins.mnemonic in ('je', 'jne', 'jl', 'jg', 'jle', 'jge') and comparison:
        left, right = comparison
        # Signed jumps only accepted for known nonnegative signed32 values.
        if not (0 <= left < 0x80000000 and 0 <= right < 0x80000000):
            break
        take = {'je': left == right, 'jne': left != right, 'jl': left < right,
                'jg': left > right, 'jle': left <= right, 'jge': left >= right}[ins.mnemonic]
        if ops[0].type != capstone.x86.X86_OP_IMM:
            break
        print(f'  known comparison -> {"taken" if take else "fallthrough"}')
        if take:
            next_pc = ops[0].imm
    else:
        break
    pc = next_pc
else:
    raise SystemExit('bounded traversal limit reached')
print(f'STOP {pc:08X}: unsupported or unknown state; registers={registers}')
