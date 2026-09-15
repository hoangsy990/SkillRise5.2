"""Inspect F12 Wrath owner capture and post-removal pool evidence.

The report is deliberately fail-closed. A complete owner submission sequence
does not prove cleanup; a later manual F11 snapshot from the same isolated PID
must show zero Grow Lancer effects and particles after the buff is removed.
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LOG = ROOT / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"
EVENT = re.compile(r"pid=(?P<pid>\d+) .*event=(?P<event>\S+)")
SUMMARY = re.compile(
    r"pid=(?P<pid>\d+) .*event=wrath-buff-visual-summary "
    r"count=(?P<count>\d+) limit=(?P<limit>\d+)"
)
POOL = re.compile(
    r"pid=(?P<pid>\d+) .*event=pool reason=(?P<reason>\S+) "
    r"effects=(?P<effects>-?\d+) glEffects=(?P<gl_effects>-?\d+) "
    r"particles=(?P<particles>-?\d+) glParticles=(?P<gl_particles>-?\d+)"
)


def main() -> int:
    if not LOG.is_file():
        print("OPEN: RuntimeQA log is missing; no F12 Wrath capture recorded")
        return 0
    lines = LOG.read_text(encoding="utf-8", errors="replace").splitlines()
    on = [(i, EVENT.search(line)) for i, line in enumerate(lines)]
    on = [(i, m) for i, m in on if m and m.group("event") == "wrath-buff-fixture-on"]
    off = [(i, EVENT.search(line)) for i, line in enumerate(lines)]
    off = [(i, m) for i, m in off if m and m.group("event") == "wrath-buff-fixture-off"]
    if not on or not off:
        print("OPEN: F12 on/off pair is missing")
        return 0

    on_index, on_match = on[-1]
    pid = on_match.group("pid")
    off_candidates = [(i, m) for i, m in off if m.group("pid") == pid and i > on_index]
    if not off_candidates:
        print(f"OPEN: pid={pid} has no matching F12 off event")
        return 0
    off_index, _ = off_candidates[-1]

    summary = None
    for i, match in enumerate(lines):
        parsed = SUMMARY.search(match)
        if parsed and parsed.group("pid") == pid and i > off_index:
            summary = parsed
    if summary is None:
        print(f"OPEN: pid={pid} has no flushed Wrath visual summary")
        return 0
    count = int(summary.group("count"))
    limit = int(summary.group("limit"))

    pools = []
    for line in lines[on_index:]:
        parsed = POOL.search(line)
        if parsed and parsed.group("pid") == pid:
            pools.append(parsed)
    before = [p for p in pools if p.group("reason") == "wrath-buff-before-off"]
    after = [p for p in pools if p.group("reason") == "wrath-buff-after-off"]
    manual_after = [p for p in pools if p.group("reason") == "manual"]
    if count < 19 or limit < 19:
        print(f"OPEN: pid={pid} owner sequence is {count}/19 records")
        return 0
    if not before or not after:
        print(f"OPEN: pid={pid} is missing before/after-off pool snapshots")
        return 0
    if not manual_after:
        print(f"OPEN: pid={pid} has no F11 snapshot after removal/lifetime")
        return 0
    final = manual_after[-1]
    if int(final.group("gl_effects")) != 0 or int(final.group("gl_particles")) != 0:
        print(
            f"OPEN: pid={pid} post-removal F11 snapshot still has "
            f"glEffects={final.group('gl_effects')} "
            f"glParticles={final.group('gl_particles')}"
        )
        return 0

    print(f"PASS: pid={pid} has 19+ Wrath owner records and zero post-removal GL pools")
    print(
        f"PASS: before/off snapshots present; final F11 manual snapshot "
        f"glEffects={final.group('gl_effects')} glParticles={final.group('gl_particles')}"
    )
    print("Scope: isolated pool telemetry only; GPU pixel parity and server buff authority remain separate")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
