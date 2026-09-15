"""Fail-closed QA check for the staged Spin Step native blur draw/map records.

PASS is limited to native draw submission and style-1 pool retirement. It does
not establish visible pixels, S21 action parity, hit damage or GameServer QA.
"""
from __future__ import annotations

import argparse
import re
from pathlib import Path

DEFAULT_LOG = Path(
    r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests"
    r"\GrowLancerBuild\RuntimeQA\Client\GrowLancerRuntimeQA.log"
)
PID = re.compile(r"\bpid=(\d+)\b")
LIVE = re.compile(r"\blive=(\d+)\b")
REGISTERED = re.compile(r"\bregistered=(\d+)\b")
TEXTURE = re.compile(r"\btexture=(\d+)\b")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", type=Path, default=DEFAULT_LOG)
    parser.add_argument("--since", default="2026-09-15T05:20:00")
    parser.add_argument("--expected-texture", type=int,
                        help="Require every style-1 draw to use this native bitmap ID; omit for historical casts")
    args = parser.parse_args()
    if not args.log.is_file():
        print(f"PENDING: QA log missing: {args.log}")
        return 2

    roots: list[tuple[str, int]] = []
    records: list[tuple[str, int, str]] = []
    with args.log.open(encoding="utf-8", errors="replace") as stream:
        for line in stream:
            if not line.startswith("2026-") or line[:19] < args.since[:19]:
                continue
            found = PID.search(line)
            if not found:
                continue
            pid = int(found.group(1))
            timestamp = line[:24]
            if "event=root-dispatched skill=271" in line:
                roots.append((timestamp, pid))
            if ("event=spin-blur-draw" in line or
                "event=map-style1-blurs" in line or
                "event=spin-render stage=cross-submit" in line):
                records.append((timestamp, pid, line.strip()))

    if not roots:
        print("PENDING: no Spin Step root-dispatched cast after candidate stage")
        return 2
    cast_time, pid = roots[-1]
    chosen = [line for timestamp, record_pid, line in records
              if record_pid == pid and timestamp >= cast_time]
    draws = [line for line in chosen if "event=spin-blur-draw" in line]
    crosses = [line for line in chosen
               if "event=spin-render stage=cross-submit" in line]
    maps = [line for line in chosen if "event=map-style1-blurs" in line]
    after = [line for line in maps if "phase=map-after-delete" in line]
    print(f"latest_spin_pid={pid} cast={cast_time} draw={len(draws)} "
          f"cross_submit={len(crosses)} map_records={len(maps)}")
    if not draws:
        print("PENDING: blur allocator/root seen but native draw fan not recorded")
        return 2
    if any((match := REGISTERED.search(line)) is None or match.group(1) != "1"
           for line in draws):
        print("FAIL: style-1 draw reached an unregistered native bitmap")
        return 1
    if args.expected_texture is not None and any(
        (match := TEXTURE.search(line)) is None or
        int(match.group(1)) != args.expected_texture for line in draws
    ):
        print(f"FAIL: style-1 draw did not bind native bitmap {args.expected_texture}")
        return 1
    if not after:
        print("PENDING: native draw observed, but no map-after-delete pool record")
        return 2
    if any((match := LIVE.search(line)) is None or match.group(1) != "0"
           for line in after):
        print("FAIL: style-1 object-blur record survived map effect teardown")
        return 1
    print("PASS: bounded native Spin draw-path/bitmap and map style-1 retirement")
    print("Scope: visible pixels, S21 parity, movement and GS damage remain OPEN")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
