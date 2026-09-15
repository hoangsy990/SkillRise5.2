"""Pinned S21 motion-blur sampler and isolated native style-1 adapter."""

import hashlib
import json
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main() -> None:
    root = Path(__file__).resolve().parents[2]
    image = DUMP.read_bytes()
    assert hashlib.sha256(image).hexdigest() == (
        "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    )
    instructions = {i.address: (i.mnemonic, i.op_str) for i in Cs(
        CS_ARCH_X86, CS_MODE_32).disasm(
        image[0x18BDDB8 - IMAGE_BASE : 0x18BDDD8 - IMAGE_BASE], 0x18BDDB8)}
    for address, expected in {
        0x18BDDB8: ("push", "0"),
        0x18BDDBA: ("push", "1"),
        0x18BDDBC: ("push", "0x2900"),
        0x18BDDC1: ("push", "0x2600"),
        0x18BDDC6: ("push", "0x7f08"),
        0x18BDDD0: ("call", "0x18f8fa2"),
    }.items():
        assert instructions[address] == expected, hex(address)
    assert image[0x1BB9540 - IMAGE_BASE : 0x1BB9540 - IMAGE_BASE + 23] == (
        b"Effect\\motion_blur.jpg\0"
    )
    asset = root / "ExMain_RISE_PC/Tests/GrowLancerBuild/Client/Data/RISE/GrowLancer/SpinStep/motion_blur.OZJ"
    qa_asset = root / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/RISE/GrowLancer/SpinStep/motion_blur.OZJ"
    source = Path(r"D:\MU FICA Season 21\Data\Effect\motion_blur.OZJ")
    expected_hash = "934716E4C09EBCA25B43E62BC1F6B69B26663FA54306F5054FA12A354B6DC5AF"
    for path in (source, asset, qa_asset):
        assert hashlib.sha256(path.read_bytes()).hexdigest().upper() == expected_hash, path
    provenance = json.loads((root / "GrowLancer/asset_provenance.json").read_text(encoding="utf-8"))
    rows = [row for row in provenance["assets"] if row.get("role") == "caster_weapon_ribbon_style1"]
    assert len(rows) == 1
    row = rows[0]
    assert row["skillId"] == 271 and row["s21RuntimeId"] == 0x7F08
    assert row["s21Sampler"] == {"min": "GL_NEAREST", "mag": "GL_NEAREST",
                                  "wrapS": "GL_CLAMP", "wrapT": "GL_CLAMP"}
    descriptor = json.loads((root / "GrowLancer/grow_lancer_skills_descriptor.json").read_text(encoding="utf-8"))
    spin = next(skill for skill in descriptor["skills"] if skill["id"] == 271)
    ribbon = next(layer for layer in spin["visualLayers"]
                  if layer["role"] == "caster_weapon_ribbon_style1")
    assert ribbon["risePrivateBitmapId"] == 32986 and ribbon["s21ResourceId"] == 0x7F08
    assert ribbon["s21Sampler"] == row["s21Sampler"]
    assert ribbon["attachmentBone"] == 33 and ribbon["renderStyle"] == 1
    header = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.h").read_text(encoding="utf-8")
    loader = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp").read_text(encoding="utf-8")
    effect = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
    render = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffectBlurSpark.cpp").read_text(encoding="utf-8")
    stock = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzOpenData.cpp").read_text(encoding="utf-8")
    qa = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(encoding="utf-8")
    entry = (root / "ExMain_RISE_PC/Main5.2_RISE/Winmain.cpp").read_text(encoding="utf-8")
    assert "kSpinMotionBlurBitmap = 32986" in header
    private = loader.split("bool EnsureSpinMotionBlurBitmap()", 1)[1].split("bool EnsureWrathScatterBitmaps()", 1)[0]
    assert '"Data\\\\RISE\\\\GrowLancer\\\\SpinStep\\\\motion_blur.jpg"' in private
    assert "GL_NEAREST, GL_CLAMP" in private
    assert "kSpinMotionBlurBitmap" in private and "BITMAP_BLUR + 1" not in private
    emit = effect.split("void EmitSpinStepWeaponBlur(", 1)[1].split("void CreateCarrier(", 1)[0]
    assert emit.index("EnsureSpinMotionBlurBitmap()") < emit.index("CreateObjectBlur(")
    drawing = render.split("void RenderObjectBlurs()", 1)[1].split("void RemoveObjectBlurs(", 1)[0]
    assert "b->RenderStyle == 1 && b->Type == 1" in drawing
    assert "b->Owner->Type == rise::growlancer::kSpinControllerModel" in drawing
    assert "nTexture = rise::growlancer::kSpinMotionBlurBitmap;" in drawing
    assert 'LoadBitmapFile("Effect\\\\motion_blur.jpg", BITMAP_BLUR + 1, GL_NEAREST, GL_CLAMP_TO_EDGE);' in stock
    probe = qa.split("if (spinProbe)", 1)[1].split("const char* files[]", 1)[0]
    assert "RISE_GL_SPIN_BLUR_BITMAP_QA" in qa
    for marker in ("EnsureSpinMotionBlurBitmap()", "GL_CLAMP_TO_EDGE",
                   "privateWrap == GL_CLAMP", "stockWrapAfter == GL_CLAMP_TO_EDGE",
                   "samePixels && gpuMatches", "event=spin-native-bitmap-release stable=%d"):
        assert marker in probe, marker
    assert entry.index("RunBodyTextureProbeQA()") < entry.index("WinHook(")
    print("PASS: S21 0x7F08 motion_blur NEAREST/CLAMP, original OZJ hash and private Spin style-1 native slot; SS6 shared sampler untouched")
    print("PASS: opt-in hidden WGL probe is before normal gameplay initialization and checks private/stock GPU samplers plus release")
    print("Scope: source/staging only; actual GPU result and owner ribbon pixels still OPEN")


if __name__ == "__main__":
    main()
