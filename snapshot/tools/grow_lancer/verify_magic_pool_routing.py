"""Resolve both S21 allocation selectors for Magic model records only."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE
data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
d = Cs(CS_ARCH_X86, CS_MODE_32)
def decode(start,end):
    instructions = list(d.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start))
    assert sum(i.size for i in instructions)==end-start
    return {i.address:i for i in instructions}
first = decode(0x10EF919,0x10EFB4D)
second = decode(0x10EFBA1,0x10EFDFC)
for address,pair in {
    0x10EFBBA:('jne','0x10efbc3'),
    0x10EFBBC:('xor','eax, eax'),
    0x10EFBBE:('jmp','0x10efdf8'),
}.items():
    i=second[address]; assert (i.mnemonic,i.op_str)==pair
def reaches_zero(rows, start, zero, effect):
    pc=start; comparison=None
    for _ in range(60):
        i=rows[pc]
        if pc==zero:
            assert (i.mnemonic,i.op_str)==('xor','eax, eax')
            assert (rows[pc+2].mnemonic,rows[pc+2].op_str)==('leave','')
            assert (rows[pc+3].mnemonic,rows[pc+3].op_str)==('ret','8')
            return
        if i.mnemonic=='cmp':
            assert i.op_str.startswith('dword ptr [ebp - 4], ')
            comparison=effect-int(i.op_str.split(', ')[1],0)
        elif i.mnemonic in ('je','jg','jmp'):
            assert comparison is not None
            if i.mnemonic=='jmp' or (i.mnemonic=='je' and comparison==0) or (i.mnemonic=='jg' and comparison>0):
                pc=int(i.op_str,0); continue
        else: raise AssertionError((hex(pc),i.mnemonic,i.op_str))
        pc+=i.size
    raise AssertionError('Non-terminating selected path')
for effect in range(0x5E5,0x5EA):
    reaches_zero(first,0x10EF927,0x10EFB47,effect)
    reaches_zero(second,0x10EFBC9,0x10EFDF6,effect)
    print(f'PASS type={effect:#x}: both selectors zero; second also zero when map predicate false')
allocation=decode(0x143E5F0,0x143E6B0)
for address,pair in {
    0x143E674:('jne','0x143e68a'),
    0x143E688:('je','0x143e69a'),
    0x143E68F:('call','0x10efefb'),
}.items():
    i=allocation[address]; assert (i.mnemonic,i.op_str)==pair
print('PASS Magic model records5E5..5E9 keep primary allocation pointer, all subtypes on these reached paths')
print('OPEN bitmap particles/joints/sprites have separate allocation/update contracts; not whole-skill pool closure')
