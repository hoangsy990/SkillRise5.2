"""Pin Wrath primary192/conditional321 source setter and private primary map."""

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


rows = decode(0x10E4894, 0x10E48C8)
rows.update(decode(0x12CB6AC, 0x12CB737))
rows.update(decode(0x1327DE8, 0x1327E57))
rows.update(decode(0x1327D72, 0x1327DE8))
for address, expected in {
    0x10E4894: ("push", "0x141"),
    0x10E4899: ("push", "0xc0"),
    0x10E48AF: ("call", "0x1327de8"),
    0x12CB6B8: ("je", "0x12cb6d8"),
    0x12CB6BA: ("push", "0x141"),
    0x12CB6BF: ("push", "0xc0"),
    0x12CB6D0: ("call", "0x1327de8"),
    0x12CB72D: ("push", "0x5ea"),
    0x1327E27: ("cmp", "dword ptr [ebp + 0x14], -1"),
    0x1327E39: ("call", "0x1327d72"),
    0x1327E4E: ("call", "0x1327d72"),
    0x1327DAB: ("je", "0x1327de6"),
}.items():
    assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
wrath = runtime.split("void CreateWrathRoot(OBJECT& caster)", 1)[1].split("\n}", 1)[0]
assert wrath.index("PrepareFixedPlayerAction(caster, 291)") < wrath.index("SetAction(&caster, 291, true);")
assert wrath.index("SetAction(&caster, 291, true);") < wrath.index("CreateEffect(kWrathControllerModel")
for forbidden in ("caster.CurrentAction =", "caster.AnimationFrame =", "caster.PriorAnimationFrame ="):
    assert forbidden not in wrath, forbidden
print("PASS S21 Wrath local/receive primary192/conditional321 setter; private primary291 uses native SetAction")
print("OPEN auxiliary321 predicate/native clip, owner ground pixels and GS buff authority")
