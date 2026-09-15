"""Verify the native RISE adapter covers all three S21 Wrath mono variants.

This is a source contract only.  It proves the selected 8086/8087/8089
branches are routed through native particle ownership, bone updates and the
ordinary native sprite renderer; it does not claim GPU pixels or live buff
parity.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
PARTICLE = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp").read_text(
    encoding="utf-8"
)
PERSISTENT = (
    ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerWrathPersistentTick.h"
).read_text(encoding="utf-8")

mono_types = (
    "kWrathMono01Bitmap",
    "kWrathMono02Bitmap",
    "kWrathMono03Bitmap",
)

# Allocation keeps the source subtype contract: 13/14 for mono01/03 and
# 14/15 for mono02, with the live owner pointer retained in PARTICLE::Target.
create = PARTICLE[PARTICLE.index("int rise::growlancer::CreateWrathPersistentParticle"):
                  PARTICLE.index("static int CreateParticleInternal",
                                 PARTICLE.index("int rise::growlancer::CreateWrathPersistentParticle"))]
for needle in (
    "attachment < 0 || attachment > 1",
    "!owner || !owner->Live",
    "const int types[] = {kWrathMono01Bitmap, kWrathMono02Bitmap, kWrathMono03Bitmap};",
    "(variant == 1 ? 14 : 13) + attachment",
    "owner, false",
):
    assert needle in create, needle

# MoveParticles maps every allocated mono type to one bounded whole-tick path,
# validates the borrowed owner before bone lookup, and selects source bone29/38
# from the private subtype pair.
move_start = PARTICLE.index("if (o->Type >= rise::growlancer::kWrathMono01Bitmap")
move_end = PARTICLE.index("if (o->Type == BITMAP_ENERGY", move_start)
move = PARTICLE[move_start:move_end]
for needle in (
    "AdvanceParticleWholeTicks(*o",
    "OBJECT* owner = p.Target;",
    "!owner || !owner->Live || !Models || owner->Type < 0",
    "const int baseSubtype = p.Type == rise::growlancer::kWrathMono02Bitmap ? 14 : 13;",
    "const int attachment = p.SubType - baseSubtype;",
    "const int bone = attachment == 0 ? 29 : 38;",
    "model.TransformByObjectBone(position, owner, bone);",
    "UpdateWrathMono01Tick",
    "UpdateWrathMono02Tick",
    "UpdateWrathMono03Tick",
):
    assert needle in move, needle

# RenderParticles keeps all three mono resources on the native full-UV sprite
# path. Components==3 establishes the same additive alpha-blend class as the
# successful S21 JPEG registrations; no alternate fake renderer is introduced.
render_scope = PARTICLE.index("void RenderParticles")
render_start = PARTICLE.index("case rise::growlancer::kWrathScatter01Bitmap:", render_scope)
render_end = PARTICLE.index("case rise::growlancer::kWrathComboBitmap:", render_start)
render = PARTICLE[render_start:render_end]
for needle in mono_types + ("RenderSprite(o->TexType, o->Position, Width, Height,",):
    assert needle in render, needle
assert "if (pBitmap->Components == 3)" in PARTICLE

# Owner retirement clears only the three mono types and unlinks Target; the
# ordinary native manager remains responsible for pool reuse/destruction.
retire = PARTICLE[PARTICLE.index("void rise::growlancer::RetireWrathPersistentParticles"):
                  PARTICLE.index("int rise::growlancer::CreateWrathScatterParticle")]
assert "p.Target == owner" in retire
assert "p.Type >= kWrathMono01Bitmap && p.Type <= kWrathMono03Bitmap" in retire
assert "p.Live = false" in retire and "p.Target = NULL" in retire

# The three selected update bodies remain explicit adapters, not a guessed
# shared alpha/position implementation.
for needle in (
    "UpdateWrathMono01Tick",
    "UpdateWrathMono02Tick",
    "UpdateWrathMono03Tick",
    "ownerLive",
    "offset[2] += p.Gravity * 10.f",
    "p.Alpha -= 0.0666666030883789f",
):
    assert needle in PERSISTENT, needle

print("PASS: 8086/8087/8089 allocation, owner-bone update and native RenderSprite routing")
print("PASS: owner retirement unlinks all three mono variants without changing ordinary pool policy")
print("Scope: source adapter contract only; GPU pixels, cadence and live buff cleanup remain open")
