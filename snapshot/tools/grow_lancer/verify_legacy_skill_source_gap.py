#!/usr/bin/env python3
"""Pin the legacy MuServer52 skill-source gap without modifying that tree."""

from __future__ import annotations

import hashlib
from pathlib import Path
import re


ROOT = Path(r"D:\MU-CrossPlatform\MuServer52\Data\Skill")
EXPECTED = {
    "Skill.txt": "A7664F0A54D603EEE100C0BA1719C547B58F74EF1B4C3B384DB81DB06F24D314",
    "Skill_eng.txt": "3A9064BFB7C043683E6EB983108BAC2BA817DDE4D7DFFF3189B9D7B5F1F87922",
    "Skill_chs.txt": "7B914E78EA46B0378126D0465EACEF27A36F955A436ECEAC085606B24DBD5FD2",
    "SkillDamage.txt": "3881F2B64112CE7D68AD4A43ED9ED6A699824472CB714CE27E31604AD2F9D83F",
    "MasterSkillTree.txt": "9D9AD5CE5FCF09708503455D94751BEF4EFB37E9E5EE789FEE1F04247001E391",
}


def digest(path: Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest().upper()


def rows(text: str) -> set[int]:
    return {int(match.group(1)) for match in re.finditer(r"^\s*(27[1-9])\s+", text, re.MULTILINE)}


def main() -> int:
    paths = {name: ROOT / name for name in EXPECTED}
    for name, path in paths.items():
        if not path.is_file():
            raise SystemExit(f"FAIL missing {path}")
        actual = digest(path)
        if actual != EXPECTED[name]:
            raise SystemExit(f"FAIL hash changed {name}: {actual}")

    legacy = paths["Skill.txt"].read_text(encoding="utf-8", errors="replace")
    if rows(legacy) != set(range(271, 280)):
        raise SystemExit(f"FAIL legacy Skill.txt rows: {sorted(rows(legacy))}")
    header = legacy.splitlines()[0]
    if "DW" not in header or "RF" not in header or "GL" in header:
        raise SystemExit("FAIL legacy Skill.txt class header is not DW..RF-only")

    for name in ("Skill_eng.txt", "Skill_chs.txt"):
        if rows(paths[name].read_text(encoding="utf-8", errors="replace")):
            raise SystemExit(f"FAIL unexpected 271..279 rows in {name}")
    if paths["SkillDamage.txt"].stat().st_size != 131:
        raise SystemExit("FAIL SkillDamage.txt is no longer the pinned 131-byte stub")

    print("PASS pinned MuServer52 legacy source gap: Skill.txt rows 271..279 only")
    print("PASS language tables have no 271..279 rows; SkillDamage.txt is 131-byte stub")
    print("Scope: read-only legacy reference; no S21 handler/class authority inferred")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
