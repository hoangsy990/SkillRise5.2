"""Check one offline native Spin cross draw; not gameplay or S21 frame parity."""

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(
    encoding="utf-8")
LOG = ROOT / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int)
    args = parser.parse_args()
    assert '"RISE_GL_SPIN_CROSS_DRAW_QA"' in QA
    assert "spinCrossDrawProbe ? kSpinCrossModel" in QA
    assert "spinCrossDrawProbe ? 8.5f : 0.7f" in QA
    assert "spinCrossDrawProbe ? 120.0f" in QA
    assert "RENDER_TEXTURE | RENDER_BRIGHT" in QA
    assert "litPixels > 0" in QA
    if args.pid is None:
        print("PASS: Spin cross hidden draw is opt-in, source-scoped and native")
        return
    rows = {}
    releases = []
    for line in LOG.read_text(encoding="utf-8", errors="replace").splitlines():
        values = dict(re.findall(r"\b(\w+)=([^\s]+)", line))
        if values.get("pid") != str(args.pid):
            continue
        if values.get("event") == "spin-cross-native-fixture-draw":
            rotation = int(values["cameraSweep"])
            assert rotation not in rows
            rows[rotation] = values
        elif values.get("event") == "spin-cross-native-fixture-release":
            releases.append(values)
    assert set(rows) == {0, 90, 180, 270}, rows
    assert len(releases) == 1 and releases[0]["stable"] == "1"
    for rotation, values in rows.items():
        assert values["type"] == "10124", rotation
        assert values["sourceAngle"] == "120"
        assert values["sourceScale"] == "8.5"
        assert values["opened"] == values["meshReady"] == "1"
        assert int(values["samples"]) > 0 and int(values["litPixels"]) > 0
        assert values["glError"] == "0"
        assert values["fixturePose"] == "1" and values["gameplayCamera"] == "0"
    print(f"PASS: pid {args.pid} four native Spin cross fixture camera draws and release")
    print("Scope: S21 model/scale/angle and native mesh; owner ribbon/target pixels OPEN")


if __name__ == "__main__":
    main()
