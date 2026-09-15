"""Pin forward live-slot traversal, not complete allocator/secondary routing."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE
data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
d = Cs(CS_ARCH_X86, CS_MODE_32)
rows = {}
for start,end in ((0x1595834,0x15958E9),(0x143E5F0,0x143E6B0)):
    code = list(d.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start))
    assert code[-1].address+code[-1].size == end
    rows.update({i.address:(i.mnemonic,i.op_str) for i in code})
for address,pair in {
    0x159589B:('and','dword ptr [ebp - 8], 0'),
    0x15958A4:('inc','eax'),
    0x15958AE:('jge','0x15958dc'),
    0x15958B0:('imul','eax, dword ptr [ebp - 8], 0x3d8'),
    0x15958C0:('mov','al, byte ptr [eax]'),
    0x15958CB:('je','0x15958da'),
    0x15958D3:('call','0x14b76c0'),
    0x15958DA:('jmp','0x15958a1'),
    0x143E627:('and','dword ptr [ebp - 0x3134], 0'),
    0x143E636:('inc','eax'),
    0x143E63D:('cmp','dword ptr [ebp - 0x3134], 0xc8'),
    0x143E66D:('call','0x10ef919'),
    0x143E681:('call','0x10efba1'),
    0x143E68F:('call','0x10efefb'),
    0x143E6AA:('jne','0x14b2e14'),
}.items(): assert rows[address] == pair,hex(address)
print('PASS source primary traversal reads Live at each forward slot, not a pre-snapshotted live list')
print('Conditional consequence: a new later slot can update this traversal; an earlier slot waits until the next traversal')
print('PASS allocation prefix has primary loop and conditional secondary routing; actual Magic child routing remains to resolve')
print('Do not batch every root tick before all child ticks or force every new child to update immediately')
