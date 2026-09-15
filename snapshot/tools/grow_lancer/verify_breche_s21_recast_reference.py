"""Pin the owner-authorized S21 Breche recast reference capture.

This verifies capture provenance only.  It does not claim 5.2 pixel parity,
effect timing, or server-authoritative behavior.
"""

from __future__ import annotations

import csv
import hashlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CAPTURE = ROOT / "artifacts/runtime_qa/s21_reference/breche_owner_recast_20260915_b"
MANIFEST = CAPTURE / "frames.csv"
RESULT = CAPTURE / "capture_result.txt"
FRAME = CAPTURE / "frame_00042.jpg"


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> None:
    assert CAPTURE.is_dir(), CAPTURE
    assert RESULT.read_text(encoding="utf-8").strip() == "captured_frames=134 duration_ms=20054"
    assert sha256(MANIFEST) == (
        "A3EA97183DB454E93A6C99074EE5BE65ECAB70721E9935316B581DFC076CA899"
    )
    assert sha256(FRAME) == (
        "B115DF7DF5E335DC8A98A5FD3F2F8D9C0D15B184E2323CB5923599F27C969DEB"
    )

    with MANIFEST.open(newline="", encoding="utf-8-sig") as handle:
        rows = list(csv.DictReader(handle))
    assert len(rows) == 134, len(rows)
    row = rows[42]
    assert row["frame"] == "42"
    assert row["file"] == "frame_00042.jpg"
    assert row["utc_iso"] == "2026-09-14T17:01:33.6588555Z"
    assert all((CAPTURE / item["file"]).is_file() for item in rows)
    print("PASS S21 Breche owner recast provenance: 134 frames, pinned manifest and frame_00042")
    print("Scope: reference capture only; 5.2 owner/target visual parity remains OPEN")


if __name__ == "__main__":
    main()
