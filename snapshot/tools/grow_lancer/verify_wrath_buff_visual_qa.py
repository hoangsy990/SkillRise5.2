#!/usr/bin/env python3
"""Verify bounded QA telemetry for the native Wrath player-buff sequence.

This is a source guard only. It proves that the QA build can arm and record one
owner sequence through the isolated F12 fixture after buff 424 is delivered;
it does not claim that the buff is learned, active, or visually accepted in
gameplay.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA_H = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.h"
QA_CPP = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp"
RUNTIME = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp"


def main() -> None:
    header = QA_H.read_text(encoding="utf-8")
    qa = QA_CPP.read_text(encoding="utf-8")
    runtime = RUNTIME.read_text(encoding="utf-8")

    assert '#include "_GlobalFunctions.h"' in qa
    assert "void ToggleWrathBuffFixtureQA()" in qa
    fixture = qa[qa.index("void ToggleWrathBuffFixtureQA()"):
                 qa.index("bool HandleRuntimeQAHotKey()")]
    assert "static_cast<eBuffState>(424)" in fixture
    assert "g_CharacterRegisterBuff((&caster), buff)" in fixture
    assert "g_CharacterUnRegisterBuff((&caster), buff)" in fixture
    assert "FlushControllerSamples();" in fixture
    assert "gControllerSkill = 278;" in fixture
    assert "gControllerStart = GetTickCount();" in fixture
    assert 'WritePoolSnapshot("wrath-buff-on")' in fixture
    assert 'WritePoolSnapshot("wrath-buff-before-off")' in fixture
    assert 'WritePoolSnapshot("wrath-buff-after-off")' in fixture
    assert "no packet" in fixture and "server authority" in fixture
    assert "SEASON3B::IsPress(VK_F12)" in qa

    # No QA buff is injected merely because the owner logged in. The
    # historical auto-on-login path was removed; F12 remains manual.
    assert "DriveAutoWrathBuffFixtureQA" not in qa
    assert 'RISE_GL_AUTO_WRATH_QA' not in qa
    launcher = (ROOT / "tools/grow_lancer/start_runtime_qa_client.ps1").read_text()
    assert "if ($AutoWrath) { throw '-AutoWrath retired:" in launcher
    assert "$env:RISE_GL_AUTO_WRATH_QA = '0'" in launcher

    assert "RecordWrathBuffVisualQA(const OBJECT& caster" in header
    assert "WrathBuffVisualSample gWrathBuffVisualSamples[32]" in qa
    assert "gWrathBuffVisualCount = 0" in qa
    assert "gWrathBuffVisualCount >= 32" in qa
    assert "&caster != &Hero->Object" in qa
    assert 'event=wrath-buff-visual-summary count=%u limit=32' in qa
    assert 'event=wrath-buff-visual index=%u stage=%s' in qa

    start = runtime.index("bool SubmitWrathPersistentSprites")
    end = runtime.index("bool RenderWrathPersistentGround", start)
    sprite = runtime[start:end]
    assert 'RecordWrathBuffVisualQA(caster, "lightmarks"' in sprite
    assert 'RecordWrathBuffVisualQA(caster, "flare"' in sprite

    start = runtime.index("bool SubmitWrathPersistentVisuals")
    end = runtime.index("bool RenderEffect(OBJECT& effect)", start)
    visual = runtime[start:end]
    for stage in ('"scatter"', '"mono"'):
        assert f"RecordWrathBuffVisualQA(caster, {stage}" in visual

    # Keep the observation separate from the actual buff gate and emission
    # order.  The production branch still checks both native buff IDs.
    assert "g_isCharacterBuff(buffOwner, static_cast<eBuffState>(424))" in visual
    assert "g_isCharacterBuff(buffOwner, static_cast<eBuffState>(425))" in visual
    assert visual.index("SubmitWrathPersistentSprites(caster,false)") < visual.index(
        'RecordWrathBuffVisualQA(caster, "scatter"')

    print("PASS: Wrath QA records one owner-only 32-entry sequence including the post-buff tail")
    print("Scope: telemetry wiring only; buff delivery, pixels, cadence and cleanup remain open")


if __name__ == "__main__":
    main()
