"""Verify the supplied Season 21 server-data boundary read-only.

This is provenance/contract evidence only.  It deliberately refuses to treat
the protected GameServer executable as source and does not invent a handler or
class ABI when no matching source/symbol dump is present.
"""

from pathlib import Path
import hashlib
import xml.etree.ElementTree as ET


ROOT = Path(r"D:\GameServer S21")
DATA = ROOT / "Data"
SERVER = ROOT / "GameServer"

# Keep the expected values explicit and readable.
HASHES = {
    DATA / "Skills" / "SkillList.xml": "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0",
    DATA / "Skills" / "SkillRequire.xml": "4E6B765FF7DCFB62E58AD9720F5E8C8E81DC8ECC5BA30C3BC3B842B390C9AC58",
    DATA / "Skills" / "SkillSettings.ini": "7CF1FF432D337EF3420B8037C2D26F95D768E317A0DFC6CA7E46C4EAF2F847C9",
    DATA / "Skills" / "SkillTreeData_3rd.xml": "CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0",
    DATA / "Skills" / "RegularSkillCalc.lua": "F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847",
    DATA / "Skills" / "MasterSkillCalc_3rd.lua": "771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699",
    DATA / "Skills" / "MasterSkillCalc_5th.lua": "E9BB95B67F63D4D3CD3DC7C28B33CD9A1DB7D9F35E5C75BD65445F0E4529BE8A",
    DATA / "Items" / "ItemList.xml": "61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78",
    DATA / "Scripts" / "Character" / "CalcCharacter.lua": "9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70",
    DATA / "CalcCharacter.ini": "BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D",
    DATA / "StatSpecialize.xml": "D79F1CBBE8A6FD7207499CC5198E2F0E99A5BE06E2607C0A8B4879A3378E9AC9",
    DATA / "BuffEffectManager.xml": "201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0",
}
PROTECTED_SERVER_BINARY = SERVER / "IGC.GameServer2_R.exe"
PROTECTED_SERVER_BINARY_SHA256 = "134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74"

SKILL_IDS = tuple(range(271, 280))
SKILL_EXPECTED = {
    271: ("Spin Step", "83", "100", "12", "0", "2", "500"),
    272: ("Circle Shield", "220", "0", "100", "50", "0", "0"),
    273: ("Obsidian", "74", "0", "50", "50", "0", "0"),
    274: ("Magic Pin", "30", "80", "5", "3", "2", "500"),
    275: ("Clash", "0", "50", "50", "50", "6", "10000"),
    276: ("Harsh Strike", "74", "100", "12", "0", "3", "500"),
    277: ("Shining Peak", "92", "50", "8", "5", "4", "0"),
    278: ("Wrath", "66", "0", "40", "30", "0", "0"),
    279: ("Breche", "300", "230", "15", "6", "5", "0"),
}


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> None:
    assert ROOT.is_dir(), ROOT
    for path, expected in HASHES.items():
        assert path.is_file(), path
        actual = digest(path)
        assert actual == expected, (path, actual, expected)
    print(f"PASS pinned S21 server-data hashes ({len(HASHES)} files)")
    assert PROTECTED_SERVER_BINARY.is_file(), PROTECTED_SERVER_BINARY
    assert digest(PROTECTED_SERVER_BINARY) == PROTECTED_SERVER_BINARY_SHA256
    print("PASS protected S21 GameServer binary hash pinned")

    skill_root = ET.fromstring((DATA / "Skills" / "SkillList.xml").read_bytes())
    rows = {int(node.get("Index")): node for node in skill_root.findall("./Skill")}
    for skill_id in SKILL_IDS:
        node = rows[skill_id]
        expected = SKILL_EXPECTED[skill_id]
        actual = (
            node.get("Name"),
            node.get("ReqLevel"),
            node.get("Damage"),
            node.get("ManaUsage"),
            node.get("BPUsage"),
            node.get("Distance"),
            node.get("Delay"),
        )
        assert actual == expected, (skill_id, actual, expected)
        assert node.get("GrowLancer") == "1"
    print("PASS exact Grow Lancer base rows 271..279")

    settings = (DATA / "Skills" / "SkillSettings.ini").read_text(encoding="utf-8")
    for key, value in (("CircleShieldDecreaseAG", "10"), ("CircleShieldTime", "30"), ("CircleShieldRate", "20")):
        assert f"{key} = {value}" in settings
    print("PASS Circle Shield settings 10/30/20")

    master = (DATA / "Skills" / "MasterSkillCalc_5th.lua").read_text(encoding="utf-8")
    start = master.index("function WindSoul_5thEnchant_Lord")
    body = master[start : master.index("function FireBeast_5thEnchant_Summoner", start)]
    assert body.count("OutDamage = InDamage") == 5
    assert "BarrageCount == 5" in body
    print("PASS Wind Soul 5th-master formula (five full-damage barrages)")

    source_exts = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".pdb", ".map", ".dmp", ".mdmp", ".dbg", ".sln", ".vcxproj"}
    server_files = [p for p in SERVER.rglob("*") if p.is_file() and p.suffix.lower() in source_exts]
    assert not server_files, server_files
    print("PASS GameServer tree has no source/symbol/project dump; binary remains protected")
    print("Scope: data contracts only; live handler/class/authority remains OPEN")


if __name__ == "__main__":
    main()
