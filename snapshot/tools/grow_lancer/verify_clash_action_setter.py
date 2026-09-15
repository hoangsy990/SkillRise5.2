"""Pin target validation and preserving action194 before Clash controller5FA."""

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


local = decode(0x10E4A8C, 0x10E4B64)
receive = decode(0x12E069D, 0x12E073C)
setter = decode(0x1327D72, 0x1327DE8)
for rows, checks in (
    (local, {
        0x10E4AA7: ("cmp", "dword ptr [ebp - 0xf5ec], 0"),
        0x10E4AB0: ("jmp", "0x10e5216"),
        0x10E4AB5: ("push", "-1"),
        0x10E4AB7: ("push", "0xc2"),
        0x10E4ACD: ("call", "0x1327de8"),
        0x10E4B5A: ("push", "0x5fa"),
        0x10E4B5F: ("call", "0x143e57c"),
    }),
    (receive, {
        0x12E069D: ("push", "-1"),
        0x12E069F: ("push", "0xc2"),
        0x12E06B0: ("call", "0x1327de8"),
        0x12E072F: ("push", "0x5fa"),
        0x12E0734: ("call", "0x143e57c"),
    }),
    (setter, {0x1327DAB: ("je", "0x1327de6"),
              0x1327DB7: ("mov", "word ptr [eax + 0x2c], cx"),
              0x1327DC7: ("mov", "dword ptr [eax + 0xd4], ecx")}),
):
    for address, expected in checks.items():
        assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
clash = runtime.split("void CreateClashRoot(OBJECT& caster, short targetIndex)", 1)[1].split("\n}", 1)[0]
assert clash.index("ResolveTarget(targetIndex, target)") < clash.index("SetAction(&caster, 293, true);")
assert clash.index("PrepareFixedPlayerAction(caster, 293)") < clash.index("SetAction(&caster, 293, true);")
assert clash.index("SetAction(&caster, 293, true);") < clash.index("CreateEffect(kClashControllerModel")
for forbidden in ("caster.CurrentAction =", "caster.AnimationFrame =", "caster.PriorAnimationFrame ="):
    assert forbidden not in clash, forbidden
print("PASS S21 Clash local target->action194->controller5FA and receive action194->controller5FA; private action293 uses native preserving setter")
print("OPEN siege GS validity/pushback, world movement packet and owner visual parity")
