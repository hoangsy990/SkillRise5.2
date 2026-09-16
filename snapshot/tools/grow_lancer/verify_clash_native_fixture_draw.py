"""Classify isolated Clash mesh pixels; synthetic camera is not owner parity."""

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LOG = ROOT / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"
SOURCE = ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp"
ANGLES = (0, 45, 90, 135, 180, 225, 270, 315)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    parser.add_argument("--cull-off", action="store_true")
    parser.add_argument("--require-projection", action="store_true")
    args = parser.parse_args()
    rows = {}
    shader = []
    releases = []
    projections = {}
    for line in LOG.read_text(encoding="utf-8", errors="replace").splitlines():
        fields = dict(re.findall(r"\b(\w+)=([^\s]+)", line))
        if fields.get("pid") != str(args.pid):
            continue
        event = fields.get("event")
        if event == "clash-native-fixture-draw":
            key = (int(fields["type"]), int(fields["rotation"]))
            assert key not in rows, f"duplicate draw {key}"
            rows[key] = fields
        elif event == "clash-native-bone-projection":
            angle = int(fields["rotation"])
            assert angle not in projections, angle
            projections[angle] = fields
        elif event == "body-shader":
            shader.append(fields)
        elif event == "clash-native-fixture-release":
            releases.append(fields)

    expected = {(kind, angle) for kind in (10135, 10136) for angle in ANGLES}
    assert set(rows) == expected, f"missing/extra Clash draw rows: {set(rows) ^ expected}"
    assert len(shader) == 1 and shader[0]["enabled"] == "1"
    assert len(releases) == 1 and releases[0]["stable"] == "1"
    missing = []
    for key, fields in rows.items():
        assert fields["opened"] == fields["meshReady"] == fields["calcReady"] == "1", key
        assert fields["glError"] == "0", key
        # This row samples cull before the queued RenderMesh call. The native
        # bright-blend path changes cull during FlushAllMesh, so it is not a
        # draw-state assertion or a meaningful cull differential by itself.
        assert fields["cull"] in ("0", "1"), key
        if args.cull_off:
            assert fields["cull"] == "0", key
        assert fields["depth"] == "1", key
        assert fields["fixturePosition"] == "1" and fields["gameplayCamera"] == "0", key
        assert fields["scale"] == ("0.89" if key[0] == 10135 else "1.25"), key
        assert fields["alpha"] == ("0.65" if key[0] == 10135 else "0.90"), key
        visible = int(fields["samples"]) > 0 and int(fields["litPixels"]) > 0
        if not visible:
            missing.append(key)
    source = SOURCE.read_text(encoding="utf-8")
    for term in ("RISE_GL_CLASH_NATIVE_DRAW_QA", "kClashFrontModel",
                 "kClashRearModel", "Calc_RenderObject(&nativeEffect, false, 0, 0)",
                 "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT"):
        assert term in source, term
    if args.require_projection:
        assert set(projections) == set(ANGLES), projections.keys()
        for angle, fields in projections.items():
            assert fields["type"] == "10136" and fields["fixtureCamera"] == "1"
            area = float(fields["doubledArea"])
            if angle in (0, 180):
                assert area < 0.1, (angle, area)
                assert (10136, angle) in missing, angle
            else:
                assert area > 100000, (angle, area)
                assert (10136, angle) not in missing, angle
    print(f"PASS diagnostic integrity: pid={args.pid}, cullOff={args.cull_off}, 16 exact native Clash fixture draws, shader and release")
    print("ZERO-PIXEL fixture angles:", missing)
    if args.require_projection:
        print("PASS fixture projection classification: rear 0/180 edge-on after native bone transform")
    if args.require_projection:
        print("Scope: native edge-on fixture contract PASS; S21 camera and owner gameplay visual still OPEN")
    else:
        print("Scope: isolated mesh/camera/GL only; rear zero-pixel rows unexplained, no owner visual PASS")


if __name__ == "__main__":
    main()
