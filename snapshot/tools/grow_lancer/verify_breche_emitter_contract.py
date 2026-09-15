#!/usr/bin/env python3
"""Verify the private Breche emitter uses native owner-bone/resource boundaries.

This is a source/static check only. It does not claim that a live target has a
compatible skeleton, that particles are visible, or that pool pressure matches
Season 21.
"""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"


def require(text: str, pattern: str, label: str) -> None:
    if not re.search(pattern, text, flags=re.S):
        raise ValueError(f"missing Breche emitter contract: {label}")


def main() -> None:
    source = RUNTIME.read_text(encoding="utf-8")

    # The emitter is a lifecycle-only OBJECT: its update creates the five
    # recovered child resource families, while RenderEffect does not submit a
    # second mesh/terrain layer for the controller or emitter itself.
    require(
        source,
        r"case kBrecheEmitterModel:.*?if \(!effect\.Owner\)\s*\{\s*"
        r"effect\.Live = false;.*?\}\s*OBJECT& owner = \*effect\.Owner;"
        r".*?if \(Models \&\&?\s*owner\.Type < 0|Models \|\| owner\.Type < 0",
        "owner guard before emitter work",
    )
    require(
        source,
        r"case kBrecheEmitterModel:.*?if \(model\.NumBones <= 0 \|\|"
        r" model\.NumBones > MAX_BONES\)\s*break;.*?"
        r"for \(int emission = 0; emission < 2; \+\+emission\).*?"
        r"model\.TransformByObjectBone\(position, &owner, rand\(\) % model\.NumBones\);"
        r".*?CreateBrecheSprite\(BITMAP_LIGHT, position, 4\.f, light, &owner, 0\.f\);"
        r".*?CreateBrecheFireParticle\(variant, position, effect\.Angle, light, scale\);",
        "bounded native bone transform and child producers",
    )
    require(
        source,
        r"if \(IsBrecheEffectType\(effect\.Type\)\)\s*\{.*?"
        r"if \(effect\.Type == kBrecheControllerModel \|\|"
        r" effect\.Type == kBrecheEmitterModel\)\s*return true;",
        "controller/emitter render suppression",
    )

    print("PASS: Breche emitter owner/bone guard, native child producers and render suppression")
    print("Scope: source/static boundary only; target skeleton, particle pixels and pool QA remain open")


if __name__ == "__main__":
    main()
