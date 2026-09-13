"""Pin reviewed S21 Circle joint renderer instructions; not GPU parity proof."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == (
        "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637")
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoded = {}
    # Decode from actual function/handler boundaries, never a numeric xref.
    for start, end in ((0x15C6594, 0x15C6900), (0x1618F44, 0x161AEB9),
                       (0x18E7137, 0x18E71C3), (0x15E21B9, 0x15E21F1),
                       (0x10F0332, 0x10F0381), (0x15E2756, 0x15E3DE1)):
        decoded.update({i.address: (i.mnemonic, i.op_str) for i in
            decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
    expected = {
        0x15C6850: ("movss", "xmm0, dword ptr [0x1b4dd48]"),
        0x15C6858: ("movss", "dword ptr [eax + 0xa4c], xmm0"),
        0x1619008: ("cmp", "dword ptr [eax + 0x68], 0"),
        0x1619089: ("call", "0x18e7137"),
        0x161940E: ("mulss", "xmm0, dword ptr [ecx + 0xa4c]"),
        0x1619430: ("mulss", "xmm0, dword ptr [ecx + 0xa4c]"),
        0x1619452: ("mulss", "xmm0, dword ptr [ecx + 0xa4c]"),
        0x161946D: ("call", "0x18e7402"),
        0x1619646: ("sub", "eax, dword ptr [ebp - 0x60]"),
        0x1619653: ("dec", "eax"),
        0x1619658: ("divss", "xmm0, xmm1"),
        0x161AA9E: ("cmp", "eax, 2"),
        0x161AAAB: ("subss", "xmm0, dword ptr [ebp - 0x70]"),
        0x161AABD: ("subss", "xmm0, dword ptr [ebp - 0x74]"),
        0x161ABEC: ("je", "0x161ad92"),
        0x161ABE9: ("and", "eax, 2"),
        0x161AC85: ("push", "7"),
        0x18E7158: ("push", "1"),
        0x18E715A: ("push", "1"),
        0x15E21BF: ("cmp", "dword ptr [eax + 0x6c], 0x32"),
        0x15E21CB: ("mov", "dword ptr [eax + 0x6c], 0x32"),
        0x15C65D6: ("cmp", "dword ptr [ebp - 0x234], 0x1f4"),
        0x15C6609: ("call", "0x10f0332"),
        0x15C6610: ("je", "0x15c6622"),
        0x10F0353: ("cmp", "dword ptr [ebp - 4], 0x7fce"),
        0x10F035C: ("jmp", "0x10f037b"),
        0x10F037B: ("xor", "eax, eax"),
        0x15E3BB7: ("call", "0xd3189d"),
        0x15E3CC8: ("mulss", "xmm0, dword ptr [0x1b4df14]"),
        0x15E3CE6: ("call", "0xd3189d"),
        0x15E3D1D: ("imul", "eax, eax, 3"),
    }
    for address, instruction in expected.items():
        assert decoded.get(address) == instruction, (hex(address), decoded.get(address))
    assert struct.unpack_from("<f", data, 0x1B4DD48-IMAGE_BASE)[0] == 1.0
    assert struct.unpack_from("<f", data, 0x1B4DF14-IMAGE_BASE)[0] == .5
    native = (Path(__file__).resolve().parents[2] / "ExMain_RISE_PC" /
              "Main5.2_RISE" / "ZzzEffectJoint.cpp").read_text(encoding="utf-8")
    assert ('if (!(Type == BITMAP_FORCEPILLAR && (SubType == 2 || SubType == 3))\n'
            '                && !(Type == BITMAP_PIN_LIGHT && SubType >= 2 && SubType <= 4))\n'
            '                o->MaxTails = static_cast<int>(o->MaxTails / FPS_ANIMATION_FACTOR);') in native
    print(f"PASS {len(expected)} reviewed renderer/constructor/blend instructions and scalar1")
    print("Circle uses U=1-(NumTails-j)/(MaxTails-1), subtype2 face2; contact3 face1+2, additive ONE/ONE.")
    print("PASS native Circle-only bypass of render-rate MaxTails scaling (source guard)")
    print("PASS pool predicate: Circle80E3 cannot select the7FCE-only auxiliary pool; fixed500 scan")
    print("Scope excludes startup/global resets, external scalar writes and runtime GPU/pool stress.")

if __name__ == "__main__":
    main()
