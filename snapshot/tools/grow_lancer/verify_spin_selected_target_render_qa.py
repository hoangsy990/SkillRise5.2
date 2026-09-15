"""Verify one selected-target Spin Step native submission sequence.

The source guards and the existing isolated QA log must show a resolved target,
the one-shot gate, and successful cross/ground native submissions.  This is
not a visual-acceptance test: it deliberately does not inspect framebuffer
pixels or infer parity from node counts.
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
QA = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeQA.cpp"
LOG = ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "RuntimeQA" / "Client" / "GrowLancerRuntimeQA.log"

CAST_RE = re.compile(r"event=cast-input phase=before-dispatch skill=271\b")
OUTCOME_RE = re.compile(r"event=spin-outcome outcome=1 .*?target=(-?\d+)")
RENDER_RE = re.compile(
    r"event=spin-render stage=([^ ]+) result=(-?\d+) type=(\d+) "
    r"subtype=(\d+) .*?target=(-?\d+)")


def main() -> None:
    runtime = RUNTIME.read_text(encoding="utf-8")
    qa = QA.read_text(encoding="utf-8")

    assert "ResolveTarget(effect.m_sTargetIndex, target)" in runtime
    assert "EmitSpinCastFanout(*target)" in runtime
    assert 'RecordSpinRenderQA(effect, "cross-submit"' in runtime
    assert 'RecordSpinRenderQA(effect, "ground-submit"' in runtime
    assert 'RecordSpinRenderQA(effect, "weapon-blur-submit"' in runtime
    assert "if (gControllerSkill != 271" in qa
    assert "gSpinRenderCount >= 128" in qa

    if not LOG.is_file():
        raise SystemExit(f"log missing: {LOG}")
    lines = LOG.read_text(encoding="utf-8", errors="replace").splitlines()
    casts: list[list[str]] = []
    for line in lines:
        if CAST_RE.search(line):
            casts.append([])
        if casts:
            casts[-1].append(line)

    selected = []
    for rows in casts:
        outcomes = [int(m.group(1)) for line in rows if (m := OUTCOME_RE.search(line))]
        renders = [m.groups() for line in rows if (m := RENDER_RE.search(line))]
        stages = {stage for stage, result, _type, _subtype, _target in renders
                  if int(result) == 1}
        if any(target >= 0 for target in outcomes) and {"cross-submit", "ground-submit"} <= stages:
            selected.append((outcomes, renders))

    assert selected, "no selected-target Spin cast has both native cross and ground submissions"
    outcomes, renders = selected[-1]
    assert any(target >= 0 for target in outcomes)
    assert all(int(result) == 1 for stage, result, _type, _subtype, _target in renders
               if stage in {"cross-submit", "ground-submit"})
    print("PASS: source retains Spin target resolution and native fanout")
    print("PASS: selected-target QA cast reached successful cross-submit and ground-submit")
    print("Scope: native submission evidence only; owner pixels, movement/contact parity and cleanup remain open")


if __name__ == "__main__":
    main()
