#!/usr/bin/env python3
"""Read-only S21 Master Slayer versus frozen 5.2 GS row-shape comparison."""

from __future__ import annotations

import hashlib
import xml.etree.ElementTree as ET
from pathlib import Path

from verify_s21_master_tree_shape import (
    S21, S21_SHA, S21_SKILL_LIST, S21_SKILL_LIST_SHA, rows,
)

LEGACY = Path(
    r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExGameServer\Tests"
    r"\SlayerBuild\ServerStackSmoke\4.MuServer\Sub-1\Data\Skill"
    r"\MasterSkillTree.txt"
)
LEGACY_SHA = "28D9C022DA82DB60C94137DA7EFBAFAF9AC9685D1A36E2FE29A00FBBFB878E81"


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def main() -> None:
    legacy_bytes = LEGACY.read_bytes()
    skill_bytes = S21_SKILL_LIST.read_bytes()
    if digest(legacy_bytes) != LEGACY_SHA or digest(skill_bytes) != S21_SKILL_LIST_SHA:
        raise AssertionError("frozen GS/S21 SkillList input drifted")
    ranks = {int(node.attrib["Index"]): int(node.attrib["Rank"])
             for node in ET.fromstring(skill_bytes).findall("Skill")}
    legacy: dict[int, tuple[int, ...]] = {}
    for text in legacy_bytes.decode("latin1").splitlines():
        fields = text.split()
        if not fields or not fields[0].isdigit():
            continue
        if len(fields) < 29:
            raise AssertionError(f"short legacy MasterSkillTree row: {text[:80]}")
        skill = int(fields[0])
        legacy[skill] = tuple(int(fields[index]) for index in (1, 2, 3, 4, 27, 28))
    compatible: list[int] = []
    missing: list[int] = []
    different: list[tuple[int, tuple[int, ...], tuple[int, ...]]] = []
    native = [row for row in rows(S21, S21_SHA, 2048)
              if row[0] and row[1] & 512]
    for row in native:
        skill = row[8]
        # SS21 BMD/XML groups are 0..2; 5.2 GS text stores 1..3.
        expected = (row[2] + 1, ranks[skill], row[3], row[4], row[6], row[7])
        found = legacy.get(skill)
        if found is None:
            missing.append(skill)
        elif found == expected:
            compatible.append(skill)
        else:
            different.append((skill, expected, found))
    if len(native) != 58 or 781 not in missing or 782 not in missing:
        raise AssertionError("S21 full tree/prerequisite inventory drifted")
    print(f"S21 Master Slayer nodes=58 compatible_5.2={len(compatible)} "
          f"missing_5.2={len(missing)} different_shape={len(different)}")
    print("compatible:", sorted(compatible))
    print("missing:", sorted(missing))
    print("different IDs:", sorted(skill for skill, *_ in different))
    print("collision 631: S21 class-512-only; legacy ID row shape may match, "
          "but RequireClass/option provenance is not proved")
    print("NOTE: matching shape is not proof of matching per-point MainValue/options")


if __name__ == "__main__":
    main()
