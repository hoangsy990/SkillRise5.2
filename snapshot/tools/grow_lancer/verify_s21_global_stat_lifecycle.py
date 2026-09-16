"""Pin S21 global stat allocation/member ownership and speed-word initialization."""
import hashlib

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == (
    "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
)
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
pins = {i.address: (i.mnemonic, i.op_str) for start, end in (
    (0x127879C, 0x1278805), (0xA07788, 0xA077BD),
    (0xA09F4B, 0xA09F69), (0xA0A0E9, 0xA0A13D),
    (0x1296442, 0x1296474),
) for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
for address, expected in {
    0x12787A2: ("push", "0x2df0"),
    0x12787BC: ("mov", "ecx, dword ptr [ebp - 0x20d8]"),
    0x12787C2: ("call", "0xa07788"),
    0x12787E8: ("mov", "dword ptr [0x1e04fb8], eax"),
    0x12787F2: ("add", "eax, 8"),
    0x12787F5: ("mov", "dword ptr [0x1e04fbc], eax"),
    0xA077B1: ("add", "ecx, 8"),
    0xA077B4: ("call", "0xa09f4b"),
    0xA09F4F: ("mov", "dword ptr [ebp - 4], ecx"),
    0xA0A0EE: ("mov", "word ptr [ecx + 0x1b6], ax"),
    0xA0A136: ("mov", "word ptr [ecx + 0x1c2], ax"),
    0x129644E: ("mov", "eax, dword ptr [0x1e04fbc]"),
    0x129645A: ("mov", "word ptr [eax + 0x1b6], cx"),
    0x129646D: ("mov", "word ptr [eax + 0x1c2], cx"),
}.items():
    assert pins[address] == expected, (hex(address), pins.get(address))
assert pins[0xA0A0E9] == ("xor", "eax, eax")
assert pins[0xA0A131] == ("xor", "eax, eax")

print("PASS source outer allocation 0x2DF0 -> A07788 -> member+8 A09F4B")
print("PASS global pointers outer1E04FB8/member1E04FBC; ctor zeros speed WORDs")
print("PASS receiver1296442 writes same member WORDs from incoming payload")
print("OPEN subsequent global writes, S21 GS sender/rate and native class7 wire mapping")
