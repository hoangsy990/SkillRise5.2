"""Verify the S21 Breche particle render branch and its native RISE adapter.

The binary checks pin the selected S21 dispatch/primitive calls.  The source
checks make sure only the private Breche particle marker takes the adapter and
that ordinary SS6 particles keep their existing switch.  This remains static
evidence; it does not assert GPU pixels or owner visual parity.
"""

from hashlib import sha256
from pathlib import Path

from capstone import CS_ARCH_X86, CS_MODE_32, Cs


ROOT = Path(__file__).resolve().parents[2]
DUMP = Path(r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe")
IMAGE_BASE = 0x400000
DUMP_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
PARTICLE_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffectParticle.cpp"
SPRITE_H = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerSpriteAdapter.h"


def decode(data: bytes, start: int, end: int):
    blob = data[start - IMAGE_BASE:end - IMAGE_BASE]
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.skipdata = False
    rows = {ins.address: (ins.mnemonic, ins.op_str)
            for ins in decoder.disasm(blob, start)}
    if not rows or sum(ins.size for ins in decoder.disasm(blob, start)) != end - start:
        raise ValueError(f"incomplete S21 decode at {start:#x}")
    return rows


def require(rows, address: int, mnemonic: str, operands: str) -> None:
    actual = rows.get(address)
    if actual != (mnemonic, operands):
        raise ValueError(f"S21 render mismatch at {address:#x}: {actual!r}")


def main() -> None:
    data = DUMP.read_bytes()
    if sha256(data).hexdigest().upper() != DUMP_SHA256:
        raise ValueError("preserved S21 image dump hash changed")

    common = decode(data, 0x1620984, 0x1620B67)
    require(common, 0x1620A3E, "movzx", "eax, byte ptr [eax + 8]")
    require(common, 0x1620A6E, "comiss", "xmm0, dword ptr [0x1b4eae0]")
    require(common, 0x1620AAE, "movss", "xmm0, dword ptr [0x1b4e4e8]")
    require(common, 0x1620AB6, "comiss", "xmm0, dword ptr [eax]")
    require(common, 0x1620AFF, "movss", "xmm0, dword ptr [ecx + 0x204]")
    require(common, 0x1620B25, "movss", "xmm0, dword ptr [ecx + 0x208]")
    require(common, 0x1620B3A, "mov", "eax, dword ptr [ebp - 0x414]")
    require(common, 0x1620B4C, "call", "0x18e7137")

    fire_hik1 = decode(data, 0x1635D58, 0x1635E01)
    fire_cursed = decode(data, 0x1627B36, 0x1627BDF)
    fire_hik3 = decode(data, 0x1624BE9, 0x1624C92)
    require(fire_hik1, 0x1635D58, "push", "4")
    require(fire_cursed, 0x1627B36, "push", "4")
    require(fire_hik3, 0x1624BE9, "push", "4")
    require(fire_hik1, 0x1635DF4, "call", "0x18e8b0c")
    require(fire_cursed, 0x1627BD2, "call", "0x18e8b0c")
    require(fire_hik3, 0x1624C85, "call", "0x18e8b0c")

    particle = PARTICLE_CPP.read_text(encoding="utf-8")
    sprite = SPRITE_H.read_text(encoding="utf-8")
    marker = "if (g_brecheParticleMode[i] != 0)"
    marker_at = particle.index(marker, particle.index("void RenderParticles"))
    render_block = particle[marker_at:marker_at + 360]
    for needle in (
        "RenderBrecheFireSprite(o->TexType,",
        "o->Position, Width, Height, o->Light, o->Rotation",
        "else\n            {",
    ):
        if needle not in render_block:
            raise ValueError(f"missing private Breche particle render route: {needle}")
    if "continue;" in render_block:
        raise ValueError("Breche private render route must not bypass the common post-render path")
    if "FPS_ANIMATION_FACTOR" in render_block:
        raise ValueError("Breche private render route must not use generic FPS scaling")
    if "RenderSprite(texture, position, width, height, light, rotation);" not in sprite:
        raise ValueError("private adapter no longer reaches native full-UV RenderSprite")

    print("PASS: S21 particle gates, dimensions, component blend and 8084/806E/8085 billboard calls pinned")
    print("PASS: RISE private Breche marker renders once then reaches the common post-render path")
    print("PASS: ordinary SS6 particle switch remains in the non-Breche branch")
    print("Scope: static renderer contract only; GPU pixels, pool retirement and owner visual parity remain open")


if __name__ == "__main__":
    main()
