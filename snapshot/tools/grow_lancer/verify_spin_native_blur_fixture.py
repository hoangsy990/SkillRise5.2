"""Pin one hidden native style-1 blur framebuffer probe; not gameplay QA."""

from pathlib import Path
import argparse
import re


ROOT = Path(__file__).resolve().parents[2]
LOG = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
       "RuntimeQA" / "Client" / "GrowLancerRuntimeQA.log")
def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    parser.add_argument("--mode", choices=("synthetic-segments", "native-player-pose",
                                           "native-player-pose-depth"))
    args = parser.parse_args()
    rows = [line for line in LOG.read_text(encoding="utf-8", errors="replace").splitlines()
            if f"pid={args.pid} " in line and "event=spin-native-blur-draw" in line]
    assert len(rows) == 1, (args.pid, len(rows))
    values = dict(re.findall(r"\b(\w+)=([^\s]+)", rows[0]))
    mode = values.get("mode", "synthetic-segments")  # old fixture schema
    if args.mode is not None:
        assert mode == args.mode, (mode, args.mode)
    loaded = int(values["loaded"])
    pixels = int(values["litPixels"])
    error = int(values["glError"])
    released = int(values["releaseStable"])
    assert loaded == released == 1 and pixels > 0 and error == 0
    if mode in ("native-player-pose", "native-player-pose-depth"):
        assert values["poseReady"] == "1"
        assert float(values["minWidth"]) > 0.0
        assert float(values["maxCoord"]) > 0.0
    if mode == "native-player-pose-depth":
        assert int(values["noPlanePixels"]) > 0
        assert float(values["groundDepth"]) < 1.0
    source = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" /
              "GrowLancerRuntimeQA.cpp").read_text(encoding="utf-8")
    assert source.index("#ifdef RISE_GROW_LANCER_RUNTIME_QA") < source.index(
        "RISE_GL_SPIN_BLUR_DRAW_QA") < source.rindex("#endif")
    fixture = source.split("if (spinBlurDrawProbe || spinPoseBlurProbe || spinPoseDepthProbe)", 1)[1].split(
        "if (wrathProbe)", 1)[0]
    for required in ("EnsureSpinMotionBlurBitmap()", "CreateObjectBlur(&owner",
                     "RenderObjectBlurs();", "glReadPixels(0, 0, 256, 256",
                     "ClearAllObjectBlurs();"):
        assert required in fixture, required
    if mode in ("native-player-pose", "native-player-pose-depth"):
        for required in ("player.Open2(playerDir, playerFile, true)",
                         "player.Animation(pose, frames[i]",
                         "player.TransformPosition(pose[33]", "minimumWidth"):
            assert required in fixture, required
    if mode == "native-player-pose-depth":
        for required in ("glEnable(GL_DEPTH_TEST)", "glDepthFunc(GL_LESS)",
                         "glBegin(GL_QUADS)", "noPlanePixels", "groundDepth"):
            assert required in fixture, required
    print(f"PASS hidden native Spin blur fixture pid={args.pid} mode={mode} pixels={pixels} GL=0 release=1")
    print("OPEN caster bone/world/camera gameplay pixels and S21 visual parity")


if __name__ == "__main__":
    main()
