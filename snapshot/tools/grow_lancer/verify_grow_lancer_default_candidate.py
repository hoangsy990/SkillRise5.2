"""Record a newer-source Grow Lancer default row as a non-authoritative lead.

The S21 dump supplied for this task does not contain the SQL
MuOnline.dbo.DefaultClassType row.  This check therefore never feeds the row
into RISE and never changes class activation; it only makes the candidate
explicit so it cannot be mistaken for S21 proof.
"""
from pathlib import Path
import hashlib
import re


S21 = Path(r"D:\GameServer S21\Data")
NEWER_DEFAULTS = Path(r"D:\MU-CrossPlatform\MuServer52\Data\Character\DefaultClassInfo.txt")


def pinned(path: Path, digest: str) -> bytes:
    raw = path.read_bytes()
    assert hashlib.sha256(raw).hexdigest().upper() == digest.upper(), path
    return raw


def main() -> None:
    calc = pinned(
        S21 / "CalcCharacter.ini",
        "BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D",
    ).decode("utf-8-sig")
    for key, value in {
        "GLLevelUpPoint": "7",
        "GLLevelUp3rdTreePoint": "1",
        "GLLevelUp4thTreePoint": "1",
        "GLMaxAttackSpeed": "273",
    }.items():
        assert re.search(rf"^{re.escape(key)}\s*=\s*{re.escape(value)}\s*$", calc, re.M)
    assert "MuOnline.dbo.DefaultClassType" in calc
    print("PASS S21 CalcCharacter.ini GL settings and DB-default delegation pinned")

    if not NEWER_DEFAULTS.exists():
        print("OPEN newer DefaultClassInfo candidate is unavailable")
        return
    raw = pinned(
        NEWER_DEFAULTS,
        "F0FE413F2E239F96754F6F7109E65E17E2FD0E431246D24733F06DAEFA5A4849",
    ).decode("utf-8-sig")
    rows = []
    for line in raw.splitlines():
        line = line.strip()
        if not line or line.startswith("//") or line == "end":
            continue
        fields = line.split()
        assert len(fields) == 12, fields
        rows.append(fields)
    assert len(rows) == 15
    gl = next(row for row in rows if row[0] == "7")
    assert gl == ["7", "30", "30", "25", "24", "0", "110", "109", "2.0", "1.0", "2.0", "1.0"]
    print("REFERENCE PASS newer MuServer52 row7 candidate: 30/30/25/24, HP110, MP109, 2.0/1.0/2.0/1.0")
    print("REFERENCE SCOPE: candidate only; no S21 DefaultClassType proof and no activation")
    print("OPEN: obtain/verify S21 DB DefaultClassType row before using these values")


if __name__ == "__main__":
    main()
