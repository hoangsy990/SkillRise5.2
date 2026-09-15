#!/usr/bin/env python3
"""Verify exact S21 Grow Lancer class-data leads without modifying S21 files."""

from __future__ import annotations

import hashlib
import re
from pathlib import Path
import xml.etree.ElementTree as ET


DATA = Path(r"D:\GameServer S21\Data")
PINNED = {
    DATA / "CalcCharacter.ini": (
        46416,
        "BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D",
    ),
    DATA / "StatSpecialize.xml": (
        11529,
        "D79F1CBBE8A6FD7207499CC5198E2F0E99A5BE06E2607C0A8B4879A3378E9AC9",
    ),
    DATA / "ClassQuest.xml": (
        25207,
        "5B2CE593AF8694F55061BAEFD95CC566E9CD4AAA2491CB0BB9819FF87201D6AC",
    ),
    DATA / "Warps" / "ClassQuest.xml": (
        24239,
        "20787D902A8B58208804E1989F6EE50AC301E689CCF167511964C16A448724F3",
    ),
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def ini_values(text: str, key: str) -> list[str]:
    pattern = re.compile(rf"^\s*{re.escape(key)}\s*=\s*([^;\r\n]+)", re.M)
    return [match.group(1).strip() for match in pattern.finditer(text)]


def main() -> None:
    for path, (size, expected_hash) in PINNED.items():
        assert path.is_file(), f"missing S21 source: {path}"
        assert path.stat().st_size == size, f"size changed: {path}"
        actual = sha256(path)
        assert actual == expected_hash, f"hash changed: {path} ({actual})"

    calc = (DATA / "CalcCharacter.ini").read_text(encoding="utf-8")
    expected = {
        "GLLevelUpPoint": ["7"],
        "GLLevelUp3rdTreePoint": ["1"],
        "GLLevelUp4thTreePoint": ["1"],
        "GLMaxAttackSpeed": ["273"],
        "GLDamageDevideToSD": ["90"],
        "GLSuccessAttackRateOption": ["20000"],
        "GLShieldGageConstA": ["12"],
        "GLShieldGageConstB": ["30"],
        "GLDamageStuckRate": ["10"],
    }
    for key, values in expected.items():
        assert ini_values(calc, key) == values, f"unexpected {key}"
    assert "555" in ini_values(calc, "GLPvMDamageRate"), "primary GL PvM rate missing"

    root = ET.parse(DATA / "StatSpecialize.xml").getroot()
    gl = next((node for node in root.findall("Class") if node.get("ID") == "8"), None)
    assert gl is not None, "S21 StatSpecialize class 8 missing"
    options = [
        (node.get("StatType"), node.get("Index"), node.get("StatLimit_1"),
         node.get("StatVal_1"), node.get("StatLimit_2"), node.get("StatVal_2"),
         node.get("StatLimit_3"), node.get("StatVal_3"))
        for node in gl.findall("Option")
    ]
    assert options == [
        ("1", "1", "1500", "10", "2000", "30", "3000", "45"),
        ("2", "4", "1500", "10", "2000", "30", "3000", "45"),
    ], "S21 class 8 specialize options changed"

    for path in (DATA / "ClassQuest.xml", DATA / "Warps" / "ClassQuest.xml"):
        text = path.read_text(encoding="utf-8")
        assert 'GrowLancer="' in text, f"GrowLancer column missing: {path}"

    print("PASS S21 Grow Lancer class-data hashes and exact CalcCharacter values")
    print("PASS StatSpecialize class 8 options: strength attack power + agility defense")
    print("PASS ClassQuest and Warps/ClassQuest expose GrowLancer columns")
    print("Scope: read-only S21 leads; SS6 class ABI and activation remain disabled")


if __name__ == "__main__":
    main()
