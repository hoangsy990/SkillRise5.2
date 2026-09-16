"""Pin S21 global speed snapshot and actor-only Tornado gate vs native owner."""

import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
rows = list(decoder.disasm(data[0x1408580 - IMAGE_BASE:0x14087C8 - IMAGE_BASE], 0x1408580))
assert rows and rows[-1].address + rows[-1].size == 0x14087C8
source = {row.address: (row.mnemonic, row.op_str) for row in rows}
for address, expected in {
    0x1408586: ("mov", "eax, dword ptr [0x1e04fbc]"),
    0x140858B: ("movzx", "eax, word ptr [eax + 0x1b6]"),
    0x140859B: ("mov", "eax, dword ptr [0x1e04fbc]"),
    0x14085A0: ("movzx", "eax, word ptr [eax + 0x1c2]"),
    0x1408716: ("cmp", "dword ptr [ebp + 8], 0"),
    0x1408721: ("mov", "ecx, dword ptr [ebp + 8]"),
    0x140872A: ("call", "0x130c8a5"),
    0x140873B: ("mulss", "xmm0, dword ptr [0x1b4df14]"),
}.items():
    assert source[address] == expected, hex(address)
actor_reads = [row.address for row in rows if "[ebp + 8]" in row.op_str]
assert actor_reads == [0x1408716, 0x1408721], actor_reads

call = list(decoder.disasm(data[0x140DD63 - IMAGE_BASE:0x140DD87 - IMAGE_BASE], 0x140DD63))
assert call and call[-1].address + call[-1].size == 0x140DD87
caller = {row.address: (row.mnemonic, row.op_str) for row in call}
assert caller[0x140DD71] == ("cmp", "dword ptr [eax + 0x5c], 0xa94")
assert caller[0x140DD7E] == ("push", "dword ptr [ebp - 0x14]")
assert caller[0x140DD81] == ("call", "0x1408580")

root = Path(__file__).resolve().parents[2] / "ExMain_RISE_PC/Main5.2_RISE"
native = (root / "ZzzCharacter.cpp").read_text(encoding="latin-1")
setter = native.split("void SetAttackSpeed()", 1)[1].split("void ", 1)[0]
assert "CharacterAttribute->AttackSpeed" in setter
assert "CharacterAttribute->MagicSpeed" in setter
ws = (root / "WSclient.cpp").read_text(encoding="latin-1")
assert "CharacterAttribute->AttackSpeed" in ws and "Data->SpeedAttack" in ws
assert "CharacterAttribute->MagicSpeed" in ws and "Data->MagicAttack" in ws
print("PASS S21 player-model cast-speed base is global stat1E04FBC +1B6/+1C2; actor arg is read only at optional Tornado gate")
print("PASS native model PlaySpeed also derives from global local CharacterAttribute; no source proof of per-remote base speed")
print("OPEN wire-width/value equivalence, source manager bonus/cap semantics, class7 normal actor/default and Tornado mapping")
