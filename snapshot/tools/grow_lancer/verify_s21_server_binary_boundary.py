#!/usr/bin/env python3
"""Record the protected S21 GameServer binary's symbol boundary, read-only."""

from __future__ import annotations

import hashlib
from pathlib import Path

import pefile


SERVER = Path(r"D:\GameServer S21\GameServer\IGC.GameServer2_R.exe")
EXPECTED_SHA256 = "134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74"
SKILL_NAMES = (
    b"Spin Step",
    b"Circle Shield",
    b"Obsidian",
    b"Magic Pin",
    b"Clash",
    b"Harsh Strike",
    b"Shining Peak",
    b"Wrath",
    b"Breche",
    b"Wind Soul",
)


def main() -> int:
    if not SERVER.is_file():
        raise SystemExit(f"FAIL missing {SERVER}")
    data = SERVER.read_bytes()
    actual = hashlib.sha256(data).hexdigest().upper()
    if actual != EXPECTED_SHA256:
        raise SystemExit(f"FAIL protected binary hash changed: {actual}")

    pe = pefile.PE(data=data, fast_load=False)
    exports = getattr(pe, "DIRECTORY_ENTRY_EXPORT", None)
    names = [] if exports is None else [entry.name for entry in exports.symbols if entry.name]
    if names != [b"OPENSSL_Applink"]:
        raise SystemExit(f"FAIL export boundary changed: {names!r}")
    if any(name in data for name in SKILL_NAMES):
        raise SystemExit("FAIL unexpected ASCII Grow Lancer skill name in protected binary")

    print("PASS protected S21 GameServer SHA-256 and export boundary (OPENSSL_Applink only)")
    print("PASS no ASCII Grow Lancer skill names/symbols in the supplied binary")
    print("Scope: binary boundary evidence only; numeric references are not handlers")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
