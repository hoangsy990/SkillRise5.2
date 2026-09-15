"""Static boundary for the Circle post-buff allocator probe, not pixel PASS."""
from pathlib import Path

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
SOURCE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE"


def require(needle: str, haystack: str, label: str) -> None:
    if needle not in haystack:
        raise AssertionError(label)


def main() -> None:
    runtime = (SOURCE / "RISE" / "GrowLancerEffectRuntime.cpp").read_text(
        encoding="utf-8", errors="replace"
    )
    qa = (SOURCE / "RISE" / "GrowLancerRuntimeQA.cpp").read_text(
        encoding="utf-8", errors="replace"
    )
    particle = (SOURCE / "ZzzEffectParticle.cpp").read_text(
        encoding="utf-8", errors="replace"
    )
    block = runtime.split("bool SubmitCirclePersistentVisuals(OBJECT& caster)", 1)[1].split(
        "bool SubmitWrathPersistentSprites", 1
    )[0]
    assert block.count("CreateCircleUpperArmParticle(position, caster.Angle,") == 2
    require("for (int i = 0; i < 2; ++i)", block, "paired mono loops")
    assert block.count("for (int i = 0; i < 2; ++i)") == 2
    require("monoCreated += CreateCircleUpperArmParticle", block, "native result accounting")
    require("#ifdef RISE_GROW_LANCER_RUNTIME_QA\n    RecordCirclePersistentEmissionQA", block,
            "post-buff QA-only emission record")
    require("gControllerSkill != 272", qa, "owner skill guard")
    require("&actor != &Hero->Object", qa, "owner pointer guard")
    require("gCircleEmissionCount >= 64", qa, "bounded emission records")
    require("gCircleEmissionCount = 0;", qa, "F7 emission reset")
    require("monoExpected=4", qa, "source four-emission contract")
    require("EnsureCirclePersistentBitmap()", particle, "native resource check")
    require("kCircleMonoSubType, scale, NULL, false", particle,
            "S21 null-owner subtype without budget bypass")
    print("PASS: Circle four post-buff native particle calls; bounded owner-only QA allocator outcomes, no SS6 budget bypass")
    print("Scope: static and build only; paired shoulder pixels/buff receipt/pool runtime OPEN")


if __name__ == "__main__":
    main()
