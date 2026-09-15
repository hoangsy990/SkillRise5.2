"""Guard the bounded, opt-in QA readback around native Wrath terrain."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(
    encoding="utf-8"
)
RUNTIME = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(
    encoding="utf-8"
)
LAUNCHER = (ROOT / "tools/grow_lancer/start_runtime_qa_client.ps1").read_text(
    encoding="utf-8"
)


def main() -> None:
    begin = QA.split("unsigned BeginWrathGroundPixelsQA", 1)[1].split(
        "void EndWrathGroundPixelsQA", 1
    )[0]
    end = QA.split("void EndWrathGroundPixelsQA", 1)[1].split(
        "void RecordWrathGroundQA", 1
    )[0]
    draw = RUNTIME.split("if (effect.Type == kWrathBrokenBitmap)", 1)[1].split(
        "if (effect.Type == kSpriteCarrierModel)", 1
    )[0]
    assert "WrathGroundPixelSample gWrathGroundPixels[4]" in QA
    assert "WrathGroundPixelPoint points[5]" in QA
    assert "gWrathGroundPixelCount >= 4" in begin
    assert 'GetEnvironmentVariableA("RISE_GL_WRATH_PIXEL_QA"' in begin
    assert "gControllerSkill != 278" in begin
    assert "glGetQueryiv(GL_SAMPLES_PASSED, GL_CURRENT_QUERY" in begin
    assert "RequestTerrainHeight" in begin and "TransformPosition" in begin
    assert begin.index("PerspectiveX <= 0.000001f") < begin.index("TransformPosition(")
    assert begin.index("VectorSubtract(ground, CameraPosition") < begin.index("TransformPosition(")
    assert begin.index("VectorRotate(fromCamera, CameraMatrix") < begin.index("TransformPosition(")
    assert begin.index("cameraSpace[2] >= -1.0f") < begin.index("TransformPosition(")
    assert "GL_READ_BUFFER" in begin and "GL_DRAW_BUFFER" in begin
    assert "glReadPixels" in begin and "GL_RGBA, GL_UNSIGNED_BYTE" in begin
    assert "glBeginQuery(GL_SAMPLES_PASSED" in begin
    assert "glEndQuery(GL_SAMPLES_PASSED)" in end
    assert "glGetQueryObjectuiv" in end and "glDeleteQueries" in end
    assert "glReadPixels" in end and "GL_RGBA, GL_UNSIGNED_BYTE" in end
    ground = QA.split("void RecordWrathGroundQA", 1)[1].split(
        "void RecordWrathBuffVisualQA", 1
    )[0]
    assert "Bitmaps.FindTexture(effect.Type)" in ground
    assert "sample.expectedTexture" in ground
    assert "matched=%d" in QA
    assert draw.index("EnableAlphaBlend();") < draw.index("BeginWrathGroundPixelsQA")
    assert draw.index("BeginWrathGroundPixelsQA") < draw.index("RenderTerrainAlphaBitmap(")
    assert draw.index("RenderTerrainAlphaBitmap(") < draw.index("EndWrathGroundPixelsQA")
    assert draw.index("EndWrathGroundPixelsQA") < draw.index("DisableAlphaBlend();")
    assert "[switch]$WrathPixelProbe" in LAUNCHER
    assert "$env:RISE_GL_WRATH_PIXEL_QA = if ($WrathPixelProbe)" in LAUNCHER
    assert "$hadWrathPixel" in LAUNCHER and "Remove-Item Env:RISE_GL_WRATH_PIXEL_QA" in LAUNCHER
    print("PASS: opt-in Wrath native ground query/readback is bounded and state-scoped")
    print("Scope: source/QA build only; no gameplay pixels or black-square visual PASS")


if __name__ == "__main__":
    main()
