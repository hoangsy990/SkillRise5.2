"""Inspect one bounded owner Wrath visual sequence from RuntimeQA.log.

The inspector is deliberately fail-closed: a missing or short sequence is
reported as OPEN, never promoted to a visual PASS.  It validates submission
telemetry only and does not read pixels or mutate the client.
"""

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
LOG = ROOT / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"
SUMMARY = re.compile(
    r"pid=(?P<pid>\d+) .*event=wrath-buff-visual-summary "
    r"count=(?P<count>\d+) limit=(?P<limit>\d+)"
)
RECORD = re.compile(
    r"pid=(?P<pid>\d+) .*event=wrath-buff-visual index=(?P<index>\d+) "
    r"stage=(?P<stage>\S+) variant=(?P<variant>-?\d+) "
    r"attachment=(?P<attachment>-?\d+) bone=(?P<bone>-?\d+)"
)


def main() -> int:
    if not LOG.is_file():
        print("OPEN: RuntimeQA log is missing; no owner Wrath cast recorded")
        return 0
    lines = LOG.read_text(encoding="utf-8", errors="replace").splitlines()
    summaries = [(i, SUMMARY.search(line)) for i, line in enumerate(lines)]
    summaries = [(i, m) for i, m in summaries if m]
    if not summaries:
        print("OPEN: no wrath-buff-visual-summary in the current RuntimeQA log")
        return 0

    summary_index, summary = summaries[-1]
    pid = summary.group("pid")
    count = int(summary.group("count"))
    limit = int(summary.group("limit"))
    records = []
    for line in lines[summary_index + 1:]:
        match = RECORD.search(line)
        if match and match.group("pid") == pid:
            records.append(match)
            continue
        if "event=wrath-buff-visual-summary" in line:
            break

    if count < 19 or limit < 19 or len(records) < 19:
        print(f"OPEN: pid={pid} has {len(records)}/19 owner-tail records")
        return 0

    expected = []
    expected += [("lightmarks", -1, -1)] * 11
    expected += [("scatter", -1, -1)] * 2
    expected += [("flare", 0, 29), ("flare", 1, 38)]
    expected += [("mono", 0, 29), ("mono", 1, 38),
                 ("mono", 0, 29), ("mono", 1, 38)]
    for index, (match, (stage, attachment, bone)) in enumerate(zip(records, expected)):
        actual = (match.group("stage"), int(match.group("attachment")),
                  int(match.group("bone")))
        if actual != (stage, attachment, bone):
            print(f"FAIL: pid={pid} record {index} expected {stage}/{attachment}/{bone}, got {actual}")
            return 1
    print(f"PASS: pid={pid} first 19 Wrath owner submissions are lightmarks -> scatter -> flare -> mono")
    print("PASS: two flare records and four mono records use owner bones 29/38")
    print("Scope: submission telemetry only; pixels, cadence, blend and cleanup remain separate gates")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
