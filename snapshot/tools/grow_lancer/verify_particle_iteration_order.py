"""Pinned active-list iteration/append path; simulation is not runtime QA."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
rows = {}
for start, end in (
    (0x1725658, 0x172567F), (0x172574D, 0x1725772),
    (0x1724EBE, 0x1724F16), (0x1724F63, 0x1724FA2),
    (0x16AA5EE, 0x16AA670), (0x16AA694, 0x16AA6CE),
    (0x172343C, 0x17234BD), (0x16A9524, 0x16A9561),
    (0x1725950, 0x172596C), (0x1724501, 0x172457A),
    (0x1725571, 0x17255D1), (0x1724444, 0x1724470),
):
    decoded = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in decoded) == end-start, hex(start)
    rows.update({i.address: (i.mnemonic, i.op_str) for i in decoded})
for address, expected in {
    0x172566E: ('push', 'dword ptr [eax]'),
    0x1725761: ('push', 'dword ptr [eax]'),
    0x1724ECE: ('cmp', 'eax, dword ptr [ecx]'),
    0x1724F72: ('mov', 'eax, dword ptr [eax]'),
    0x1724F74: ('mov', 'dword ptr [ecx], eax'),
    0x16AA615: ('call', '0x172574d'),
    0x16AA657: ('cmp', 'dword ptr [eax + 0x48], 0'),
    0x16AA6A3: ('call', '0x1724f7b'),
    0x16AA6C3: ('call', '0x172579f'),
    0x1723447: ('add', 'eax, 0x48'),
    0x1723458: ('dec', 'eax'),
    0x17234A2: ('call', '0x1724f7b'),
    0x17234B8: ('jmp', '0x16aa608'),
    0x16A9531: ('call', '0x1725950'),
    0x172595D: ('push', 'dword ptr [eax]'),
    0x1725962: ('call', '0x1724501'),
    0x1724563: ('call', '0x1725571'),
    0x172557D: ('mov', 'eax, dword ptr [eax + 4]'),
    0x17255BF: ('mov', 'dword ptr [eax + 4], ecx'),
    0x17255C8: ('mov', 'dword ptr [eax], ecx'),
    0x172446C: ('mov', 'dword ptr [ecx], eax'),
}.items():
    assert rows[address] == expected, (hex(address), rows[address])

# Conditional linked-list fixture: parent appends before advancing its iterator.
# No claim that every Magic particle subtype emits a child in its update.
sentinel = {'name': 'end'}
parent = {'name': 'parent', 'next': sentinel}
sentinel['next'] = parent
sentinel['prev'] = parent
visited = []
cursor = sentinel['next']
while cursor is not sentinel:
    visited.append(cursor['name'])
    if cursor is parent:
        child = {'name': 'child', 'next': sentinel}
        sentinel['prev']['next'] = child
        sentinel['prev'] = child
    cursor = cursor['next']
assert visited == ['parent', 'child']
print('PASS pinned particle active-list sentinel, append links and post-update increment')
print('PASS conditional append-during-update fixture visits newborn in the same traversal')
print('OPEN allocation failure/refcount, concrete child emitters and native private scheduling/runtime')
