"""Pin S21 Spin ribbon's transform semantics to native BMD API, read-only."""

from hashlib import sha256
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


ROOT = Path(__file__).resolve().parents[2]
DUMP = Path(r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
            r"\season21_client_qa\memory_dump\process_23824\400000.main.exe")
DUMP_SHA = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
BASE = 0x400000


def instructions(image: bytes, start: int, end: int):
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    code = list(decoder.disasm(image[start - BASE:end - BASE], start))
    assert code and code[-1].address + code[-1].size == end, hex(start)
    return {item.address: (item.mnemonic, item.op_str) for item in code}


def main() -> None:
    image = DUMP.read_bytes()
    assert sha256(image).hexdigest().upper() == DUMP_SHA
    transform = instructions(image, 0x132F433, 0x132F56F)
    assert transform[0x132F468] == ("call", "0xd31aa2")
    assert transform[0x132F479] == ("movss", "xmm0, dword ptr [ecx + 0x74]")
    assert transform[0x132F4D9] == ("add", "ecx, 0x78")
    assert transform[0x132F4E7] == ("addss", "xmm0, dword ptr [eax]")
    assert transform[0x132F56C] == ("ret", "0x10")
    caller = instructions(image, 0x157C828, 0x157CA4A)
    assert caller[0x157C828] == ("push", "1")
    assert caller[0x157C84A] == ("call", "0x132f433")
    assert caller[0x157C84F] == ("push", "1")
    assert caller[0x157C871] == ("call", "0x132f433")
    assert caller[0x157C8EC] == ("call", "0xd3189d")
    assert caller[0x157CA45] == ("call", "0x15c01cc")
    native = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzBMD.cpp").read_text(
        encoding="utf-8", errors="replace")
    body = native.split("void BMD::TransformPosition(", 1)[1].split(
        "void BMD::RotationPosition(", 1)[0]
    for required in ("VectorTransform(Position, Matrix, p)",
                     "VectorScale(p, BodyScale, p)",
                     "VectorAdd(p, BodyOrigin, WorldPosition)"):
        assert required in body, required
    runtime = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" /
               "GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
    ribbon = runtime.split("void EmitSpinStepWeaponBlur(", 1)[1].split(
        "void CreateCarrier(", 1)[0]
    assert "VectorCopy(owner->Position, ownerModel.BodyOrigin)" in ribbon
    assert ribbon.count("ownerModel.TransformPosition(BoneTransform[33]") == 2
    assert "VectorAdd(start, rotatedOffset, start)" in ribbon
    assert "VectorAdd(end, rotatedOffset, end)" in ribbon
    print("PASS S21 translated bone33 endpoints + rotated local offset map to native BMD API")
    print("OPEN actual owner pose/origin/world coordinates and gameplay pixel visibility")


if __name__ == "__main__":
    main()
