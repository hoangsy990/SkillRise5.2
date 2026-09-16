"""Pin S21 Clash terrain/send/movement byte order to a dormant 5.2 payload."""

import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
decoder = Cs(CS_ARCH_X86, CS_MODE_32)


def decode(start: int, end: int) -> dict[int, tuple[str, str]]:
    rows = list(decoder.disasm(data[start - IMAGE_BASE:end - IMAGE_BASE], start))
    assert rows and rows[-1].address + rows[-1].size == end, hex(start)
    return {row.address: (row.mnemonic, row.op_str) for row in rows}


rows = decode(0x10E4CB5, 0x10E4E21)
rows.update(decode(0x95DB09, 0x95DB27))
rows.update(decode(0xD8DC5A, 0xD8DCCA))
for address, expected in {
    0x10E4CDA: ("and", "eax, 0x1c"),
    0x10E4CDD: ("je", "0x10e4d0f"),
    0x10E4D5C: ("push", "0x59"),
    0x10E4D5E: ("push", "0xc1"),
    0x10E4D69: ("call", "0xd8dc5a"),
    0x10E4D6E: ("push", "1"),
    0x10E4D76: ("call", "0x95db09"),
    0x10E4D7B: ("push", "dword ptr [ebp - 0xf5b0]"),  # destination Y
    0x10E4D81: ("push", "dword ptr [ebp - 0xf5a8]"),  # destination X
    0x10E4D93: ("push", "eax"),  # skill low
    0x10E4DA2: ("push", "eax"),  # target low
    0x10E4DB2: ("push", "eax"),  # skill high
    0x10E4DBF: ("push", "eax"),  # target high, top of stack
    0x10E4DEE: ("push", "0"),
    0x10E4DF0: ("push", "1"),
    0x10E4DF8: ("call", "0xd8de14"),
    0x10E4E0E: ("push", "dword ptr [ebp - 0xf5b0]"),
    0x10E4E14: ("push", "dword ptr [ebp - 0xf5a8]"),
    0x10E4E1A: ("call", "0xd6dc05"),
    0x95DB10: ("push", "1"),
    0x95DB12: ("push", "1"),
    0x95DB1B: ("call", "0xd8dd46"),
    0xD8DC81: ("cmp", "byte ptr [ebp - 8], 0xc1"),
}.items():
    assert rows[address] == expected, hex(address)
assert [address for address, (mnemonic, operand) in rows.items()
        if 0x10E4DC6 <= address <= 0x10E4DE9 and
        (mnemonic, operand) == ("call", "0x95db09")] == [
            0x10E4DC6, 0x10E4DCD, 0x10E4DD4, 0x10E4DDB, 0x10E4DE2, 0x10E4DE9]

root = Path(__file__).resolve().parents[2]
header = (root / "Shared/GrowLancerClashMovementPayload.h").read_text(encoding="utf-8")
assert "ClashBlockedTerrainMask = 0x1C" in header
previous = -1
for token in ("payload[0] = 1;", "payload[1] = static_cast<uint8_t>(targetIndex >> 8);",
              "payload[2] = static_cast<uint8_t>(skillId >> 8);",
              "payload[3] = static_cast<uint8_t>(targetIndex);",
              "payload[4] = static_cast<uint8_t>(skillId);",
              "payload[5] = destinationX;", "payload[6] = destinationY;"):
    previous = header.index(token, previous + 1)
print("PASS S21 Clash blocked terrain0x1C, C1:59 mode1+six chained writer bytes, send then D6DC05 follow-up call; dormant native payload pinned")
print("OPEN movement vector quantization, authoritative GS siege validation/pushback/rollback and native opcode mapping")
