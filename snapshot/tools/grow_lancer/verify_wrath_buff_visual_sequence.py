"""Pin the S21 Wrath post-buff visual order against the native adapter.

This is a source/descriptor contract only.  It does not claim owner-pixel,
cadence, GPU blend, or live buff runtime parity.
"""
from pathlib import Path
import json

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
runtime = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
emission = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerWrathEmission.h").read_text(encoding="utf-8")
sprites = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerWrathSprites.h").read_text(encoding="utf-8")
particles = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp").read_text(encoding="utf-8")
resources = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp").read_text(encoding="utf-8")
docs = (ROOT / "GrowLancer/WRATH_REVERSE.md").read_text(encoding="utf-8")
descriptor = json.loads((ROOT / "GrowLancer/grow_lancer_skills_descriptor.json").read_text(encoding="utf-8"))

assert emission.count("scatter(texture,x,y,scale);") == 2
first_scatter = emission.index("scatter(texture,x,y,scale);")
second_scatter = emission.index("scatter(texture,x,y,scale);", first_scatter + 1)
assert first_scatter < second_scatter < emission.index("flares();")
assert emission.index("flares();") < emission.index("mono(variant,0);")
assert emission.count("mono(variant,") == 2
assert emission.count("mono(2,") == 2
print("PASS: S21 sequence is two scatter particles -> bone flares -> four blue mono particles")

submit = runtime[runtime.index("bool SubmitWrathPersistentVisuals"):runtime.index("bool RenderEffect")]
assert "SubmitWrathParticleSequence" in submit
assert "CreateWrathScatterParticle(variant,position,caster.Angle,light,scale);" in submit
assert "SubmitWrathPersistentSprites(caster,true);" in submit
assert "CreateWrathPersistentParticle(variant,attachment,position,caster.Angle," in submit
assert submit.index("CreateWrathScatterParticle") < submit.index("SubmitWrathPersistentSprites(caster,true)")
assert submit.index("SubmitWrathPersistentSprites(caster,true)") < submit.index("CreateWrathPersistentParticle")
assert "light = {.4f,.6f,1.f}" in submit
assert "light,.75f,&caster" in submit
assert "424" in submit and "425" in submit
print("PASS: native Wrath submit keeps the buff gate and the same emission order")

assert "kWrathScatter01Bitmap + variant" in particles
assert "kWrathMono01Bitmap, kWrathMono02Bitmap, kWrathMono03Bitmap" in particles
assert "attachment == 0 ? 29 : 38" in submit
scatter_start = particles.index("int rise::growlancer::CreateWrathScatterParticle")
scatter_end = particles.index("int rise::growlancer::CreateWrathPersistentParticle", scatter_start)
scatter = particles[scatter_start:scatter_end]
assert "kWrathScatter01Bitmap + variant" in scatter
assert "NULL, false" in scatter
persistent_start = particles.index("int rise::growlancer::CreateWrathPersistentParticle")
persistent = particles[persistent_start:]
assert "owner || !owner->Live" in persistent
assert "&caster" in submit
assert "kWrathFlare01Bitmap" in runtime[runtime.index("bool SubmitWrathPersistentSprites"):runtime.index("bool RenderWrathPersistentGround")]
print("PASS: null-owner scatter stays separate from caster-owned flare/blue mono attachments 29/38")

skill = next(row for row in descriptor["skills"] if row["id"] == 278)
tail = next(layer for layer in skill["visualLayers"]
            if layer.get("role") == "persistent_buff_post_tail")
assert tail["trigger"] == "native buff membership 424 OR 425"
assert [step["stage"] for step in tail["order"]] == [
    "player_fire_dots", "player_blue_lightning"]
assert tail["order"][0]["count"] == 2
assert tail["order"][0]["attachments"] == [29, 38]
assert tail["order"][1]["count"] == 4
assert tail["order"][1]["attachments"] == [29, 38, 29, 38]
assert tail["order"][1]["subtypeByResource"] == {
    "8086": [13, 14], "8087": [14, 15], "8089": [13, 14]}
assert tail["precedingSeparateLayer"]["owner"] is None
print("PASS: Wind Soul descriptor records the two owner fire dots before blue lightning")

for path in (
    r'Data\\RISE\\GrowLancer\\Wrath\\lighting_mega01.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\lighting_mega02.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\lighting_mega03.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\firehik_mono01.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\firehik_mono02.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\firehik_mono03.jpg',
    r'Data\\RISE\\GrowLancer\\Wrath\\flare01.jpg',
):
    assert path in resources, path
assert "two" in docs and "small fire-like marks followed by a blue lightning effect" in docs
print("PASS: private asset registration and owner visual observation are recorded")
print("OPEN: actual buff-active owner pixels, cadence, blend and cleanup still require runtime QA")
