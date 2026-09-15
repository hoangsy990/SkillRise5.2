"""Pinned subtype-12 Circle sprite render-mode comparison, not pixel QA."""
from hashlib import sha256
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
SOURCE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE"
DUMP_SHA = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"


def decoded(image: bytes, start: int, end: int):
    cs = Cs(CS_ARCH_X86, CS_MODE_32)
    block = list(cs.disasm(image[start - IMAGE_BASE:end - IMAGE_BASE], start))
    assert block and block[-1].address + block[-1].size == end
    return block


def main() -> None:
    image = DUMP.read_bytes()
    assert sha256(image).hexdigest() == DUMP_SHA, "S21 dump drift"
    import struct
    assert struct.unpack_from("<I", image,
        0x16410E8 + 12 * 4 - IMAGE_BASE)[0] == 0x1635D58
    body = decoded(image, 0x1635D58, 0x1635DFC)
    assert (body[0].mnemonic, body[0].op_str) == ("push", "4")
    assert (body[-1].mnemonic, body[-1].op_str) == ("add", "esp, 0x2c")
    assert any(i.address == 0x1635DF4 and i.mnemonic == "call" and
               i.op_str == "0x18e8b0c" for i in body), "11-argument sprite callee"
    mode = decoded(image, 0x18E8E9F, 0x18E8EF9)
    for address, bit, axis in ((0x18E8E9F, 1, 0),
                               (0x18E8EBD, 2, 1),
                               (0x18E8EDB, 4, 2)):
        assert any(i.address == address and i.mnemonic == "mov" and
                   i.op_str == "eax, dword ptr [ebp + 0x30]" for i in mode)
        assert any(i.mnemonic == "and" and i.op_str == f"eax, {bit}"
                   for i in mode), "mode bit"
        assert any(i.mnemonic == "push" and i.op_str == str(axis)
                   for i in mode), "rotation axis"
    particle = (SOURCE / "ZzzEffectParticle.cpp").read_text(
        encoding="utf-8", errors="replace")
    renderer = (SOURCE / "ZzzOpenglUtil.cpp").read_text(
        encoding="utf-8", errors="replace")
    assert "case rise::growlancer::kCircleUpperArmMonoBitmap:\n                RenderSprite" in particle
    assert "o->Light, o->Rotation);" in particle
    sprite = renderer.split("void RenderSprite(int Texture, vec3_t Position", 1)[1].split(
        "void RenderSpriteUV", 1
    )[0]
    assert "VectorTransform(Position, CameraMatrix, p2);" in sprite
    assert "Vector(0.f, 0.f, Rotation, Angle);" in sprite
    print("PASS: S21 Circle mono subtype12 mode4 selects camera-space Z rotation; native RenderSprite uses camera transform/Z rotation and source light/scale")
    print("Scope: static renderer contract only; actual shoulder pixels/emission/buff/pool OPEN")


if __name__ == "__main__":
    main()
