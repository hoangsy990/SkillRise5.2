"""Verify staged Pegasus sound files against the compiled provenance catalog."""

from __future__ import annotations

import hashlib
import re
import sys
import wave
from pathlib import Path


ROW = re.compile(
    r'\{\d+,0x[0-9A-Fa-f]+,HighSkillSoundRole::\w+,"([^"]+)","([0-9A-Fa-f]{64})"\}'
)


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: VerifyPegasusSoundAssets.py STAGED_SOUND_ROOT")
    root = Path(sys.argv[1]).resolve()
    catalog = Path(__file__).resolve().parents[2] / "Shared" / "PegasusHighSkillSoundCatalog.h"
    rows = ROW.findall(catalog.read_text(encoding="utf-8"))
    failures: list[str] = []
    if len(rows) != 23:
        failures.append(f"catalog row count is {len(rows)}, expected 23")
    for relative, expected_hash in rows:
        path = root / Path(relative.replace("\\", "/"))
        if not path.is_file():
            failures.append(f"missing: {relative}")
            continue
        actual_hash = hashlib.sha256(path.read_bytes()).hexdigest().upper()
        if actual_hash != expected_hash.upper():
            failures.append(f"hash mismatch: {relative}")
            continue
        try:
            with wave.open(str(path), "rb") as stream:
                if (stream.getnchannels() not in (1, 2) or
                        stream.getsampwidth() not in (1, 2) or
                        stream.getframerate() <= 0 or stream.getnframes() <= 0):
                    failures.append(f"invalid PCM parameters: {relative}")
        except (wave.Error, EOFError) as error:
            failures.append(f"invalid WAVE header: {relative}: {error}")
    if failures:
        print("FAIL: Pegasus staged sound audit")
        for failure in failures:
            print(f"  {failure}")
        return 1
    print("PASS: 23 staged Pegasus WAV files match catalog hashes and PCM headers")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
