"""Pin the opt-in Spin cross gameplay draw query to one native mesh call."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(
    encoding="utf-8")
RUNTIME = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(
    encoding="utf-8")
LAUNCHER = (ROOT / "tools/grow_lancer/start_runtime_qa_client.ps1").read_text(
    encoding="utf-8")


def main() -> None:
    begin = QA.split("unsigned BeginSpinCrossSamplesQA", 1)[1].split(
        "void EndSpinCrossSamplesQA", 1)[0]
    end = QA.split("void EndSpinCrossSamplesQA", 1)[1].split(
        "void ResetBrecheCasterRenderQA", 1)[0]
    draw = RUNTIME.split("const bool zeroMagicUv", 1)[1].split(
        "return true;", 1)[0]
    assert "gSpinCrossProbeCount >= 12" in begin
    assert "gSpinCrossProbeCount = 0" in QA
    assert "gControllerSkill != 271" in begin
    assert "effect.Type != kSpinCrossModel" in begin
    assert "!effect.Owner || !effect.Owner->Live" in begin
    assert 'GetEnvironmentVariableA("RISE_GL_SPIN_CROSS_RASTER_QA"' in begin
    assert "GL_CURRENT_QUERY" in begin and "if (active) return 0" in begin
    assert "glGenQueries" in begin and "glBeginQuery(GL_SAMPLES_PASSED" in begin
    assert "GMMeshShader->FlushAllMesh()" in begin and end
    assert "glEndQuery(GL_SAMPLES_PASSED)" in end
    assert "glGetQueryObjectuiv" in end and "glDeleteQueries" in end
    assert "event=spin-cross-raster-probe" in end
    assert "effect.Position[0]" in end and "effect.Owner->Position[0]" in end
    assert draw.index("BeginSpinCrossSamplesQA(effect)") < draw.index("model.RenderMesh(0")
    assert draw.index("model.RenderMesh(0") < draw.index("EndSpinCrossSamplesQA(effect")
    assert "[switch]$SpinCrossRasterProbe" in LAUNCHER
    assert "$env:RISE_GL_SPIN_CROSS_RASTER_QA = if ($SpinCrossRasterProbe)" in LAUNCHER
    assert "$hadSpinCrossRaster" in LAUNCHER
    assert "Remove-Item Env:RISE_GL_SPIN_CROSS_RASTER_QA" in LAUNCHER
    print("PASS: Spin cross gameplay query is bounded, opt-in and mesh-scoped")
    print("Scope: source/build only until fresh local F7; samples are not visual PASS")


if __name__ == "__main__":
    main()
