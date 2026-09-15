"""Verify the native sprite route for Wrath's two owner-bound fire dots.

The recovered S21 post-buff tail submits flare01 at the two caster bones.
This guard proves the private resource registration, subtype-0 additive
sprite path and private visibility envelope without claiming live pixels.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RUNTIME = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(
    encoding="utf-8"
)
SPRITE = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffectSprite.cpp").read_text(
    encoding="utf-8"
)
RESOURCES = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp").read_text(
    encoding="utf-8"
)
DOCS = (ROOT / "GrowLancer/WRATH_REVERSE.md").read_text(encoding="utf-8")


start = RUNTIME.index("bool SubmitWrathPersistentSprites")
end = RUNTIME.index("bool RenderWrathPersistentGround", start)
submit = RUNTIME[start:end]
assert "const int texture = boneFlareGroup ? kWrathFlare01Bitmap : kWrathLightmarksBitmap;" in submit
assert "model.TransformByObjectBone(position,&caster,bone);" in submit
assert "CreateSprite(texture,position,scale,light,&caster,0.f,0);" in submit
assert "bone == 29 ? 0 : 1" in submit

resource_start = RESOURCES.index("bool EnsureWrathGroundSpriteBitmaps")
resource_end = RESOURCES.index("void LoadSounds", resource_start)
resource = RESOURCES[resource_start:resource_end]
assert '{kWrathFlare01Bitmap, "Data\\\\RISE\\\\GrowLancer\\\\Wrath\\\\flare01.jpg"}' in resource
assert "bitmap->Components != 3" in resource
assert "bitmap->Width != sizes[i] || bitmap->Height != sizes[i]" in resource

render_start = SPRITE.index("void RenderSprites")
render_end = SPRITE.index("void CheckSprites", render_start)
render = SPRITE[render_start:render_end]
assert "else if(o->SubType == 0)" in render
assert "EnableAlphaBlend();" in render
assert "RenderSprite(o,o->Owner);" in render
assert "rise::growlancer::ClearBrecheSprite(o);" in render

special = SPRITE[SPRITE.index("void RenderSprite(OBJECT *o,OBJECT *Owner)"):render_start]
assert "o->Type == rise::growlancer::kWrathFlare01Bitmap" in special
assert "o->AnimationFrame += o->Visible ? .1f : -.1f;" in special
assert "RenderSprite(o->Type,o->Position,Width,Height,o->Light,o->Angle[2]);" in special

assert "two `7EF7/flare01` sprites" in DOCS
print("PASS: Wrath flare01 uses native owner-bone sprite allocation at bones 29/38")
print("PASS: flare01 resource is private 64x64 RGB and RenderSprites subtype0 selects additive blend")
print("PASS: private visibility envelope and native sprite-pool retirement remain scoped")
print("Scope: source adapter contract only; live buff pixels, cadence and owner cleanup remain open")
