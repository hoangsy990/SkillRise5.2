"""Pin actual RISE mesh bright-blend/cull behavior, not prequeue QA state."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2] / "ExMain_RISE_PC/Main5.2_RISE"


def main() -> None:
    qa = (ROOT / "RISE/GrowLancerRuntimeQA.cpp").read_text(encoding="utf-8", errors="replace")
    render = (ROOT / "New_RenderBMD.cpp").read_text(encoding="utf-8", errors="replace")
    opengl = (ROOT / "ZzzOpenglUtil.cpp").read_text(encoding="utf-8", errors="replace")
    assert "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT" in qa
    texture_branch = render[render.index("else if ((command.RenderFlags & RENDER_TEXTURE) == RENDER_TEXTURE)"):]
    bright = texture_branch[texture_branch.index("if ((command.RenderFlags & RENDER_BRIGHT) == RENDER_BRIGHT)"):]
    assert bright.index("EnableAlphaBlend();") < bright.index("else if ((command.RenderFlags & RENDER_DARK)"), bright[:400]
    blend = opengl[opengl.index("void EnableAlphaBlend()"):opengl.index("void EnableAlphaBlendMinus()")]
    assert "DisableCullFace();" in blend
    assert "DisableDepthMask();" in blend
    assert "glBlendFunc(GL_ONE, GL_ONE);" in blend
    print("PASS native 5.2 Clash flags0x42 -> texture+bright -> additive blend/cull-off/depth-mask-off")
    print("Scope: native RISE renderer source only; S21 render-state and owner gameplay parity still OPEN")


if __name__ == "__main__":
    main()
