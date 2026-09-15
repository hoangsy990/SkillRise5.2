"""Summarize the latest bounded Clash/Breche render telemetry.

This is a read-only QA log tool.  It reports native submission stages and
never treats a stage as proof of GPU pixels or visual parity.
"""

from __future__ import annotations

import argparse
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_LOG = ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "RuntimeQA" / "Client" / "GrowLancerRuntimeQA.log"
CAST_RE = re.compile(r"event=cast-input phase=before-dispatch skill=(\d+)")
RENDER_RE = re.compile(r"event=target-skill-render skill=(\d+) stage=([^ ]+) result=(-?\d+)")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("log", nargs="?", type=Path, default=DEFAULT_LOG)
    args = parser.parse_args()
    if not args.log.is_file():
        raise SystemExit(f"log missing: {args.log}")

    latest_cast = {275: -1, 279: -1}
    cast_serial = 0
    render_rows: list[tuple[int, int, str, int]] = []
    for line in args.log.read_text(encoding="utf-8", errors="replace").splitlines():
        cast = CAST_RE.search(line)
        if cast and int(cast.group(1)) in latest_cast:
            cast_serial += 1
            latest_cast[int(cast.group(1))] = cast_serial
        render = RENDER_RE.search(line)
        if render:
            skill = int(render.group(1))
            render_rows.append((skill, latest_cast.get(skill, -1), render.group(2), int(render.group(3))))

    for skill, name in ((275, "Clash"), (279, "Breche")):
        serial = latest_cast[skill]
        rows = [row for row in render_rows if row[0] == skill and row[1] == serial]
        if not rows:
            print(f"OPEN: {name} has no bounded target-render records in the latest cast")
            continue
        stages = Counter(row[2] for row in rows)
        failures = [row for row in rows if row[3] == 0]
        print(f"INFO: {name} latest cast records={len(rows)} stages={dict(stages)}")
        if failures:
            print(f"OPEN: {name} has result=0 stages {[row[2] for row in failures]}")
        else:
            print(f"INFO: {name} all recorded stages returned result=1")
    print("Scope: read-only native submission telemetry; GPU pixels and visual parity remain unproven")


if __name__ == "__main__":
    main()
