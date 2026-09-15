"""Bound one historical Spin QA cast; this is not current gameplay acceptance."""

from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
LOG = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
       "RuntimeQA" / "Client" / "GrowLancerRuntimeQA.log")
PID = "pid=28160 "
CAST = "2026-09-15T05:37:13.058Z"
FLUSH = "2026-09-15T05:37:14.038Z"
FLUSH_END = "2026-09-15T05:37:14.042Z"


def fields(line: str) -> dict[str, str]:
    return dict(re.findall(r"\b([A-Za-z][A-Za-z0-9-]*)=([^\s]+)", line))


def main() -> None:
    roots: list[str] = []
    draws: list[dict[str, str]] = []
    samples: list[dict[str, str]] = []
    outcomes: list[dict[str, str]] = []
    summary: list[dict[str, str]] = []
    # Stream the append-only log and scope all assertions to one PID/cast/flush.
    with LOG.open(encoding="utf-8", errors="replace") as handle:
        for line in handle:
            if PID not in line:
                continue
            stamp = line[:24]
            if stamp == CAST and "event=root-dispatched skill=271" in line:
                roots.append(line)
            if CAST <= stamp < FLUSH and "event=spin-blur-draw" in line:
                draws.append(fields(line))
            if FLUSH <= stamp < FLUSH_END:
                if "event=controller-samples skill=271" in line:
                    summary.append(fields(line))
                elif "event=controller-sample skill=271" in line:
                    samples.append(fields(line))
                elif "event=spin-outcome " in line:
                    outcomes.append(fields(line))
    assert len(roots) == 1 and "target=12" in roots[0]
    assert len(summary) == 1 and summary[0]["count"] == "21" and summary[0]["dropped"] == "0"
    assert len(samples) == 21 and [int(row["index"]) for row in samples] == list(range(21))
    assert all(row["target"] == "12" for row in samples)
    assert all(row["action"] == "285" and abs(float(row["actionSpeed"]) - 0.706) < 1e-5
               for row in samples[:20])
    assert float(samples[15]["frame"]) > 4.7 and samples[15]["stage"] == "0"
    assert 4.7 < float(samples[16]["frame"]) <= 6.5 and samples[16]["stage"] == "1"
    assert samples[20]["action"] == "11" and samples[20]["stage"] == "1"
    assert len(draws) == 20 and all(row["texture"] == "32019" and
                                      row["registered"] == "1" and
                                      row["owner"] == "0873D4AC" and
                                      row["action"] == "285" for row in draws)
    assert int(draws[0]["segments"]) == 2 and int(draws[15]["segments"]) == 30
    assert any(row["outcome"] == "1" and row["type"] == "10137" and
               row["frame"].startswith("4.706") and row["ms"] == "250"
               for row in outcomes)
    print("PASS historical Spin pid=28160 second cast target=12: "
          "action285 speed=0.706, frame4.706 stage0 -> frame5.001 stage1, "
          "20 stock-texture32019 blur draws, action exit later")
    print("OPEN current private-sampler gameplay pixels, S21 cadence equivalence, "
          "internal280/contact and owner visual acceptance")


if __name__ == "__main__":
    main()
