"""Verify the scoped native render contract for Breche receive layers.

This is a source/static guard.  It does not claim a real GL context or owner
pixel parity; it prevents the recovered S21 ONE/ONE terrain pass from being
silently rendered with whatever blend state a neighboring effect left behind.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
ADAPTER = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerTerrainAdapter.h"


def main() -> None:
    runtime = RUNTIME.read_text(encoding="utf-8")
    adapter = ADAPTER.read_text(encoding="utf-8")
    match = re.search(
        r"if \(IsBrecheEffectType\(effect\.Type\)\).*?"
        r"if \(effect\.Type == kBrecheControllerModel \|\|"
        r" effect\.Type == kBrecheEmitterModel\).*?"
        r"const bool marks = effect\.Type == kBrecheLightMarksModel \|\|\s*"
        r"effect\.Type == kBrecheOwnerLightMarksModel;"
        r"(?P<body>.*?)return true;\n    \}",
        runtime,
        flags=re.S,
    )
    if not match:
        raise ValueError("Breche RenderEffect body not found")
    body = match.group("body")
    required = (
        "EnableAlphaBlend();",
        "RenderGrowLancerTerrainBitmap(bitmap, effect.Position[0], effect.Position[1],",
        "marks ? 1.f : effect.Alpha, 5.f, !marks);",
        "DisableAlphaBlend();",
    )
    for needle in required:
        if needle not in body:
            raise ValueError(f"missing scoped Breche terrain render contract: {needle}")
    if "VectorScale(effect.Light, effect.Alpha, light);" not in match.group(0):
        raise ValueError("Breche terrain light must be multiplied by stored alpha once")
    if body.index("EnableAlphaBlend();") > body.index("RenderGrowLancerTerrainBitmap"):
        raise ValueError("Breche blend enable must precede terrain submission")
    if body.index("DisableAlphaBlend();") < body.index("RenderGrowLancerTerrainBitmap"):
        raise ValueError("Breche blend restore must follow terrain submission")

    if "if (!flipV)" not in adapter or "ScopedTerrainFlipV scope;" not in adapter:
        raise ValueError("private terrain V-flip adapter missing")
    if "RenderTerrainAlphaBitmap(texture, x, y, sizeX, sizeY, light," not in adapter:
        raise ValueError("adapter no longer delegates to native terrain renderer")

    print("PASS: Breche ground layers use scoped native ONE/ONE blend, terrain adapter and per-layer V flip")
    print("Scope: source/static boundary only; real GL pixels, culling and owner contact remain open")


if __name__ == "__main__":
    main()
