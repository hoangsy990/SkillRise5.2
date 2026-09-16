"""Pin Clash destination normalization/rounding and second coordinate send."""

import hashlib
import struct
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


rows = decode(0x10E4B67, 0x10E4CB5)
rows.update(decode(0xD2E703, 0xD2E71E))
rows.update(decode(0xD2E7AF, 0xD2E7C5))
rows.update(decode(0xD2E84D, 0xD2E8FC))
rows.update(decode(0xD336B8, 0xD33705))
rows.update(decode(0xA7CB95, 0xA7CBC8))
rows.update(decode(0xD6DC05, 0xD6DCA3))
for address, expected in {
    0x10E4B6F: ("cvtsi2ss", "xmm0, dword ptr [0xb51ecd8]"),
    0x10E4B7F: ("cvtsi2ss", "xmm1, dword ptr [eax + 0x184]"),
    0x10E4B87: ("subss", "xmm0, xmm1"),
    0x10E4B91: ("cvtsi2ss", "xmm0, dword ptr [0xb51ecd4]"),
    0x10E4BA1: ("cvtsi2ss", "xmm1, dword ptr [eax + 0x180]"),
    0x10E4BA9: ("subss", "xmm0, xmm1"),
    0x10E4BC8: ("call", "0xd2e703"),
    0x10E4BD9: ("call", "0xd2e7af"),
    0x10E4BDE: ("cvttss2si", "eax, dword ptr [ebp - 0xf5fc]"),
    0x10E4BF6: ("call", "0xd2e436"),
    0x10E4C12: ("call", "0xd336b8"),
    0x10E4C4A: ("call", "0xd336b8"),
    0x10E4C80: ("cvttss2si", "eax, dword ptr [eax]"),
    0x10E4C84: ("add", "eax, dword ptr [esi + 0x180]"),
    0x10E4CA5: ("cvttss2si", "eax, dword ptr [eax]"),
    0x10E4CA9: ("add", "eax, dword ptr [esi + 0x184]"),
    0xD2E87D: ("push", "ecx"),
    0xD2E888: ("call", "0xa7cb95"),
    0xD2E8A1: ("movss", "xmm0, dword ptr [0x1b4dd48]"),
    0xD2E8A9: ("divss", "xmm0, dword ptr [ebp - 0x10]"),
    0xD336C1: ("comiss", "xmm0, dword ptr [0x1b4dd3c]"),
    0xD336CF: ("addss", "xmm0, dword ptr [0x1b4df14]"),
    0xD336EB: ("subss", "xmm0, dword ptr [0x1b4df14]"),
    0xD6DC43: ("push", "0x31"),
    0xD6DC45: ("push", "0xc1"),
    0xD6DC50: ("call", "0xd8dc5a"),
    0xD6DC55: ("push", "dword ptr [ebp + 0xc]"),
    0xD6DC58: ("push", "dword ptr [ebp + 8]"),
    0xD6DC61: ("call", "0x95db09"),
    0xD6DC68: ("call", "0x95db09"),
    0xD6DC77: ("call", "0xd8de14"),
}.items():
    assert rows[address] == expected, hex(address)
for address, expected in {0x1B509C0: 1e-6, 0x1B509C4: -1e-6,
                          0x1B4DD48: 1.0, 0x1B4DF14: 0.5,
                          0x1B4DD3C: 0.0}.items():
    actual = struct.unpack_from("<f", data, address - IMAGE_BASE)[0]
    assert abs(actual - expected) < 1e-11, hex(address)
root = Path(__file__).resolve().parents[2]
helper = (root / "Shared/GrowLancerClashDestination.h").read_text(encoding="utf-8")
previous = -1
for token in ("const float squared = x * x + y * y;",
              "sqrt(static_cast<double>(squared))",
              "length > -0.000001f && length < 0.000001f",
              "const float reciprocal = 1.0f / length;",
              "static_cast<int>(length)",
              "x *= extent;", "y *= extent;",
              "x > 0.0f ? x + 0.5f : x - 0.5f",
              "y > 0.0f ? y + 0.5f : y - 0.5f",
              "static_cast<int>(caster.x) + offsetX",
              "static_cast<int>(caster.y) + offsetY"):
    previous = helper.index(token, previous + 1)
print("PASS S21 Clash captures Euclidean length, normalizes near-zero-aware, scales by truncated original length, half-away rounds X/Y, adds caster tiles")
print("CORRECTION D6DC05 SENDS second C1:31 two-coordinate packet; it does NOT mutate local character position. Native 5.2 C1:31 is NPC close, so routing remains disabled.")
