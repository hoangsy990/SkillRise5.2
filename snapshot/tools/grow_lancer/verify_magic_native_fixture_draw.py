"""Check one offline native Magic Pin mesh draw, not gameplay pixels."""

import argparse
import re
from pathlib import Path


LOG = (Path(__file__).resolve().parents[2] /
       "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log")
SOURCE = (Path(__file__).resolve().parents[2] /
          "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    parser.add_argument("--mode", choices=("direct-pose", "calc-render"),
                        default="direct-pose")
    args = parser.parse_args()
    rows = {}
    release = []
    shader = []
    for line in LOG.read_text(encoding="utf-8", errors="replace").splitlines():
        values = dict(re.findall(r"\b(\w+)=([^\s]+)", line))
        if values.get("pid") != str(args.pid):
            continue
        draw_event = ("magic-calc-native-fixture-draw" if args.mode == "calc-render"
                      else "magic-native-fixture-draw")
        if values.get("event") == draw_event:
            key = (int(values["type"]), int(values["rotation"]))
            assert key not in rows, f"duplicate draw {key}"
            rows[key] = values
        elif values.get("event") == "magic-native-fixture-release":
            release.append(values)
        elif values.get("event") == "body-shader":
            shader.append(values)
    expected = {(kind, rotation) for kind in (10125, 10127)
                for rotation in (0, 90, 180, 270)}
    assert set(rows) == expected, f"missing exact S21 caster-angle draw: {rows.keys()}"
    assert len(release) == 1 and release[0]["stable"] == "1"
    assert len(shader) == 1 and shader[0]["enabled"] == "1"
    for key, values in rows.items():
        assert values["opened"] == values["meshReady"] == "1", key
        assert int(values["samples"]) > 0, key
        assert values["glError"] == "0", key
        if args.mode == "calc-render":
            assert values["sourceScale"] == "0.7" and values["calcReady"] == "1"
            assert values["fixturePosition"] == "1" and values["gameplayCamera"] == "0"
        else:
            # Retained legacy row says scale=1; the fixture code actually
            # configured BodyScale/RequestScale=0.7, see source, so do not
            # treat the old log label as actor-scale proof.
            assert values["sourceScale"] == "1", key
            assert values["fixturePose"] == "1" and values["gameplayCamera"] == "0"
    if args.mode == "calc-render":
        source = SOURCE.read_text(encoding="utf-8")
        assert source.index("#ifdef RISE_GROW_LANCER_RUNTIME_QA") < source.index(
            "RISE_GL_MAGIC_CALC_RENDER_QA") < source.rindex("#endif")
        for required in ("calcReady = Calc_RenderObject(&nativeEffect, false, 0, 0)",
                         "nativeEffect.Scale = 0.7f", "GMMeshShader->BeginScope()",
                         "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT"):
            assert required in source, required
    print(f"PASS: pid {args.pid} mode={args.mode} 8 native Magic Pin strip/root angle draws and release")
    print("Scope: exact models/flags and fixture camera; no caster-foot owner visual PASS")


if __name__ == "__main__":
    main()
