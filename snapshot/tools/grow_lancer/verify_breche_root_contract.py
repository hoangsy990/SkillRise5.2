"""Guard the evidence-scoped Breche receive-root contract.

The pinned Season 21 heap snapshot resolves runtime id 0x5FD to a model with
zero bones, meshes and actions.  RISE must therefore keep the root as a
20-tick owner/lifecycle record and emit the five proven children, without
inventing a root mesh or animation path.  This verifier checks that boundary
in source and the corresponding reverse evidence; it is not visual QA.
"""
from pathlib import Path
import hashlib


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
REVERSE = ROOT / "GrowLancer" / "BRECHE_REVERSE.md"
SNAPSHOT = ROOT / "artifacts" / "grow_lancer" / "full_dump" / (
    "s21-21072-a2dc6fe4f8124ea1a22827e774d6bb06.dmp"
)
SNAPSHOT_SHA256 = "374d1ecb06719389416938e0be27a32596e91e0a5b5d5a7c6f8df690c326efed"


def main():
    source = SOURCE.read_text(encoding="utf-8")
    reverse = REVERSE.read_text(encoding="utf-8")
    with SNAPSHOT.open("rb") as stream:
        if hashlib.file_digest(stream, "sha256").hexdigest() != SNAPSHOT_SHA256:
            raise ValueError("Pinned Breche full dump changed")
    for needle in (
        "key1533 at3B0E3FD0",
        "Signed counts at+22/+24/+26 are0 bones/0 meshes/0 actions",
        "Do not stage a\nroot BMD or render invented root geometry",
    ):
        if needle not in reverse:
            raise ValueError(f"Missing root evidence: {needle!r}")

    init = source[source.index("void InitializeEffect"):source.index("void UpdateEffect")]
    update = source[source.index("void UpdateEffect"):source.index("bool RenderEffect")]
    render = source[source.index("bool RenderEffect"):source.index("bool RenderBrecheClippedMesh")]
    expected_children = (
        "CreateEffect(kBrecheLightMarksModel",
        "CreateEffect(kBrecheTwilight02Model",
        "CreateEffect(kBrecheTwilight01Model",
        "CreateEffect(kBrecheEmitterModel",
    )
    if init.count("CreateEffect(kBrecheTwilight01Model") != 2:
        raise ValueError("Breche root must create both 0x809E children")
    if any(token not in init for token in expected_children):
        raise ValueError("Breche root child list changed")
    if "effect.LifeTime = 20.f;" not in init:
        raise ValueError("Breche root lifetime changed")
    root_update = update[update.index("case kBrecheControllerModel:"):update.index("case kBrecheLightMarksModel:")]
    if "break;" not in root_update or "Empty cached S21 model" not in root_update:
        raise ValueError("Breche root update must remain lifecycle-only")
    root_guard_start = render.index("if (effect.Type == kBrecheControllerModel || effect.Type == kBrecheEmitterModel)")
    root_guard_end = render.index("vec3_t light;", root_guard_start)
    root_render = render[root_guard_start:root_guard_end]
    if "effect.Type == kBrecheControllerModel || effect.Type == kBrecheEmitterModel" not in root_render:
        raise ValueError("Breche empty root/emitter render guard changed")
    if "RenderMesh" in root_render or "RenderBrecheClippedMesh" in root_render:
        raise ValueError("Breche root render must not invent geometry")
    print("PASS: Breche 0x5FD root is lifecycle-only; five children remain the visual chain")
    print("Scope: pinned heap/source contract only; child pixels, contact and runtime parity remain open")


if __name__ == "__main__":
    main()
