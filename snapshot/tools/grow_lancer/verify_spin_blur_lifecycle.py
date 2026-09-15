"""Focused native Spin Step ribbon ownership/draw-path source guard.

This does not establish visible pixels, S21 parity or a clean map transition.
"""
from pathlib import Path

root = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
main = root / "ExMain_RISE_PC/Main5.2_RISE"
effect = (main / "ZzzEffect.cpp").read_text(errors="replace")
blur = (main / "ZzzEffectBlurSpark.cpp").read_text(errors="replace")
runtime = (main / "RISE/GrowLancerEffectRuntime.cpp").read_text(errors="replace")
qa = (main / "RISE/GrowLancerRuntimeQA.cpp").read_text(errors="replace")
scene = (main / "ZzzScene.cpp").read_text(errors="replace")
map_manager = (main / "MapManager.cpp").read_text(errors="replace")

destructor = effect.split("void EffectDestructor(OBJECT* o)", 1)[1].split(
    "void DeleteEffect", 1)[0]
spin = destructor.split("case rise::growlancer::kSpinControllerModel:", 1)[1].split(
    "break;", 1)[0]
assert "if (o->SubType == 0)" in spin and "RemoveObjectBlurs(o, 0);" in spin
assert "ClearAllObjectBlurs();" in scene
teardown = map_manager.split("void CMapManager::DeleteObjects()", 1)[1].split(
    "bool CMapManager::InChaosCastle", 1)[0]
assert teardown.index("Effects[i].Type == rise::growlancer::kSpinControllerModel") < teardown.index(
    "RemoveObjectBlurs(&Effects[i], 0);") < teardown.index("Effects[i].Live = false;")
emitter = runtime.split("void EmitSpinStepWeaponBlur", 1)[1].split(
    "void CreateSpinCross", 1)[0]
assert "CreateObjectBlur(&effect, start, end, light, 1, false, 0, -1, 1);" in emitter
renderer = blur.split("void RenderObjectBlurs()", 1)[1].split(
    "void RemoveObjectBlurs", 1)[0]
assert renderer.index("if(b->Number >= 2)") < renderer.index("glBegin(GL_TRIANGLE_FAN)")
assert renderer.index("glEnd();") < renderer.index("RecordSpinBlurDrawQA")
assert "b->RenderStyle == 1" in renderer
assert "b->Owner->Type == rise::growlancer::kSpinControllerModel" in renderer
assert "gSpinBlurDrawCount >= 128" in qa
assert "Bitmaps.FindTexture(textureId) != NULL" in qa
assert "CountStyleOneObjectBlursQA()" in qa and "event=map-style1-blurs" in qa
assert "ObjectBlur[i].RenderStyle == 1" in blur and "ObjectBlur[i].SubType == 0" in blur
print("PASS: Spin Step controller-owned style-1 blur retires on expiry and map teardown before slot reuse; QA observes native draw pass")
print("Scope: source ordering only; actual draw records and owner visual parity remain OPEN")
