#!/usr/bin/env python3
"""Fail-closed verifier for the accepted Grow Lancer S21 server tables."""

from __future__ import annotations

import hashlib
import re
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


ROOT = Path(r"C:\Users\DELL\Desktop\Skills")
ITEM_BMD = Path(r"D:\MU FICA Season 21\Data\Local\item.bmd")
ITEM_BMD_HASH = "9B9D62B7F3BA085D31B4AEFBCC9DE5DB505C48D45C336A18CAD424C93D3B02F6"
HASHES = {
    "SkillList.xml": "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0",
    "RegularSkillCalc.lua": "F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847",
    "MasterSkillCalc_3rd.lua": "771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699",
    "SkillSettings.ini": "7CF1FF432D337EF3420B8037C2D26F95D768E317A0DFC6CA7E46C4EAF2F847C9",
    "SkillAOETargetting.xml": "CE5C501CB82D2179DB5ED5407DC89BEA4CB118446BA444D8FF95D7DE96F984D9",
    "Skill_UseArea.xml": "7C6B0C57A33F2E288AF106D8E19D43D9093279D01CE9DFB9C0E03EB62621BFC2",
    "SkillRequire.xml": "4E6B765FF7DCFB62E58AD9720F5E8C8E81DC8ECC5BA30C3BC3B842B390C9AC58",
    "SkillElect.hit": "809B8D5724DB063EB497C09722B894E1BEB6A66D7103FD075B9331858455BE10",
    "SkillTreeData_3rd.xml": "CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0",
}

ROWS = {
    271: ("Spin Step", 83, 100, 12, 0, 2, 500),
    272: ("Circle Shield", 220, 0, 100, 50, 0, 0),
    273: ("Obsidian", 74, 0, 50, 50, 0, 0),
    274: ("Magic Pin", 30, 80, 5, 3, 2, 500),
    275: ("Clash", 0, 50, 50, 50, 6, 10000),
    276: ("Harsh Strike", 74, 100, 12, 0, 3, 500),
    277: ("Shining Peak", 92, 50, 8, 5, 4, 0),
    278: ("Wrath", 66, 0, 40, 30, 0, 0),
    279: ("Breche", 300, 230, 15, 6, 5, 0),
}

SCROLL_ROWS = {
    2891: (12, 271, "Spin Step Scroll"),
    2892: (12, 272, "Obsidian Scroll"),
    2893: (12, 273, "Magic Pin Scroll"),
    2894: (12, 274, "Harsh Strike Scroll"),
    2895: (12, 275, "Shining Peak Scroll"),
    2896: (12, 276, "Wrath Scroll"),
    2897: (12, 277, "Breche Scroll"),
}

MASTER_ROWS = {
    # id: (name, brand, mana, BP, range, delay, buff, parent, min/max, Lua function)
    687: ("Spin Step Strengthener", 271, 14, 0, 2, 500, 0, 0, 1, 20,
          "GrowLancerSpinStep_Level1"),
    690: ("Spin Step Mastery", 687, 16, 0, 2, 500, 0, 687, 1, 20,
          "GrowLancerSpinStep_Level2"),
    688: ("Harsh Strike Strengthener", 276, 14, 0, 3, 500, 0, 0, 1, 20,
          "GrowLancerHarshStrike_Level1"),
    691: ("Harsh Strike Mastery", 688, 16, 0, 3, 500, 0, 688, 10, 10,
          "GrowLancerHarshStrike_Level2"),
    692: ("Magic Pin Strengthener", 274, 17, 10, 2, 500, 0, 0, 1, 20,
          "GrowLancerMagicPin_Level1"),
    695: ("Magic Pin Mastery", 692, 20, 13, 3, 500, 0, 692, 10, 10,
          "GrowLancerMagicPin_Level2"),
    693: ("Obsidian Strengthener", 273, 50, 50, 0, 0, 220, 0, 1, 20,
          "GrowLancerObsidian_Level1"),
    696: ("Breche Strengthener", 279, 16, 10, 5, 0, 0, 0, 1, 20,
          "GrowLancerBreche_Level1"),
    698: ("Breche Mastery", 696, 16, 10, 6, 0, 0, 696, 10, 10,
          "GrowLancerBreche_Level2"),
    699: ("Shining Peak Strengthener", 277, 10, 7, 4, 0, 0, 698, 1, 20,
          "GrowLancerShiningPeak_Level1"),
    703: ("Circle Shield PowUp", 272, 100, 50, 0, 0, 221, 0, 1, 20, None),
    706: ("Circle Shield Mastery", 703, 100, 50, 0, 0, 222, 703, 1, 20, None),
    895: ("Wrath Strengthener", 278, 50, 40, 0, 0, 425, 0, 1, 10, None),
}


def fail(message: str) -> None:
    raise AssertionError(message)


def main() -> int:
    for name, expected in HASHES.items():
        path = ROOT / name
        actual = hashlib.sha256(path.read_bytes()).hexdigest().upper()
        if actual != expected:
            fail(f"hash mismatch: {name}: {actual}")

    tree = ET.parse(ROOT / "SkillList.xml")
    by_id = {int(node.attrib["Index"]): node.attrib for node in tree.iter("Skill")}
    for skill_id, expected in ROWS.items():
        row = by_id[skill_id]
        actual = (
            row["Name"], int(row["ReqLevel"]), int(row["Damage"]),
            int(row["ManaUsage"]), int(row["BPUsage"]),
            int(row["Distance"]), int(row["Delay"]),
        )
        if actual != expected:
            fail(f"row mismatch for {skill_id}: {actual!r}")
        if row["GrowLancer"] != "1" or row["ItemSkill"] != "0":
            fail(f"class/item flag mismatch for {skill_id}")
        other_classes = (
            "DarkWizard", "DarkKnight", "FairyElf", "MagicGladiator",
            "DarkLord", "Summoner", "RageFighter", "RuneWizard", "Slayer",
            "GunCrusher", "LightWizard", "LemuriaMage", "IllusionKnight",
            "Alchemist", "Crusader",
        )
        if any(row[name] != "0" for name in other_classes):
            fail(f"unexpected alternate class flag for {skill_id}")

    if by_id[280]["Name"] != "Explosion" or by_id[280]["UseType"] != "6":
        fail("Spin Step internal Explosion contract changed")
    if by_id[281]["Name"] != "Magic Pin Explosion" or by_id[281]["UseType"] != "6":
        fail("Magic Pin internal Explosion contract changed")

    master_tree = {
        int(node.attrib["MagicNumber"]): node.attrib
        for node in ET.parse(ROOT / "SkillTreeData_3rd.xml").iter("Skill")
        if int(node.attrib.get("MagicNumber", -1)) in MASTER_ROWS
    }
    master_lua = (ROOT / "MasterSkillCalc_3rd.lua").read_text(
        encoding="utf-8-sig")
    for skill_id, expected in MASTER_ROWS.items():
        name, brand, mana, bp, distance, delay, buff, parent, minimum, maximum, function = expected
        row = by_id[skill_id]
        actual = (
            row["Name"], int(row["Brand"]), int(row["ManaUsage"]),
            int(row["BPUsage"]), int(row["Distance"]), int(row["Delay"]),
            int(row["BuffIndex"]),
        )
        if actual != (name, brand, mana, bp, distance, delay, buff):
            fail(f"master SkillList row mismatch for {skill_id}: {actual!r}")
        tree_row = master_tree[skill_id]
        tree_actual = (
            tree_row["Name"], int(tree_row["ParentSkill1"]),
            int(tree_row["ReqMinPoint"]), int(tree_row["MaxPoint"]),
        )
        if tree_actual != (name, parent, minimum, maximum):
            fail(f"active master tree row mismatch for {skill_id}: {tree_actual!r}")
        if function and not re.search(rf"(?m)^function\s+{function}\s*\(", master_lua):
            fail(f"missing pinned master formula function for {skill_id}: {function}")
    if set(master_tree) != set(MASTER_ROWS):
        fail(f"active master row set changed: {sorted(master_tree)}")
    if {708, 709, 710} & set(master_tree):
        fail("legacy Wrath chain unexpectedly entered the active tree")

    settings = (ROOT / "SkillSettings.ini").read_text(encoding="utf-8-sig")
    for key, value in (
        ("CircleShieldDecreaseAG", 10),
        ("CircleShieldTime", 30),
        ("CircleShieldRate", 20),
    ):
        if not re.search(rf"(?m)^\s*{key}\s*=\s*{value}\s*$", settings):
            fail(f"missing setting {key}={value}")

    base_ids = set(ROWS)
    aoe_ids = {int(n.attrib["Index"]) for n in ET.parse(ROOT / "SkillAOETargetting.xml").iter("Skill")}
    area_ids = {int(n.attrib["ID"]) for n in ET.parse(ROOT / "Skill_UseArea.xml").iter("Skill")}
    if base_ids & aoe_ids:
        fail(f"unexpected AOE targeting rows: {sorted(base_ids & aoe_ids)}")
    if base_ids & area_ids:
        fail(f"unexpected use-area rows: {sorted(base_ids & area_ids)}")

    hit = (ROOT / "SkillElect.hit").read_bytes()
    if len(hit) != 36 * 21 * 21:
        fail(f"SkillElect.hit size mismatch: {len(hit)}")

    item_data = ITEM_BMD.read_bytes()
    if hashlib.sha256(item_data).hexdigest().upper() != ITEM_BMD_HASH:
        fail("S21 item.bmd hash mismatch")
    record_count = int.from_bytes(item_data[:4], "little")
    if record_count != 4950 or (len(item_data) - 8) % record_count:
        fail("S21 item.bmd header/record size mismatch")
    record_size = (len(item_data) - 8) // record_count
    if record_size != 708:
        fail(f"S21 item.bmd record size mismatch: {record_size}")
    key = (0xFC, 0xCF, 0xAB)
    all_names = []
    for record_index in range(record_count):
        start = 4 + record_index * record_size
        encrypted = item_data[start:start + record_size]
        record = bytes(value ^ key[offset % 3]
                       for offset, value in enumerate(encrypted))
        all_names.append(record[528:608].split(b"\0", 1)[0]
                         .decode("ascii", errors="ignore"))
    for record_index, expected in SCROLL_ROWS.items():
        start = 4 + record_index * record_size
        encrypted = item_data[start:start + record_size]
        record = bytes(value ^ key[offset % 3]
                       for offset, value in enumerate(encrypted))
        actual = (
            int.from_bytes(record[4:6], "little"),
            int.from_bytes(record[6:8], "little"),
            record[528:608].split(b"\0", 1)[0].decode("ascii"),
        )
        if actual != expected:
            fail(f"item.bmd record {record_index} mismatch: {actual!r}")
    if any(term in name for term in ("Circle Shield", "Clash")
           for name in all_names):
        fail("unexpected Circle Shield/Clash scroll attribution")

    print("PASS: 9 base rows, 2 internal rows, 3 Circle Shield settings")
    print("PASS: 13 active third-master rows and formula-presence contracts")
    print("PASS: pinned hashes and optional-system absences")
    print("PASS: SkillElect.hit is 36 x 21 x 21 bytes")
    print("PASS: 7 hash-pinned S21 item.bmd scroll records")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (AssertionError, KeyError, OSError, ET.ParseError) as exc:
        print(f"FAIL: {exc}", file=sys.stderr)
        raise SystemExit(1)
