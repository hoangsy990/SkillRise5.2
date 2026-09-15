#!/usr/bin/env python3
"""Verify the latest isolated native class-body GPU probe evidence.

This consumes only the private RuntimeQA log.  It proves the bounded shader,
upload, cache, draw and release probe; it does not claim gameplay class
activation, S21 pose parity, or final screen-pixel parity.
"""
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
LOG = ROOT / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"


def rows_for_latest_pid(text: str) -> tuple[int, list[str]]:
    rows = text.splitlines()
    pids = [int(m.group(1)) for row in rows if (m := re.search(r"\bpid=(\d+)\b", row))]
    if not pids:
        raise AssertionError("no PID-tagged RuntimeQA rows")
    latest = pids[-1]
    selected = [row for row in rows if f"pid={latest} " in row]
    return latest, selected


def require(rows: list[str], needle: str, count: int = 1) -> None:
    actual = sum(needle in row for row in rows)
    assert actual >= count, f"{needle!r}: expected >= {count}, got {actual}"


def require_log(text: str, needle: str) -> None:
    actual = sum(needle in row for row in text.splitlines())
    assert actual >= 1, f"{needle!r}: expected a passing fixture row"


def main() -> None:
    text = LOG.read_text(encoding="utf-8")
    pid, rows = rows_for_latest_pid(text)
    require(rows, "event=body-shader enabled=1")
    for filename, width, height in (
        ("hair_Class108.tga", 64, 64),
        ("skin_Class108.jpg", 256, 256),
        ("Skin_Class308.jpg", 256, 256),
    ):
        require(rows, f"event=body-texture-upload file={filename} loaded=1 gpuWidth={width} gpuHeight={height} glError=0")
        require(rows, f"event=body-texture-readback file={filename} cpuGpuEqual=1")
    require(rows, "event=body-texture-missing rejected=1")
    require(rows, "event=body-adapter part=0 loaded=1 cached=1 countersStable=1")
    require(rows, "event=body-adapter part=1 loaded=1 cached=1 countersStable=1")
    require(rows, "event=body-adapter part=2 loaded=1 cached=1 countersStable=1")
    require(rows, "event=body-adapter part=3 loaded=1 cached=1 countersStable=1")
    require(rows, "event=body-adapter part=4 loaded=1 cached=1 countersStable=1")
    require(rows, "event=body-gpu sizesMatch=1", 5)
    require(rows, "event=body-gpu callerBindingsRestored=1", 5)
    require(rows, "event=body-gpu cacheHandlesStable=1", 5)
    for part in range(5):
        require(rows, f"event=body-native-draw part={part} accepted=1")
        draw = next(row for row in rows if f"event=body-native-draw part={part} " in row)
        match = re.search(r"samples=(\d+) glError=0", draw)
        assert match and int(match.group(1)) > 0, draw
        for test in range(4):
            require(rows, f"event=body-owner-submit part={part} test={test} ", 1)
            owner = next(row for row in rows if f"event=body-owner-submit part={part} test={test} " in row)
            assert "pass=1" in owner and "glError=0" in owner, owner
    require(rows, "event=body-gpu released=1")
    require(rows, "event=body-texture-release baselineRestored=1")
    # These are deliberately separate process fixtures: the rollback fixture
    # changes cwd and the sampler fixture intentionally preloads a conflicting
    # native texture.  Accept only rows whose complete pass tuple is present.
    require_log(text, "event=body-sampler-conflict preloaded=1 rejected=1 reset=1 refPreserved=1 countersStable=1 retry=1")
    require_log(text, "event=body-adapter-rollback firstPresent=1 secondAbsent=1 rejected=1 reset=1 refsRestored=1")
    print(f"PASS: latest isolated native class-body GPU probe PID {pid} covers shader/upload/cache/draw/owner/release")
    print("PASS: separate sampler-conflict and missing-material rollback fixtures both pass")
    print("Scope: private probe evidence only; class activation, S21 pose parity and final screen pixels remain open")


if __name__ == "__main__":
    main()
