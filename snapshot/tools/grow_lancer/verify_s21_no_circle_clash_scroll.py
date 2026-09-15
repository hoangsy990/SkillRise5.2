"""Pin the S21 absence of a Circle Shield/Clash scroll row.

This is an evidence check, not a license to invent a learn path.  The two
skills are present in SkillList.xml with GrowLancer=1 and ItemSkill=0, while
the pinned ItemList has the seven named skill scrolls but no Circle Shield or
Clash scroll.  A server-side class/default or siege learn route remains a
separate unresolved contract.
"""

from __future__ import annotations

import hashlib
import re
from pathlib import Path


S21 = Path(r"D:\GameServer S21\Data")
SKILLS = S21 / "Skills/SkillList.xml"
ITEMS = S21 / "Items/ItemList.xml"
TREE = S21 / "Skills/SkillTreeData_3rd.xml"
BUFFS = S21 / "BuffEffectManager.xml"

PINNED = {
    SKILLS: "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0",
    ITEMS: "61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78",
    TREE: "CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0",
    BUFFS: "201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0",
}


def read_pinned(path: Path) -> str:
    raw = path.read_bytes()
    expected = PINNED[path]
    actual = hashlib.sha256(raw).hexdigest().upper()
    assert actual == expected, f"hash changed: {path} ({actual})"
    return raw.decode("utf-8-sig", errors="strict")


def skill_row(skill_text: str, skill_id: int) -> str:
    match = re.search(rf'<Skill\b[^>]*\bIndex="{skill_id}"[^>]*/>', skill_text)
    assert match, f"missing SkillList row {skill_id}"
    return match.group(0)


def main() -> None:
    skill_text = read_pinned(SKILLS)
    item_text = read_pinned(ITEMS)
    tree_text = read_pinned(TREE)
    buff_text = read_pinned(BUFFS)

    circle = skill_row(skill_text, 272)
    clash = skill_row(skill_text, 275)
    for row, name in ((circle, "Circle Shield"), (clash, "Clash")):
        assert f'Name="{name}"' in row
        assert 'GrowLancer="1"' in row
        assert 'ItemSkill="0"' in row
    assert 'BuffIndex="216"' in circle
    assert 'Brand="75"' in clash

    # The item catalog's skill-scroll names are exact strings; neither skill
    # has a named scroll row or a different alias in this pinned S21 table.
    assert not re.search(r"(?i)(circle\s*shield|clash)\s+scroll", item_text)
    for expected in (
        "Spin Step Scroll",
        "Obsidian Scroll",
        "Magic Pin Scroll",
        "Harsh Strike Scroll",
        "Shining Peak Scroll",
        "Wrath Scroll",
        "Breche Scroll",
    ):
        assert expected in item_text, expected

    # Circle has master rows; Clash has no Circle/Clash master row in this
    # third-tree table.  This narrows the source without inventing learning.
    assert "Circle Shield PowUp" in tree_text
    assert "Circle Shield Mastery" in tree_text
    assert "Clash" not in tree_text
    assert 'Index="216"' in buff_text

    print("PASS: S21 SkillList rows 272/275 are GrowLancer-only with ItemSkill=0")
    print("PASS: pinned ItemList contains seven named GL skill scrolls, no Circle/Clash scroll")
    print("PASS: Circle master rows are present; Clash has no third-tree master row")
    print("Scope: absence evidence only; class/default/siege learn and server authority remain OPEN")


if __name__ == "__main__":
    main()
