"""Guard the scoped native Wrath ground draw and its GL-state restoration."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
runtime = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
loop = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp").read_text(encoding="utf-8")
ground = runtime.split("if (effect.Type == kWrathBrokenBitmap)", 1)[1].split(
    "if (effect.Type == kSpriteCarrierModel)", 1
)[0]
assert ground.count("EnableAlphaBlend();") == 1
assert ground.count("RenderTerrainAlphaBitmap(") == 1
assert ground.count("DisableAlphaBlend();") == 1
assert ground.index("EnableAlphaBlend();") < ground.index("RenderTerrainAlphaBitmap(")
assert ground.index("RenderTerrainAlphaBitmap(") < ground.index("RecordWrathGroundQA(effect);")
assert ground.index("RecordWrathGroundQA(effect);") < ground.index("DisableAlphaBlend();")
assert ground.index("DisableAlphaBlend();") < ground.index("return true;")
effect_loop = loop.split("void RenderEffects(bool bRenderBlendMesh)", 1)[1].split(
    "void RenderAfterEffects(", 1
)[0]
assert "rise::growlancer::RenderEffect(*o);" in effect_loop
assert not effect_loop.rstrip().endswith("DisableAlphaBlend();")
print("PASS: Wrath S21 ONE/ONE terrain draw keeps its QA state capture then restores the native 5.2 blend state before return")
print("Scope: static render-state contract only; black-square owner pixels and S21 frame parity remain OPEN")
