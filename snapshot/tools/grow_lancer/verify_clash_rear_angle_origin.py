"""Pin S21 Clash rear CreateEffect angle/origin arguments versus scoped RISE."""

import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32

from disasm_s21_window import DUMP, IMAGE_BASE


EXPECTED_DUMP_SHA = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"


def main() -> None:
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == EXPECTED_DUMP_SHA
    start, end = 0x157DE7E, 0x157DED8
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    instructions = {item.address: (item.mnemonic, item.op_str) for item in
                    decoder.disasm(data[start - IMAGE_BASE:end - IMAGE_BASE], start)}
    expected = {
        0x157DE85: ("movzx", "eax, word ptr [eax + 0x364]"),
        0x157DEA3: ("push", "dword ptr [ebp - 0x11a04]"),
        0x157DEB1: ("push", "eax"),
        0x157DEB2: ("mov", "ecx, dword ptr [ebp - 0x11a04]"),
        0x157DEB8: ("add", "ecx, 0x164"),
        0x157DEBE: ("call", "0xd2dff6"),
        0x157DEC3: ("push", "eax"),
        0x157DEC4: ("lea", "eax, [ebp - 0x2c4c]"),
        0x157DECA: ("push", "eax"),
        0x157DECB: ("push", "0x5fc"),
        0x157DED0: ("call", "0x143e57c"),
    }
    for address, pair in expected.items():
        assert instructions[address] == pair, (hex(address), instructions.get(address))
    identity = {item.address: (item.mnemonic, item.op_str) for item in
                decoder.disasm(data[0xD2DFF6 - IMAGE_BASE:0xD2E002 - IMAGE_BASE], 0xD2DFF6)}
    assert identity[0xD2DFFD] == ("mov", "eax, dword ptr [ebp - 4]")
    root = Path(__file__).resolve().parents[2]
    native = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(
        encoding="utf-8", errors="replace")
    rear = native[native.index("ClashInitialPosition(effect.Owner->Position, direction, false, position);"):]
    assert "CreateEffect(kClashRearModel, position, effect.Owner->Angle, light," in rear
    assert "effect.m_sTargetIndex);" in rear[:400]
    assert rear.index("CreateEffect(kClashRearModel") < rear.index("ClashInitialPosition(effect.Owner->Position, direction, true, position);")
    bmd = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.h").read_text(encoding="utf-8", errors="replace")
    assert "#define RENDER_TEXTURE      0x00000002" in bmd
    assert "#define RENDER_BRIGHT       0x00000040" in bmd
    print("PASS pinned S21 rear5FC uses caster+0x164 angle pointer, caster owner/index and computed position; native uses scoped owner.Angle/position/target")
    print("PASS native RISE render flags texture2|bright40=0x42; S21 actual shared raster state still OPEN")


if __name__ == "__main__":
    main()
