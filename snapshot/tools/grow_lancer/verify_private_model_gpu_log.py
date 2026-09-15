"""Audit one isolated hidden WGL run; this is not visual acceptance."""

import argparse
import re
from pathlib import Path


DEFAULT_LOG = (
    Path(__file__).resolve().parents[2]
    / "ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log"
)
EXPECTED = set(range(10119, 10137))


def fields(line: str) -> dict[str, str]:
    return dict(re.findall(r"\b(\w+)=([^\s]+)", line))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--pid", type=int, required=True)
    parser.add_argument("--log", type=Path, default=DEFAULT_LOG)
    args = parser.parse_args()
    rows: dict[int, dict[str, str]] = {}
    summaries: list[dict[str, str]] = []
    for line in args.log.read_text(encoding="utf-8", errors="replace").splitlines():
        values = fields(line)
        if values.get("pid") != str(args.pid):
            continue
        if values.get("event") == "private-model-gpu":
            model = int(values["type"])
            assert model not in rows, f"duplicate model {model}"
            rows[model] = values
        elif values.get("event") == "private-model-gpu-summary":
            summaries.append(values)
    assert set(rows) == EXPECTED, f"missing/extra model IDs {set(rows) ^ EXPECTED}"
    assert len(summaries) == 1, f"expected one summary, got {len(summaries)}"
    for model, values in rows.items():
        for key in ("opened", "texturesReady", "shaderUploaded"):
            assert values[key] == "1", f"model {model}: {key}={values[key]}"
        assert int(values["geometry"]) > 0, f"model {model}: no mesh geometry"
        assert values["glError"] == "0", f"model {model}: GL error"
    summary = summaries[0]
    for key, expected in (("expected", "18"), ("opened", "18"),
                          ("uploaded", "18"), ("releaseStable", "1")):
        assert summary[key] == expected, f"summary {key}={summary[key]}"
    print(f"PASS: pid {args.pid} all 18 registered visible BMDs GPU uploaded/released")
    print("Scope: hidden shader/texture/VAO plumbing only; no ingame pixels or GS authority")


if __name__ == "__main__":
    main()
