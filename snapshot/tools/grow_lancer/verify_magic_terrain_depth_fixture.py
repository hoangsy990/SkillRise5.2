"""Pin native Magic bright meshes against a depth-writing flat-ground control."""

from pathlib import Path
import argparse
import re


ROOT = Path(__file__).resolve().parents[2]
LOG = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
       "RuntimeQA" / "Client" / "GrowLancerRuntimeQA.log")
SOURCE = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" /
          "GrowLancerRuntimeQA.cpp")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    args = parser.parse_args()
    rows = {}
    shader = []
    released = []
    with LOG.open(encoding="utf-8", errors="replace") as handle:
        for line in handle:
            if f"pid={args.pid} " not in line:
                continue
            values = dict(re.findall(r"\b([A-Za-z][A-Za-z0-9-]*)=([^\s]+)", line))
            event = values.get("event")
            if event == "magic-terrain-depth-fixture-draw":
                key = (int(values["type"]), int(values["rotation"]),
                       int(values["flatPlane"]))
                assert key not in rows, key
                rows[key] = values
            elif event == "body-shader":
                shader.append(values)
            elif event == "magic-native-fixture-release":
                released.append(values)
    expected = {(model, angle, plane) for model in (10125, 10127)
                for angle in (0, 90, 180, 270) for plane in (0, 1)}
    assert set(rows) == expected, (len(rows), expected - set(rows))
    assert len(shader) == 1 and shader[0]["enabled"] == "1"
    assert len(released) == 1 and released[0]["stable"] == "1"
    for key, row in rows.items():
        assert row["calcReady"] == row["opened"] == row["meshReady"] == "1", key
        assert row["depth"] == "1" and row["glError"] == "0", key
        assert row["fixtureTerrain"] == "1" and row["gameplayTerrain"] == "0"
        assert float(row["preModelCenterDepth"]) == 1.0  # above flat-plane coverage
        ground = float(row["preModelGroundDepth"])
        assert (ground == 1.0 if key[2] == 0 else 0.0 < ground < 1.0), key
        assert int(row["samples"]) > 0, key
    source = SOURCE.read_text(encoding="utf-8")
    assert source.index("#ifdef RISE_GROW_LANCER_RUNTIME_QA") < source.index(
        "RISE_GL_MAGIC_TERRAIN_DEPTH_QA") < source.rindex("#endif")
    for required in ("Calc_RenderObject(&nativeEffect, false, 0, 0)",
                     "glEnable(GL_DEPTH_TEST)", "glColorMask(GL_FALSE",
                     "glVertex3f(-300.0f, -300.0f, 0.0f)",
                     "glReadPixels(128, 64, 1, 1, GL_DEPTH_COMPONENT"):
        assert required in source, required
    print(f"PASS hidden native Magic flat-depth control pid={args.pid}: "
          "16 strip/root draws, ground depth writes, model samples>0, GL0/release")
    print("OPEN actual terrain/camera, owner foot pixels, frame recurrence, S21 parity")


if __name__ == "__main__":
    main()
