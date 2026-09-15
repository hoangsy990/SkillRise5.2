"""Verify selected S21 membership semantics, not buff duration or network delivery."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
cs = Cs(CS_ARCH_X86, CS_MODE_32)
def function(start, end):
    items = list(cs.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in items) == end-start
    assert items[0].mnemonic == 'push' and items[-1].mnemonic == 'ret'
    return {i.address: (i.mnemonic, i.op_str) for i in items}

add = function(0x130C63E, 0x130C6CD)
remove = function(0x130C769, 0x130C7ED)
equal = function(0x130E115, 0x130E13E)
unequal = function(0x130E16D, 0x130E19C)
def pins(decoded, expected):
    for address, value in expected.items():
        assert decoded[address] == value, (hex(address), decoded[address])

pins(equal, {0x130E125: ('cmp', 'eax, dword ptr [ecx]'),
    0x130E127: ('jne', '0x130e132'), 0x130E129: ('mov', 'dword ptr [ebp - 4], 1'),
    0x130E132: ('and', 'dword ptr [ebp - 4], 0')})
pins(unequal, {0x130E17B: ('call', '0x130e115'),
    0x130E185: ('jne', '0x130e190'), 0x130E187: ('mov', 'dword ptr [ebp - 4], 1'),
    0x130E190: ('and', 'dword ptr [ebp - 4], 0')})
pins(add, {
    0x130C655: ('call', '0x130f0cb'), 0x130C665: ('call', '0x130f052'),
    0x130C66E: ('call', '0x130e115'), 0x130C678: ('je', '0x130c6c9'),
    0x130C67A: ('push', '0xc'), 0x130C67C: ('call', '0x19b8ffc'),
    0x130C68E: ('mov', 'dword ptr [eax], 1'),
    0x130C697: ('mov', 'ecx, dword ptr [ebp + 0xc]'),
    0x130C69A: ('mov', 'dword ptr [eax + 4], ecx'),
    0x130C6A0: ('or', 'dword ptr [eax + 8], 0xffffffff'),
    0x130C6C3: ('call', '0x130dcd5'), 0x130C6C9: ('leave', ''),
})
pins(remove, {
    0x130C775: ('call', '0x130c887'), 0x130C781: ('jmp', '0x130c7e9'),
    0x130C791: ('call', '0x130f0cb'), 0x130C7A1: ('call', '0x130f052'),
    0x130C7AA: ('call', '0x130e16d'), 0x130C7B4: ('je', '0x130c7e9'),
    0x130C7BE: ('mov', 'eax, dword ptr [eax + 4]'),
    0x130C7CA: ('push', '0xc'), 0x130C7CF: ('call', '0x19b902c'),
    0x130C7E3: ('call', '0x130dc61'), 0x130C7E9: ('leave', ''),
})
print('PASS: complete add/remove/equality decoder coverage and selected branch pins.')
print('Existing membership skips allocation and writes; absent membership initializes 12-byte payload (1, argument, -1).')
print('Removal releases payload then erases entry; missing entry takes no-release exit. No duration refresh is proven here.')
activation = function(0x12FD717, 0x12FD882)
pins(activation, {
    0x12FD7DE: ('push', 'dword ptr [ebp + 0x14]'),
    0x12FD7E1: ('push', 'dword ptr [ebp + 8]'),
    0x12FD7ED: ('call', '0x130c63e'),
    0x12FD864: ('push', 'dword ptr [ebp + 0x10]'),
    0x12FD86D: ('call', '0x12892d3'),
})
print('PASS: membership payload receives activation argument4, distinct from argument3 passed to logical effects. Units remain unverified.')
value_get = function(0x130CAF9, 0x130CB64)
value_set = function(0x130CB64, 0x130CBBB)
state_set = function(0x130CBBB, 0x130CC12)
state_get = function(0x130CC12, 0x130CC7F)
pins(value_get, {0x130CB46: ('je', '0x130cb5e'),
    0x130CB59: ('mov', 'eax, dword ptr [eax + 4]'),
    0x130CB5E: ('xor', 'eax, eax')})
pins(value_set, {0x130CB9E: ('je', '0x130cbb7'),
    0x130CBB1: ('mov', 'ecx, dword ptr [ebp + 0xc]'),
    0x130CBB4: ('mov', 'dword ptr [eax + 4], ecx')})
pins(state_set, {0x130CBF5: ('je', '0x130cc0e'),
    0x130CC08: ('mov', 'ecx, dword ptr [ebp + 0xc]'),
    0x130CC0B: ('mov', 'dword ptr [eax + 8], ecx')})
pins(state_get, {0x130CC73: ('mov', 'eax, dword ptr [eax + 8]'),
    0x130CC78: ('or', 'eax, 0xffffffff')})
reply = function(0x1293A5C, 0x1293B12)
pins(reply, {
    0x1293A6B: ('movzx', 'eax, byte ptr [eax + 5]'),
    0x1293A7A: ('movzx', 'ecx, byte ptr [ecx + 4]'),
    0x1293A87: ('shl', 'ecx, 8'),
    0x1293AC1: ('movzx', 'eax, word ptr [eax + 6]'),
    0x1293ACF: ('call', '0x130c8a5'),
    0x1293AD9: ('je', '0x1293b10'),
    0x1293ADE: ('push', 'dword ptr [eax + 8]'),
    0x1293AF2: ('call', '0x130cb64'),
    0x1293AF7: ('push', '1'),
    0x1293B0A: ('call', '0x130cbbb'),
})
consumer = function(0xA0B4F4, 0xA0B614)
pins(consumer, {
    0xA0B583: ('call', '0x130cc12'), 0xA0B588: ('cmp', 'eax, -1'),
    0xA0B598: ('call', '0xa0bf35'), 0xA0B59F: ('push', '0'),
    0xA0B5AD: ('call', '0x130cbbb'), 0xA0B5C2: ('call', '0x130cc12'),
    0xA0B5C7: ('test', 'eax, eax'), 0xA0B5C9: ('jne', '0xa0b5cf'),
    0xA0B5DB: ('call', '0x130caf9'), 0xA0B5F8: ('call', '0xa0acb8'),
    0xA0B603: ('cmp', 'eax, dword ptr [ebp - 0x1c]'),
    0xA0B606: ('jle', '0xa0b60d'),
})
print('PASS: payload accessors, existing-membership reply writes value/+8=1, and consumer -1->request->0 wait gate.')
print('Payload+8 behaves as request state, not a ticking lifetime here; comparison value units and Wrath caller relevance remain OPEN.')
selector = function(0xA0B614, 0xA0B6A2)
initialize = function(0xA0ABA4, 0xA0ACB8)
clear = function(0xA0C186, 0xA0C1D3)
pins(clear, {0xA0C1BB: ('mov', 'dword ptr [eax], ecx'),
    0xA0C1C7: ('mov', 'dword ptr [eax + 4], ecx'),
    0xA0C1CD: ('and', 'dword ptr [eax + 4], 0')})
pins(initialize, {0xA0ABB0: ('add', 'ecx, 0x18'),
    0xA0ABB3: ('call', '0xa0c186')})
insert_values = [int(operands.rsplit(', ',1)[1],0)
    for mnemonic,operands in initialize.values()
    if mnemonic == 'mov' and operands.startswith('dword ptr [ebp - ')
    and operands.rsplit(', ',1)[1] != 'ecx']
assert insert_values == [1,400,346,2,401,347,142,402,348,427]
pins(selector, {0xA0B698: ('mov', 'eax, dword ptr [ebp - 8]'),
    0xA0B69B: ('add', 'eax, 0x18')})
dispatch_values = {int(op.rsplit(', ',1)[1],0) for mnemonic,op in selector.values()
    if mnemonic == 'cmp'}
assert dispatch_values == set(insert_values)
assert not {424,425}.intersection(dispatch_values)
print('PASS: initialized selector groups contain1/400/346,2/401/347,142/402/348/427; default group is cleared and unpopulated here.')
print('Wrath424/425 is absent from these initialized groups. This comparison/request branch is not a demonstrated Wrath prerequisite.')
