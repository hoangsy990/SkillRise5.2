"""Pin both S21 Harsh action185 producers to the native private setter."""

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


rows = decode(0x10E385B, 0x10E38FA)
rows.update(decode(0x12CB122, 0x12CB1B7))
rows.update(decode(0x1327D72, 0x1327DE8))
for address, expected in {
    0x10E385B: ("push", "-1"),
    0x10E385D: ("push", "0xb9"),
    0x10E3873: ("call", "0x1327de8"),
    0x10E38DC: ("push", "0x5e0"),
    0x10E38E1: ("call", "0x143e57c"),
    0x12CB12E: ("je", "0x12cb1ad"),
    0x12CB130: ("push", "-1"),
    0x12CB132: ("push", "0xb9"),
    0x12CB143: ("call", "0x1327de8"),
    0x12CB1A0: ("push", "0x5e0"),
    0x12CB1A5: ("call", "0x143e57c"),
    0x1327DAB: ("je", "0x1327de6"),
    0x1327DB7: ("mov", "word ptr [eax + 0x2c], cx"),
    0x1327DC7: ("mov", "dword ptr [eax + 0xd4], ecx"),
}.items():
    assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
harsh = runtime.split("void CreateHarshStrikeRoot(OBJECT& caster)", 1)[1].split("\n}", 1)[0]
assert harsh.index("PrepareLocalQADynamicAction(caster, 284)") < harsh.index("SetAction(&caster, 284, true);")
assert harsh.index("SetAction(&caster, 284, true);") < harsh.index("CreateEffect(kHarshStrikeControllerModel")
for forbidden in ("caster.CurrentAction =", "caster.AnimationFrame =", "caster.PriorAnimationFrame ="):
    assert forbidden not in harsh, forbidden
print("PASS S21 local/receive action185 setter precedes controller5E0; private action284 uses native SetAction")
print("OPEN owner action/child timing visual and ordinary server-authoritative activation")
