#!/usr/bin/env python3
"""Guard the isolated RISE owner/target split for Breche.

This is a source/static contract.  It ensures the owner producer added from
the S21 dump cannot regress into replaying the target-contact root on the
caster; it does not claim runtime pixel parity or server authority.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
RESOURCES = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerResources.cpp"
PROVENANCE = ROOT / "GrowLancer" / "asset_provenance.json"


def require(text: str, needle: str, label: str) -> None:
    if needle not in text:
        raise ValueError(f"missing Breche owner adapter contract: {label}")


def main() -> None:
    source = RUNTIME.read_text(encoding="utf-8")
    resources = RESOURCES.read_text(encoding="utf-8")
    provenance = PROVENANCE.read_text(encoding="utf-8")

    action = source[source.index("void CreateBrecheAction"):source.index("void CreateBrecheHit")]
    hit = source[source.index("void CreateBrecheHit"):source.index("}}", source.index("void CreateBrecheHit"))]
    require(action, "CreateEffect(kBrecheControllerModel, caster.Position, caster.Angle", "caster controller allocation")
    require(action, "caster.Light, 0, &caster, -1, kBrecheSkill", "caster owner/subtype-zero fields")
    require(hit, "target.Position, target.Angle", "target contact position")
    require(hit, "target.Light, 1, &target", "target owner/subtype-one fields")
    if "CreateBrecheHit(caster" in action or "CreateBrecheHit(*caster" in action:
        raise ValueError("owner action must not replay target contact on caster")

    owner = source[source.index("void UpdateBrecheOwnerTick"):source.index("void UpdateEffect")]
    for needle, label in (
        ("SpawnBrecheOwnerParticles(effect);", "owner particle wrappers"),
        ("UpdateBrecheOwnerControllerPosition(effect);", "owner -200 controller offset"),
        ("SpawnBrecheOwnerPinLights(effect);", "owner pin-light joints"),
        ("CreateBrecheOwnerTimedLayers(effect);", "owner timed layers"),
    ):
        require(owner, needle, label)

    particles = source[source.index("void SpawnBrecheOwnerParticles"):
                       source.index("void CreateBrecheOwnerTimedLayers")]
    require(particles, "Vector(.4f, .4f, .4f, light)",
            "S21 owner particle light vector")
    require(particles, "rand() % 2 + 1", "S21 owner particle integer scale")
    if "position[0] +=" in particles or "position[1] +=" in particles or "position[2] +=" in particles:
        raise ValueError("owner particles must keep the direct controller position")

    pin_lights = source[source.index("void SpawnBrecheOwnerPinLights"):
                        source.index("void SpawnBrecheOwnerParticles")]
    require(pin_lights, "rand() % 61 - 30", "S21 pin-light X angle range")
    require(pin_lights, "rand() % 21 + 70", "S21 pin-light Z angle range")

    owner_position = source[source.index("void UpdateBrecheOwnerControllerPosition"):
                           source.index("void SpawnBrecheOwnerPinLights")]
    for needle, label in (
        ("Vector(0.f, -200.f, 0.f, localOffset)", "S21 local owner offset"),
        ("AngleMatrix(effect.Angle, matrix)", "controller-angle rotation"),
        ("VectorRotate(localOffset, matrix, rotatedOffset)", "native offset rotation"),
        ("VectorAdd(effect.Owner->Position, rotatedOffset, effect.Position)",
         "owner snapshot plus rotated offset"),
    ):
        require(owner_position, needle, label)

    timed = source[source.index("void CreateBrecheOwnerTimedLayers"):
                   source.index("void UpdateBrecheOwnerTick")]
    for needle, label in (
        ("vec3_t ownerPosition;", "owner snapshot vector"),
        ("VectorCopy(owner.Position, ownerPosition);", "owner snapshot copy"),
        ("CreateEffect(kBrecheOwnerRingModel, ownerPosition, ownerAngle", "caster ring position"),
        ("CreateEffect(kBrecheOwnerLightMarksModel, ownerPosition, ownerAngle", "caster mark position"),
        ("CreateEffect(kBrecheOwnerFireRingModel, ownerPosition, ownerAngle", "snapshot fire-ring position"),
        ("CreateEffect(kBrecheOwnerTwilight02Model, ownerPosition, ownerAngle", "snapshot twilight-02 position"),
        ("CreateEffect(kBrecheOwnerTwilight01Model, ownerPosition, ownerAngle", "snapshot twilight-01 position"),
        ("if (effect.LifeTime == 7.f)", "one-shot wind timing gate"),
        ("windPosition[1] -= 5.f;", "S21 wind local Y correction"),
        ("Vector(3.f, .6f, .6f, windLight);", "S21 AD9 initial light"),
        ("CreateEffect(kBrecheOwnerWindModel, windPosition, ownerAngle", "owner wind model"),
    ):
        require(timed, needle, label)
    if "CreateEffect(kBrecheOwnerWindModel" in owner:
        raise ValueError("owner wind must be emitted only by the remaining-life-7 timed block")

    render = source[source.index("bool RenderEffect"):source.index("bool RenderBrecheClippedMesh")]
    require(render, "if (effect.Type == kBrecheOwnerWindModel)", "owner wind render branch")
    require(render, "model.RenderBody(RENDER_TEXTURE, effect.Alpha, effect.BlendMesh,",
            "S21 AD9 body renderer flag")
    require(render, "effect.BlendMeshLight, effect.BlendMeshTexCoordU,",
            "S21 generic AD9 blend tuple")
    if "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT" in render[
            render.index("if (effect.Type == kBrecheOwnerWindModel)"):
            render.index("if (effect.Type == kBrecheControllerModel")]:
        raise ValueError("Breche owner wind must not use the old bright mesh-zero renderer")
    require(render, "without inventing a RENDER_BRIGHT flag",
            "documented S21 blend-mesh material mapping")
    require(render, "if (effect.Type == kBrecheControllerModel || effect.Type == kBrecheEmitterModel)\n            return true;", "empty controller/emitter render suppression")
    require(render, "owner ? \"breche-owner-submit\" : \"breche-submit\"", "owner/target QA labels")

    update = source[source.index("void UpdateEffect"):source.index("bool RenderWrathPersistentGround")]
    for needle, label in (
        ("effect.Scale /= 1.1f;", "80BC/subtype1 scale contraction"),
        ("effect.Light[0] /= 1.1f;", "80BC/subtype1 light contraction"),
        ("effect.Angle[2] += 15.f;", "81EB/subtype0 rotation"),
        ("effect.Alpha += effect.LifeTime > 6.f ? (1.f / 6.f) : -(1.f / 6.f);",
         "81EB/subtype0 twelve-tick alpha envelope"),
        ("effect.Angle[2] -= 50.f;", "AD9/subtype6 rotation"),
        ("effect.Alpha -= .01f;", "AD9/subtype6 fade"),
        ("effect.Light[0] *= effect.Alpha;", "AD9/subtype6 RGB fade"),
    ):
        require(update, needle, label)

    constructor = source[source.index("void InitializeEffect"):
                         source.index("void CreateBrecheAction")]
    require(constructor, "effect.Type == kBrecheOwnerWindModel ? .9f",
            "S21 AD9 constructor alpha")
    require(constructor, "if (effect.Type == kBrecheOwnerWindModel)\n            effect.BlendMesh = 0;",
            "S21 AD9 blend-mesh selector")
    require(constructor,
            "effect.Type == kBrecheOwnerFireRingModel ? 12.f : 20.f",
            "S21 81EB subtype0 lifetime")
    require(constructor,
            "effect.Type == kBrecheOwnerFireRingModel ? 0.f : 1.f",
            "S21 81EB subtype0 initial alpha")

    qa = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(encoding="utf-8")
    cast = qa[qa.index("void CastCurrentSkill"):qa.index("void EmitCurrentContact")]
    facing = "caster.Angle[2] = CreateAngle2D(caster.Position, target->Position);"
    require(cast, facing, "F7 ReceiveMagic-equivalent target facing")
    if cast.index(facing) > cast.index("CreateBrecheAction(caster);"):
        raise ValueError("Breche QA facing must be established before owner-controller creation")
    require(cast, 'WriteQALog("breche-caster-action-local-only", 279, SelectedCharacter);',
            "F7 local-only action evidence")
    for forbidden in (
        "Hero->Skill = static_cast<WORD>(279);",
        "Hero->TargetCharacter = static_cast<short>(SelectedCharacter);",
        "Hero->SkillSuccess = true;",
        "Hero->AttackFlag = ATTACK_FAIL;",
        "Hero->AttackTime = 1;",
    ):
        if forbidden in cast:
            raise ValueError("Breche visual-only F7 mutates server-authoritative attack state: " + forbidden)

    require(resources, 'kBrecheOwnerWindModel, "Data\\\\RISE\\\\GrowLancer\\\\Breche\\\\Owner\\\\", "wind_foce.bmd"', "private owner BMD registration")
    require(provenance, '"riseRuntimeId":"kBrecheOwnerWindModel"', "symbolic owner BMD provenance")
    require(provenance, '"role":"owner_lightmarks_ground"', "owner lightmarks provenance")
    require(provenance, '"role":"owner_twilight01_ground"', "owner twilight provenance")

    print("PASS: Breche owner adapter keeps caster presentation separate from target contact")
    print("PASS: owner particle/joint/timed/BMD paths and private provenance are wired")
    print("PASS: owner visible ground layers use caster position and wind is a one-shot life-7 emission")
    print("PASS: owner wind uses the S21 generic RenderBody path; ring/fire/wind tick motion and QA target facing are wired")
    print("PASS: visual-only F7 does not arm server-authoritative CHARACTER attack fields")
    print("Scope: source/static boundary only; 5.2 owner pixels, pool stress and server remain open")


if __name__ == "__main__":
    main()
