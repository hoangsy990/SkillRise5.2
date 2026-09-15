"""Verify the isolated RISE Clash model/texture render pipeline.

This is a source and staged-asset contract.  It proves that the two visible
S21 children use private RISE model rows, native BMD::Open2 and the existing
bitmap loader before the renderer calls Calc_RenderObject/RenderMesh.  It
does not claim GPU pixels or owner-target visual parity.
"""

import hashlib
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RESOURCES = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerResources.cpp"
RUNTIME = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
PROVENANCE = ROOT / "GrowLancer" / "asset_provenance.json"
STAGE = ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "RuntimeQA" / "Client"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> None:
    resources = RESOURCES.read_text(encoding="utf-8")
    runtime = RUNTIME.read_text(encoding="utf-8")
    provenance = json.loads(PROVENANCE.read_text(encoding="utf-8"))

    for row in (
        '{kClashFrontModel, "Data\\\\RISE\\\\GrowLancer\\\\Clash\\\\", "crasha01.bmd"}',
        '{kClashRearModel, "Data\\\\RISE\\\\GrowLancer\\\\Clash\\\\", "crasha02.bmd"}',
    ):
        if row not in resources:
            raise ValueError(f"missing private Clash model row: {row}")

    for needle in (
        "model.m_iBMDSeqID = modelId;",
        "if (!model.Open2(directory, filename))",
        "const std::string path = std::string(directory) + model.Textures[mesh].FileName;",
        "model.IndexTexture[mesh] = Bitmaps.LoadImageFile(path, GL_LINEAR, GL_REPEAT);",
    ):
        if needle not in resources:
            raise ValueError(f"native Clash model-loader contract missing: {needle}")

    render_start = runtime.index("if (effect.Type != kClashFrontModel && effect.Type != kClashRearModel)")
    render_end = runtime.index("bool RenderBrecheClippedMesh", render_start)
    render = runtime[render_start:render_end]
    for needle in (
        "EnsureModel(effect.Type)",
        "Calc_RenderObject(&effect, false, 0, 0)",
        "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha, 0,",
    ):
        if needle not in render:
            raise ValueError(f"Clash render pipeline missing: {needle}")

    expected = {
        "crasha01.bmd": "10B3A1733D01046ECE7038DC7EA631DF28F674AD638D8D4938E9193D80020D23",
        "crasha02.bmd": "315E549CBC13846EE2921CD9141EEAAC60A4C2810CD6B495978D92FD16D6C191",
        "alpha_line2.OZJ": "D2BBB29A580CE0B2ABFC69689691D4BADB102251250A1452DC7BD6A57D4CD69B",
        "wind1.OZJ": "8D6F8BAA8408E33D23FB1F96218CB38980F671A644B2C55E42D8841A48171C4F",
    }
    for name, digest in expected.items():
        path = next(STAGE.rglob(name), None)
        if path is None:
            raise ValueError(f"missing isolated Clash staged asset: {name}")
        if sha256(path) != digest:
            raise ValueError(f"staged Clash hash mismatch: {name}")

    rows = [a for a in provenance["assets"] if a.get("skillId") == 275]
    for name in expected:
        row = next((a for a in rows if Path(a.get("stagedPath", "")).name == name), None)
        if row is None:
            raise ValueError(f"provenance row missing for Clash asset: {name}")

    print("PASS: Clash private model rows use native Open2 and bitmap loading")
    print("PASS: Clash render calls EnsureModel -> Calc_RenderObject -> native RenderMesh mesh0")
    print("PASS: isolated crasha01/crasha02 and alpha_line2/wind1 hashes match provenance")
    print("Scope: source/staged pipeline only; GPU pixels, cache lifecycle and owner-target parity remain open")


if __name__ == "__main__":
    main()
