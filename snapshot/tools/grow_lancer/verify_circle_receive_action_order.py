"""Pin Circle Shield send-only local branch and receive root-before-setter."""

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


local = decode(0x10E3E77, 0x10E3FF2)
assert local[0x10E3FED] == ("jmp", "0x10e5216")
assert not any((mnemonic == "push" and operand == "0x5dd") or
               (mnemonic == "call" and operand == "0x1327de8")
               for mnemonic, operand in local.values())

receive = decode(0x12CB2F7, 0x12CB392)
for address, expected in {
    0x12CB2FD: ("mov", "byte ptr [eax + 0x21], 1"),
    0x12CB356: ("push", "0x5dd"),
    0x12CB35B: ("call", "0x143e57c"),
    0x12CB363: ("push", "0x13c"),
    0x12CB368: ("push", "0xbb"),
    0x12CB379: ("call", "0x1327de8"),
}.items():
    assert receive[address] == expected, hex(address)

setter = decode(0x1327DE8, 0x1327E57)
assert setter[0x1327E27] == ("cmp", "dword ptr [ebp + 0x14], -1")
assert setter[0x1327E39] == ("call", "0x1327d72")
assert setter[0x1327E4E] == ("call", "0x1327d72")

root = Path(__file__).resolve().parents[2]
runtime = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
circle = runtime.split("void CreateCircleShieldRoot(OBJECT& caster)", 1)[1].split("\n}", 1)[0]
assert circle.index("PrepareFixedPlayerAction(caster, 286)") < circle.index("CreateEffect(kCircleShieldControllerModel")
assert circle.index("CreateEffect(kCircleShieldControllerModel") < circle.index("SetAction(&caster, 286, true);")
for forbidden in ("caster.CurrentAction =", "caster.AnimationFrame =", "caster.PriorAnimationFrame ="):
    assert forbidden not in circle, forbidden
print("PASS S21 Circle local send-only, receive controller5DD BEFORE primary187/alternate316 setter; private primary286 native order matches")
print("OPEN auxiliary316 predicate/clip, actual receiver class7 activation, owner visual/buff GS authority")
