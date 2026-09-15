"""Verify the isolated RISE adapter keeps the pinned S21 Breche particle contract.

This is a source-boundary check only.  It does not claim raster or owner
visual parity; the purpose is to catch accidental reuse of a nearby native
fire subtype when the client is rebuilt or merged.
"""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[2]
PARTICLE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffectParticle.cpp"
FIRE_HEADER = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerFireParticle.h"
SPRITE_HEADER = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerSpriteAdapter.h"


def require(text: str, pattern: str, label: str) -> None:
    if not re.search(pattern, text, flags=re.S):
        raise ValueError(f"missing Breche contract: {label}")


def main() -> None:
    particle = PARTICLE.read_text(encoding="utf-8")
    fire = FIRE_HEADER.read_text(encoding="utf-8")
    sprite = SPRITE_HEADER.read_text(encoding="utf-8")

    # Producer mapping: the three S21 IDs use the native fire textures, with
    # only 806E selecting native subtype 4.
    require(
        fire,
        r"variant 0/1/2 = S21 8084/0, 806E/4, 8085/0",
        "variant provenance comment",
    )
    require(
        particle,
        r"const int types\[\] = \{BITMAP_FIRE_HIK1, BITMAP_FIRE_CURSEDLICH, BITMAP_FIRE_HIK3\};"
        r"\s*return CreateParticleInternal\(types\[variant\].*?variant == 1 \? 4 : 0",
        "8084/806E/8085 producer mapping",
    )

    # Native constructor envelopes: life 27/12/17, scale 72..143 times the
    # supplied emitter scale, random rotation, gravity 64..87 * .1, alpha 0,
    # TurningForce=light, stored light zero.
    require(
        particle,
        r"case BITMAP_FIRE_HIK1:.*?if \(o->SubType == 0 \|\| o->SubType == 6\)\s*\{"
        r"\s*o->LifeTime = rand\(\) % 5 \+ 27;"
        r".*?o->Scale = \(float\)\(rand\(\) % 72 \+ 72\) \* 0\.01f \* Scale;"
        r".*?o->Gravity = \(float\)\(rand\(\) % 24 \+ 64\) \* 0\.1f;"
        r".*?o->Alpha = 0;.*?VectorCopy\(Light, o->TurningForce\);"
        r".*?Vector\(0, 0, 0, o->Light\);",
        "8084 subtype-0 constructor",
    )
    require(
        particle,
        r"case BITMAP_FIRE_CURSEDLICH:.*?else if \(o->SubType == 4 \|\| o->SubType == 9\)\s*\{"
        r"\s*o->LifeTime = rand\(\) % 5 \+ 12;"
        r".*?o->Scale = \(float\)\(rand\(\) % 72 \+ 72\) \* 0\.01f \* Scale;"
        r".*?o->Gravity = \(float\)\(rand\(\) % 24 \+ 64\) \* 0\.1f;"
        r".*?o->Alpha = 0;.*?VectorCopy\(Light, o->TurningForce\);"
        r".*?Vector\(0, 0, 0, o->Light\);",
        "806E subtype-4 constructor",
    )
    require(
        particle,
        r"case BITMAP_FIRE_HIK3:.*?if \(o->SubType == 0 \|\| o->SubType == 6\)\s*\{"
        r"\s*o->LifeTime = rand\(\) % 5 \+ 17;"
        r".*?o->Scale = \(float\)\(rand\(\) % 72 \+ 72\) \* 0\.01f \* Scale;"
        r".*?o->Gravity = \(float\)\(rand\(\) % 24 \+ 64\) \* 0\.1f;"
        r".*?o->Alpha = 0;.*?VectorCopy\(Light, o->TurningForce\);"
        r".*?Vector\(0, 0, 0, o->Light\);",
        "8085 subtype-0 constructor",
    )

    # The private mode is the only update route, and the source-order helper
    # performs the Live clear before the remaining light/scale/position/rot
    # operations.  This catches accidental fall-through to ordinary SS6 math.
    require(
        particle,
        r"if \(g_brecheParticleMode\[i\] != 0\)\s*\{.*?UpdateBrecheFireParticleTick\(",
        "private Breche update dispatch",
    )
    require(
        fire,
        r"const int fadeThreshold = variant == 0 \? 15 : 10;.*?"
        r"if \(p\.Alpha < \.1f\)\s*p\.Live = false;.*?"
        r"p\.Light\[component\] = p\.TurningForce\[component\] \* p\.Alpha;.*?"
        r"p\.Position\[2\] \+= p\.Gravity;\s*p\.Rotation \+= 3\.f;",
        "Breche update ordering",
    )

    # Rendering stays in the private adapter: full UV, native rotation and a
    # scoped clamp state.  No shared BITMAP_LIGHT subtype is rewritten.
    require(
        particle,
        r"if \(g_brecheParticleMode\[i\] != 0\)\s*\{\s*"
        r"rise::growlancer::RenderBrecheFireSprite\(o->TexType,",
        "private Breche render dispatch",
    )
    require(
        sprite,
        r"class ScopedBrecheClamp.*?glTexParameteri\(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP\);"
        r".*?RenderSprite\(texture, position, width, height, light, rotation\);",
        "scoped clamp and native sprite render",
    )

    # Pool reuse resets the private mode and fractional tick remainder before
    # marking the slot live.  The generic pool must remain untouched.
    require(
        particle,
        r"g_brecheParticleMode\[i\] = breche \?\s*"
        r"\(Type == BITMAP_FIRE_HIK1 \? 1 : Type == BITMAP_FIRE_CURSEDLICH \? 2 : 3\) : 0;"
        r".*?g_brecheParticleRemainder\[i\] = 0\.f;.*?o->Live = true;",
        "pool mode/remainder reset before allocation",
    )

    print("PASS: Breche 8084/806E/8085 producer, constructor, tick, render and pool-reset contract")
    print("Scope: source/static boundary only; owner pixels, target contact and exhaustion stress remain open")


if __name__ == "__main__":
    main()
