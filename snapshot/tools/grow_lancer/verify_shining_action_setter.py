"""Pin S21 Shining Peak action189 before both caster-owned roots."""

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


rows = decode(0x10E42B9, 0x10E43B2)
rows.update(decode(0x12D09E7, 0x12D0AC6))
rows.update(decode(0x1327D72, 0x1327DE8))
for address, expected in {
    0x10E42B9: ("push", "-1"),
    0x10E42BB: ("push", "0xbd"),
    0x10E42D1: ("call", "0x1327de8"),
    0x10E433A: ("push", "0x5f6"),
    0x10E43A8: ("push", "0x5f3"),
    0x12D09E7: ("push", "-1"),
    0x12D09E9: ("push", "0xbd"),
    0x12D09FA: ("call", "0x1327de8"),
    0x12D0A57: ("push", "0x5f6"),
    0x12D0AB9: ("push", "0x5f3"),
    0x1327DAB: ("je", "0x1327de6"),
}.items():
    assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
shining = runtime.split("void CreateShiningPeakRoots(OBJECT& caster)", 1)[1].split("\n}", 1)[0]
assert shining.index("PrepareLocalQADynamicAction(caster, 288)") < shining.index("SetAction(&caster, 288, true);")
assert shining.index("SetAction(&caster, 288, true);") < shining.index("CreateEffect(kShiningPeakControllerModel")
for forbidden in ("caster.CurrentAction =", "caster.AnimationFrame =", "caster.PriorAnimationFrame ="):
    assert forbidden not in shining, forbidden
print("PASS S21 local/receive Shining action189 setter precedes both roots; private action288 uses native SetAction")
print("OPEN owner four-stage pin pixels, joint pool/render parity, class7 ordinary activation and GS authority")
