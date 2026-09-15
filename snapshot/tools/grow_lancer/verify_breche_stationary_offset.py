"""Guard the audited Breche child offset/movement conclusion.

This is a pinned S21 disassembly and native-source check only.  It does not
claim owner pixels, terrain eligibility, contact or runtime visual parity.
"""

from __future__ import annotations

import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


ROOT = Path(__file__).resolve().parents[2]
DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
BASE = 0x400000
PINNED_SHA256 = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"


def decode(data: bytes, start: int, end: int):
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    return list(decoder.disasm(data[start - BASE : end - BASE], start))


def main() -> None:
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == PINNED_SHA256

    # The generic S21 constructor clears OBJECT+0xF8 (Direction) one scalar at
    # a time immediately after the selected record is allocated.
    # Start at the first instruction boundary; 0x143EB00 is in the tail of
    # the preceding basic block and would make a linear decoder stop early.
    generic = decode(data, 0x143EB14, 0x143EB60)
    zero_stores = [
        instruction.address
        for instruction in generic
        if instruction.mnemonic == "movss"
        and instruction.op_str == "dword ptr [eax], xmm0"
    ]
    assert zero_stores == [0x143EB22, 0x143EB3C, 0x143EB56]
    assert sum(i.mnemonic == "add" and i.op_str == "ecx, 0xf8" for i in generic) == 3

    # The four Breche child calls are in one producer block.  No parent-side
    # write to +0xF8 follows those calls for 809F/809E/809E; their common
    # movement therefore sees only the constructor's zero vector.
    producer = decode(data, 0x1482600, 0x1482820)
    calls = {
        i.address: i.op_str.lower()
        for i in producer
        if i.mnemonic == "call" and i.op_str.lower() == "0x143e57c"
    }
    assert set(calls) == {0x1482645, 0x14826A7, 0x148274E, 0x14827B0, 0x1482818}
    assert not any("0xf8" in i.op_str.lower() for i in producer)

    # 806E/sub15's selected constructor branch also explicitly zeros the
    # same three components before returning to the generic tail.
    fire_branch = decode(data, 0x144280E, 0x14428FB)
    assert sum(
        i.mnemonic == "add" and i.op_str == "ecx, 0xf8" for i in fire_branch
    ) == 3
    assert sum(
        i.mnemonic == "movss" and i.op_str == "dword ptr [eax], xmm0"
        for i in fire_branch
    ) == 3

    # 81EC/sub0's selected update branch changes lifetime/alpha only; it has
    # no Direction write before its common-tail jump.
    aura_branch = decode(data, 0x1482BFD, 0x1482D1B)
    assert not any("0xf8" in i.op_str.lower() for i in aura_branch)

    native = (
        ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp"
    ).read_text(encoding="latin-1")
    assert "Vector(0.f, 0.f, 0.f, o->Direction);" in native
    print(
        "PASS: pinned Breche child constructors keep Direction zero; "
        "common movement remains stationary at copied target position"
    )
    print(
        "Scope: source/static boundary only; target contact, terrain culling, "
        "owner pixels and runtime parity remain open"
    )


if __name__ == "__main__":
    main()
