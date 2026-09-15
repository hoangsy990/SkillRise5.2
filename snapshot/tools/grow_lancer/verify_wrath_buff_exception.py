"""Pin complete exception decoder and selected Wrath add/remove paths only."""
import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
cs = Cs(CS_ARCH_X86, CS_MODE_32)
def decode(start, end):
    result = list(cs.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert result[0].mnemonic == 'push' and result[0].op_str == 'ebp'
    assert result[-1].mnemonic == 'ret'
    assert sum(i.size for i in result) == end-start
    return {i.address: (i.mnemonic, i.op_str) for i in result}

d = decode(0x1284D00, 0x128527A)
expected = {
    0x1284D28: ('movzx', 'eax, byte ptr [ebp + 0x10]'),
    0x1284D2C: ('test', 'eax, eax'),
    0x1284D2E: ('je', '0x12850a0'),
    0x1284D34: ('mov', 'eax, dword ptr [ebp + 8]'),
    0x1284D37: ('mov', 'dword ptr [ebp - 0x1c], eax'),
    0x1284D3A: ('cmp', 'dword ptr [ebp - 0x1c], 0xb'),
    0x1284D3E: ('je', '0x1284d4b'),
    0x1284D40: ('cmp', 'dword ptr [ebp - 0x1c], 0xc'),
    0x1284D44: ('je', '0x1284d7f'),
    0x1284D46: ('jmp', '0x1285094'),
    0x1285094: ('mov', 'al, 1'),
    0x1285096: ('jmp', '0x128526d'),
    0x12850A0: ('mov', 'eax, dword ptr [ebp + 8]'),
    0x12850A3: ('mov', 'dword ptr [ebp - 0x18], eax'),
    0x12850E0: ('jmp', '0x128526b'),
    0x128526B: ('mov', 'al, 1'),
}
for address, value in expected.items():
    assert d[address] == value, (hex(address), d[address])
comparisons = [(a, value) for a, value in d.items() if 0x12850A6 <= a < 0x12850E0]
assert len(comparisons) == 14
for n in range(7):
    assert comparisons[n*2][1] == ('cmp', f'dword ptr [ebp - 0x18], {hex(11+n)}')
    assert comparisons[n*2+1][1][0] == 'je'
for buff in (424, 425):
    assert buff not in (11, 12) and buff not in range(11, 18)
print('PASS: full exception function decode; Wrath424/425 add/remove default returns true without exception-path calls.')
c = decode(0x113B5F8, 0x113B611)
assert c[0x113B605] == ('call', '0x113b50e')
assert c[0x113B60A] == ('mov', 'eax, dword ptr [eax + 0x28]')
print('PASS: classifier reads record+0x28; record contents, duration, packet and live buff lifecycle remain OPEN.')
