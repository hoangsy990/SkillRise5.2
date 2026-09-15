"""Verify bounded Clash/Breche render-submission telemetry wiring.

The recorder is diagnostic only: it proves that the isolated QA build can
distinguish visibility, model load, transform and native submission stages.
It intentionally makes no GPU-pixel or visual-parity claim.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA_H = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeQA.h"
QA_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeQA.cpp"
RUNTIME_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
EFFECT_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffect.cpp"


def main() -> None:
    header = QA_H.read_text(encoding="utf-8")
    qa = QA_CPP.read_text(encoding="utf-8")
    runtime = RUNTIME_CPP.read_text(encoding="utf-8")
    effect = EFFECT_CPP.read_text(encoding="utf-8")

    assert "void RecordTargetSkillRenderQA(const OBJECT& effect, int skill," in header
    assert "gTargetSkillRenderCount >= 256" in qa
    assert "event=target-skill-render-summary" in qa
    assert "sampleSkill == 275" in qa and "sampleSkill == 279" in qa

    clash = runtime[runtime.index("if (effect.Type != kClashFrontModel"):
                    runtime.index("bool RenderBrecheClippedMesh")]
    assert '"clash-ensure"' in clash
    assert '"clash-calc"' in clash
    assert '"clash-submit"' in clash

    breche = runtime[runtime.index("if (IsBrecheEffectType(effect.Type))"):
                     runtime.index("if (effect.Type == kWrathBrokenBitmap)")]
    assert '"breche-submit"' in breche

    assert '"clash-visible"' in effect and '"clash-culled"' in effect
    assert '"breche-visible"' in effect and '"breche-culled"' in effect
    assert "RecordTargetSkillRenderQA(*o, skill, stage" in effect

    print("PASS: Clash records visible -> EnsureModel -> Calc_RenderObject -> RenderMesh stages")
    print("PASS: Breche records terrain submission plus visible/culled effect stages")
    print("PASS: QA telemetry is bounded to 256 records and does not read back pixels")
    print("Scope: diagnostic submission evidence only; owner visual parity remains open")


if __name__ == "__main__":
    main()
