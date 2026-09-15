"""Pin source Obsidian setter behavior and the isolated action191->290 port."""

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


rows = decode(0x10E46D5, 0x10E46F5)
rows.update(decode(0x1327D72, 0x1327DE8))
rows.update(decode(0x1327DE8, 0x1327E57))
for address, expected in {
    0x10E46D5: ("push", "0x13d"),  # conditional alternate317
    0x10E46DA: ("push", "0xbf"),   # primary191
    0x10E46F0: ("call", "0x1327de8"),
    0x1327E42: ("push", "dword ptr [ebp + 0x10]"),
    0x1327E4E: ("call", "0x1327d72"),
    0x1327DAB: ("je", "0x1327de6"),  # same action returns
    0x1327DB7: ("mov", "word ptr [eax + 0x2c], cx"),
    0x1327DC7: ("mov", "dword ptr [eax + 0xd4], ecx"),
    0x1327DD4: ("mov", "word ptr [eax + 0x2a], cx"),
    0x1327DDE: ("movss", "dword ptr [eax + 0xd0], xmm0"),
}.items():
    assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
setter_source = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzAI.cpp").read_text(encoding="latin-1")
setter = setter_source.split("void SetAction(OBJECT* o, int Action, bool bBlending)", 1)[1].split("bool TestDistance", 1)[0]
for token in ("if (o->CurrentAction != Action)", "o->PriorAction = o->CurrentAction;",
              "o->PriorAnimationFrame = o->AnimationFrame;", "o->AnimationFrame = 0;"):
    assert token in setter, token

runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
obsidian = runtime.split("void CreateObsidianRoots(OBJECT& caster)", 1)[1].split("\n}", 1)[0]
assert obsidian.index("PrepareFixedPlayerAction(caster, 290)") < obsidian.index("SetAction(&caster, 290, true);")
assert "caster.CurrentAction =" not in obsidian
assert "caster.AnimationFrame =" not in obsidian
assert "caster.PriorAnimationFrame =" not in obsidian
print("PASS S21 Obsidian primary191/alternate317 use preserving setter; isolated primary290 uses native SetAction(true)")
print("OPEN source auxiliary317 predicate/native clip and owner visual/party-buff server authority")
