"""Pin same-offset outbound stat serialization, not global stat ownership."""
import hashlib
import struct

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == (
    "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
)
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
pins = {i.address: (i.mnemonic, i.op_str) for start, end in (
    (0x95C0C1, 0x95C145), (0x95CB79, 0x95CC00),
    (0x9572D5, 0x95734D),
) for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
for address, expected in {
    0x9572DB: ("lea", "eax, [ebp - 0x210]"),
    0x9572E8: ("call", "0x95c0c1"),
    0x957303: ("push", "0xae"),
    0x957308: ("push", "0xc2"),
    0x95732C: ("lea", "eax, [ebp - 0x210]"),
    0x95C0E9: ("mov", "dword ptr [ebp - 0x48], ecx"),
    0x95C126: ("mov", "eax, dword ptr [ebp + 8]"),
    0x95C129: ("mov", "dword ptr [ebp - 0x44], eax"),
    0x95CB93: ("mov", "cx, word ptr [ecx + 0x15e]"),
    0x95CB9A: ("mov", "word ptr [eax + 0x1b6], cx"),
    0x95CBD6: ("mov", "ax, word ptr [edx + eax + 0x1ec]"),
    0x95CBDE: ("mov", "word ptr [ecx + 0x1c2], ax"),
}.items():
    assert pins[address] == expected, (hex(address), pins.get(address))

target = 0x95C0C1
direct_calls = []
start = 0
while True:
    start = data.find(b"\xe8", start)
    if start < 0 or start + 5 > len(data):
        break
    rel = struct.unpack_from("<i", data, start + 1)[0]
    va = IMAGE_BASE + start
    if va + 5 + rel == target:
        direct_calls.append(va)
    start += 1

print("PASS 95C0C1 self(ECX) +15E/+1EC WORD copy into caller TEMP +1B6/+1C2")
print("PASS caller9572A6 passes stack temp and builds C2:AE, not global stat1E04FBC")
print("DIRECT_CALL_LEADS", [hex(x) for x in direct_calls])
print("OPEN self-field producer, outbound packet send semantics and server wire rounding")
print("OPEN separate global stat initialization: offset matching alone is not identity")
