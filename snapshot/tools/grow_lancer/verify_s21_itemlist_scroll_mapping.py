"""Pin the authoritative S21 ItemList scroll rows without inventing learn paths.

The item table contains item identity and class/stat gates, not a skill ID
field.  Skill IDs are correlated by exact scroll name to the pinned SkillList
rows; the item.bmd records remain the independent group/index evidence.
"""
from pathlib import Path
import hashlib
import xml.etree.ElementTree as ET


SERVER_DATA = Path(r"D:\GameServer S21\Data")


def pinned(path: Path, digest: str) -> bytes:
    raw = path.read_bytes()
    assert hashlib.sha256(raw).hexdigest().upper() == digest.upper(), path
    return raw


def main() -> None:
    item_path = SERVER_DATA / "Items" / "ItemList.xml"
    item_raw = pinned(
        item_path,
        "61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78",
    )
    item_root = ET.fromstring(item_raw)
    section = item_root.find("./Section[@Index='12']")
    assert section is not None and section.get("Name") == "Pets and Rings and Misc"
    rows = {int(node.get("Index")): node for node in section.findall("./Item")}

    skill_path = SERVER_DATA / "Skills" / "SkillList.xml"
    skill_raw = pinned(
        skill_path,
        "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0",
    )
    skill_root = ET.fromstring(skill_raw)
    skills = {
        int(node.get("Index")): node
        for node in skill_root.findall("./Skill")
        if node.get("Index") in {"271", "272", "273", "274", "275", "276", "277", "278", "279"}
    }

    # ItemList has no Circle Shield or Clash scroll.  Keep those learn paths
    # absent instead of assigning an adjacent index.
    assert not any(
        node.get("Name") in {"Circle Shield Scroll", "Clash Scroll"}
        for node in section.findall("./Item")
    )

    # skill ID -> (item group, item index, item name, stat gates, drop level,
    # price).  Group 12 is the section's client group; item.bmd independently
    # pins these as records 2891..2897.
    expected = {
        271: (271, "Spin Step Scroll", {"ReqDexterity": "150"}, "53", "100000"),
        273: (272, "Obsidian Scroll", {"ReqEnergy": "200"}, "74", "120000"),
        274: (273, "Magic Pin Scroll", {"ReqStrength": "200", "ReqDexterity": "200"}, "80", "200000"),
        276: (274, "Harsh Strike Scroll", {"ReqStrength": "150"}, "58", "100000"),
        277: (275, "Shining Peak Scroll", {"ReqStrength": "600"}, "92", "300000"),
        278: (276, "Wrath Scroll", {"ReqStrength": "200", "ReqDexterity": "200"}, "66", "120000"),
        279: (277, "Breche Scroll", {"ReqStrength": "300", "ReqDexterity": "300"}, "92", "200000"),
    }
    for skill_id, (item_index, item_name, gates, drop_level, money) in expected.items():
        assert item_index in rows, item_index
        item = rows[item_index]
        assert item.get("Name") == item_name
        assert item.get("Slot") == "-1" and item.get("SkillIndex") == "0"
        assert item.get("Type") == "0" and item.get("Dump") == "1"
        assert item.get("ModelFile") == "SkillScroll.bmd"
        assert item.get("GrowLancer") == "1"
        assert item.get("Drop") == "1" and item.get("DropLevel") == drop_level
        assert item.get("Money") == money
        for field, value in gates.items():
            assert item.get(field) == value, (item_name, field)
        skill = skills[skill_id]
        assert skill.get("Name") == item_name.removesuffix(" Scroll")
        assert skill.get("GrowLancer") == "1"
        print(
            f"PASS skill {skill_id} {skill.get('Name')} -> group12/index{item_index} "
            f"{item_name}; class/stat/drop gate pinned"
        )

    assert set(expected) == {271, 273, 274, 276, 277, 278, 279}
    print("PASS 7/9 scroll mappings corroborated by ItemList.xml + SkillList.xml")
    print("PASS Circle Shield 272 and Clash 275 have no exact scroll row; learn path remains OPEN")
    print("Scope: table provenance only; no native class activation or server handler claim")


if __name__ == "__main__":
    main()
