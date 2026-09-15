"""Check one offline native Magic Pin mesh draw, not gameplay pixels."""

import argparse
import re
from pathlib import Path


LOG = (Path(__file__).resolve().parents[2] /
       "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    args = parser.parse_args()
    rows = {}
    release = []
    shader = []
    for line in LOG.read_text(encoding="utf-8", errors="replace").splitlines():
        values = dict(re.findall(r"\b(\w+)=([^\s]+)", line))
        if values.get("pid") != str(args.pid):
            continue
        if values.get("event") == "magic-native-fixture-draw":
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
        assert values["sourceScale"] == "1", key
        assert values["fixturePose"] == "1" and values["gameplayCamera"] == "0"
    print(f"PASS: pid {args.pid} 8 native Magic Pin strip/root angle draws and release")
    print("Scope: source scale/mesh flags, fixture pose/projection; no caster-foot owner visual PASS")


if __name__ == "__main__":
    main()
