"""Pin source two-word speed bonuses and a dormant source-domain composer."""

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


rows = decode(0x1408580, 0x14086AA)
rows.update(decode(0xC5B813, 0xC5B8B3))
rows.update(decode(0xB60918, 0xB60931))
rows.update(decode(0x10FC071, 0x10FC0BA))
for address, expected in {
    0x140858B: ("movzx", "eax, word ptr [eax + 0x1b6]"),
    0x14085A0: ("movzx", "eax, word ptr [eax + 0x1c2]"),
    0x14085C4: ("push", "1"),
    0x14085D4: ("call", "0xc5b813"),
    0x14085E1: ("addss", "xmm0, dword ptr [ebp - 8]"),
    0x14085F0: ("addss", "xmm0, dword ptr [ebp - 4]"),
    0x1408666: ("call", "0xc4ff95"),
    0x1408672: ("je", "0x14086aa"),
    0x1408674: ("push", "0x3f0"),
    0x140867C: ("call", "0xb60918"),
    0x1408685: ("addss", "xmm0, dword ptr [ebp - 8]"),
    0x140868F: ("push", "0x3f0"),
    0x1408697: ("call", "0xb60918"),
    0x14086A0: ("addss", "xmm0, dword ptr [ebp - 4]"),
    0xC5B840: ("cmp", "dword ptr [ebp - 4], 3"),
    0xC5B854: ("cmp", "dword ptr [eax + 0x18], 0"),
    0xC5B88C: ("cmp", "eax, dword ptr [ebp - 0x18]"),
    0xC5B8A4: ("add", "ecx, dword ptr [eax + 0x1c]"),
    0xB6091F: ("push", "dword ptr [ebp + 8]"),
    0xB60928: ("call", "0x10fc071"),
    0x10FC0B4: ("xor", "eax, eax"),
}.items():
    assert rows[address] == expected, hex(address)

root = Path(__file__).resolve().parents[2]
header = (root / "GrowLancer/compat/S21CastSpeedProducer.h").read_text(encoding="utf-8")
test = (root / "tools/grow_lancer/test_s21_cast_speed_producer.cpp").read_text(encoding="utf-8")
for token in ("static_cast<float>(sourceAttack) + static_cast<float>(managerType1Bonus)",
              "static_cast<float>(sourceMagic) + static_cast<float>(managerType1Bonus)",
              "static_cast<float>(lookup1008Bonus)", "S21AdjustCastSpeedTail("):
    assert token in header, token
assert "both.attack == 140 && both.magic == 110" in test
assert "capped.attack == 55 && capped.magic == 55" in test
print("PASS source +1B6/+1C2 words, manager three-slot type1 sum and optional lookup1008 return added to BOTH speed locals before flag/tornado/cap tail")
print("OPEN manager/type semantic attribution, actual native class7 buff/stat bridge and remote actor adjusted-speed ownership")
