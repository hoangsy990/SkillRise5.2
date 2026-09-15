"""Guard Breche effect retirement against owner-recursive or particle cleanup.

Season 21 resets the five receive-root effect records through the native
effect-pool destructor.  The root/children are model-effect records; the
fire variants emitted by 0x806E are PARTICLE records and must use their own
Live/pool path.  This verifier checks that the RISE adapter keeps those two
lifecycles separate and that a dead Breche effect reaches the shared native
destructor boundary.
"""
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
EFFECT_RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
EFFECT = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffect.cpp"
PARTICLE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffectParticle.cpp"
REVERSE = ROOT / "GrowLancer" / "BRECHE_CHILDREN_REVERSE.md"


def section(text: str, begin: str, end: str) -> str:
    start = text.index(begin)
    stop = text.index(end, start)
    return text[start:stop]


def main():
    runtime = EFFECT_RUNTIME.read_text(encoding="utf-8")
    effect = EFFECT.read_text(encoding="utf-8")
    particle = PARTICLE.read_text(encoding="utf-8")
    reverse = REVERSE.read_text(encoding="utf-8")

    update = section(runtime, "void UpdateEffect", "bool RenderEffect")
    breche = section(update, "case kBrecheControllerModel:", "case kWrathBrokenBitmap:")
    emitter = section(update, "case kBrecheEmitterModel:", "case kWrathBrokenBitmap:")
    if "EffectDestructor" in breche or "DeleteEffect" in breche:
        raise ValueError("Breche root update must not recursively destroy children")
    if "effect.Live = false" not in emitter or "if (!effect.Owner)" not in emitter:
        raise ValueError("Breche emitter null-owner retirement guard changed")

    move = section(effect, "void MoveEffect(OBJECT* o, int iIndex)", "void MoveEffects()")
    if "rise::growlancer::IsBrecheEffectType(o->Type)" not in move:
        raise ValueError("MoveEffect no longer recognizes Breche lifecycle")
    if "rise::growlancer::AdvanceWholeTicks(*o, FPS_ANIMATION_FACTOR," not in move:
        raise ValueError("Breche model effects no longer use the native whole-tick scheduler")
    if "EffectDestructor(o);" not in move:
        raise ValueError("MoveEffect no longer reaches shared effect retirement")

    destructor = section(effect, "void EffectDestructor(OBJECT* o)", "void TerminateOwnerEffectObject")
    if "rise::growlancer::ResetEffectAnimationSamples(o);" not in destructor:
        raise ValueError("Effect destructor lost animation-sample reset")
    if "o->Live = false;" not in destructor or "o->Owner = NULL;" not in destructor:
        raise ValueError("Effect destructor lost native Live/Owner clearing")
    if "EffectDestructor(o);" not in section(effect, "bool DeleteEffect(int Type, OBJECT* Owner", "void DeleteEffect(int efftype)"):
        raise ValueError("owner-scoped effect deletion no longer uses shared destructor")

    particle_move = section(particle, "void MoveParticles()", "void RenderParticles")
    breche_particle = section(particle_move, "if (g_brecheParticleMode[i] != 0)", "continue;",)
    if "AdvanceParticleWholeTicks" not in breche_particle:
        raise ValueError("Breche fire particles lost private whole-tick scheduling")
    if "UpdateBrecheFireParticleTick" not in breche_particle:
        raise ValueError("Breche fire particle update callback changed")
    if "EffectDestructor" in breche_particle:
        raise ValueError("Breche PARTICLE path must not invoke EffectDestructor")
    if "g_brecheParticleMode[i] = breche ?" not in particle:
        raise ValueError("Breche particle provenance marker is not reset at allocation")
    if "g_brecheParticleRemainder[i] = 0.f;" not in particle:
        raise ValueError("Breche particle remainder is not reset at slot reuse")

    for needle in (
        "This supports native effect-pool reset ownership",
        "not pass PARTICLE to the effect destructor.",
        "render-pass ownership, not emitter lifetime20",
    ):
        if needle not in reverse:
            raise ValueError(f"Missing retirement evidence: {needle!r}")

    print("PASS: Breche model effects and fire particles retain separate native retirement paths")
    print("Scope: source/evidence contract only; pool exhaustion and owner visual parity remain open")


if __name__ == "__main__":
    main()
