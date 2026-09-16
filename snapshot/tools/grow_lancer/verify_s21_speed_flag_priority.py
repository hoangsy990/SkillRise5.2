"""Pin the S21 cast-speed flag1/flag8 priority branch and dormant tail."""

import hashlib
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
rows = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(
    data[0x14086AA - IMAGE_BASE:0x1408716 - IMAGE_BASE], 0x14086AA))
assert rows and rows[-1].address + rows[-1].size == 0x1408716
operations = {row.address: (row.mnemonic, row.op_str) for row in rows}
for address, expected in {
    0x14086B6: ("and", "eax, 1"),
    0x14086B9: ("je", "0x14086e1"),
    0x14086C0: ("addss", "xmm0, dword ptr [0x1b4e4d4]"),
    0x14086D2: ("addss", "xmm0, dword ptr [0x1b4e4d4]"),
    0x14086DF: ("jmp", "0x1408716"),
    0x14086ED: ("and", "eax, 8"),
    0x14086F0: ("je", "0x1408716"),
    0x14086F7: ("addss", "xmm0, dword ptr [0x1b4e4d4]"),
    0x1408709: ("addss", "xmm0, dword ptr [0x1b4e4d4]"),
}.items():
    assert operations[address] == expected, hex(address)
assert struct.unpack_from("<f", data, 0x1B4E4D4 - IMAGE_BASE)[0] == 20.0
root = Path(__file__).resolve().parents[2]
header = (root / "GrowLancer/compat/S21CastAnimationSpeed.h").read_text(encoding="utf-8")
test = (root / "ExMain_RISE_PC/Tests/GrowLancerCastAnimationSpeedTest.cpp").read_text(encoding="utf-8")
assert "if ((sourceFlags & 0x09u) != 0)" in header
assert "static_assert(S21AdjustCastSpeedTail(100,9,false,false,0) == 120);" in test
assert "static_assert(S21AdjustCastSpeedTail(100,9,true,false,0) == 60);" in test
print("PASS source flag1 has priority; true branch skips flag8, so both bits add +20 once, not +40")
print("OPEN actual adjusted source speed producer and native class7 actor/stat mapping")
