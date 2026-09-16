"""Do not infer the S21 character join from the native 5.2 F3:03 number."""
import hashlib
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == (
    "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
)
table = 0x130505C - IMAGE_BASE
assert struct.unpack_from("<I", data, table + 0x03*4)[0] == 0x12FEA31
assert struct.unpack_from("<I", data, table + 0xF3*4)[0] == 0x12FF92C
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
pins = {i.address: (i.mnemonic, i.op_str) for start, end in (
    (0x12FEA31, 0x12FEA3A), (0x12FF92C, 0x12FF974),
    (0x12FF9F7, 0x12FFA04), (0x12FFB21, 0x12FFB31),
    (0x986942, 0x9869B9), (0x1297213, 0x129724A),
    (0xD54C5B2, 0xD54C5B7), (0xC02D500, 0xC02D505),
    (0xE12B1DA, 0xE12B1E1),
) for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
for address, expected in {
    0x12FEA34: ("call", "0x1297213"),
    0x12FF94F: ("movzx", "eax, byte ptr [eax + 3]"),
    0x12FF96A: ("movzx", "eax, byte ptr [eax + 4]"),
    0x12FF9F7: ("cmp", "dword ptr [ebp - 0x298], 3"),
    0x12FF9FE: ("je", "0x12ffb21"),
    0x12FFB24: ("call", "0x12884db"),
    0x12FFB2B: ("call", "0x986942"),
    0x98698A: ("call", "dword ptr [eax + 0x44]"),
    0x9869A2: ("mov", "dword ptr [ebp - 0x2c], eax"),
    0x9869B2: ("jmp", "dword ptr [eax*4 + 0x986bca]"),
    0x1297245: ("jmp", "0xd54c5b2"),
    0xD54C5B2: ("jmp", "0xc02d500"),
    0xC02D500: ("jmp", "0xe12b1da"),
    0xE12B1DA: ("push", "0"),
    0xE12B1DC: ("jmp", "0xe0cdae2"),
}.items():
    assert pins[address] == expected, (hex(address), pins.get(address))

# Use the isolated source tree, not the dump's parent path (the untouched
# Source project).
root = Path(__file__).resolve().parents[2]
ws = (root / "ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp").read_text(
    encoding="latin-1"
)
assert "ReceiveJoinMapServer(ReceiveBuffer, bEncrypted)" in ws
assert "CharacterAttribute->AttackSpeed" in ws

print("PASS S21 head03 and headF3 dispatch to different handlers")
print("PASS F3 subtype03 routes packet wrapper12884DB -> indirect/status switch986942")
print("PASS plain head03 begins multi-hop virtualized branch1297245->D54C5B2->C02D500->E12B1DA")
print("OPEN complete protected continuation and initial stat join semantics")
print("OPEN native 5.2 C1:F3:03 equivalence; numeric opcode alone is not mapping")
