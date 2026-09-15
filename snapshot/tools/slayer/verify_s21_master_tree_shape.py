#!/usr/bin/env python3
"""Pin the read-only S21 Master Slayer BMD shape against the 5.2 loader.

This intentionally does not convert or rewrite either protected/source BMD.
The two 24-byte records have a different last-field meaning in S21 and 5.2;
a blind copy is not a usable native class-tree import.
"""

from __future__ import annotations

import hashlib
import struct
import xml.etree.ElementTree as ET
from pathlib import Path


S21 = Path(r"D:\MU FICA Season 21\Data\Local\masterskilltreedata.bmd")
RISE52 = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer\Client\Data\RISE\Config\MasterSkillTreeData.bmd")
S21_SERVER_TREE = Path(r"D:\GameServer S21\Data\Skills\SkillTreeData_3rd.xml")
S21_SKILL_LIST = Path(r"D:\GameServer S21\Data\Skills\SkillList.xml")
S21_TOOLTIP = Path(r"D:\MU FICA Season 21\Data\Local\masterskilltooltip.bmd")
RISE52_SERVER_TREE = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExGameServer\Tests\SlayerBuild\ServerStackSmoke\4.MuServer\Sub-1\Data\Skill\MasterSkillTree.txt")
S21_SHA = "A0381045194779304C7922685D1DDB9EA1E23DD0354FE02A36A9C91C8305D586"
RISE52_SHA = "D67B20890CBB2DCFF9FF9CAB670E30D51A3DD13C97B17D4C641676B43A7DDECC"
S21_SERVER_TREE_SHA = "CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0"
S21_SKILL_LIST_SHA = "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0"
S21_TOOLTIP_SHA = "76E07F264FCA9C20692EF8C1870B90C63B2C0EB0456E273D9DBBC6628BE73F0A"
RISE52_SERVER_TREE_SHA = "28D9C022DA82DB60C94137DA7EFBAFAF9AC9685D1A36E2FE29A00FBBFB878E81"
RECORD = struct.Struct("<HHBBBBiiiI")
BUX = (0xFC, 0xCF, 0xAB)


def rows(path: Path, expected_sha: str, expected_count: int):
    raw = path.read_bytes()
    digest = hashlib.sha256(raw).hexdigest().upper()
    if digest != expected_sha:
        raise AssertionError(f"{path}: hash drift {digest}")
    if len(raw) != RECORD.size * expected_count + 4:
        raise AssertionError(f"{path}: record count/size drift {len(raw)}")
    result = []
    for index in range(expected_count):
        offset = index * RECORD.size
        decoded = bytes(
            value ^ BUX[byte_index % len(BUX)]
            for byte_index, value in enumerate(raw[offset:offset + RECORD.size])
        )
        result.append(RECORD.unpack(decoded))
    return result


def main() -> None:
    s21 = rows(S21, S21_SHA, 2048)
    rise52 = rows(RISE52, RISE52_SHA, 512)
    if any(row[0] == 0 for row in rise52):
        raise AssertionError("5.2 512-record table unexpectedly has a free slot")
    slayer = [row for row in s21 if row[0] and row[1] & 512]
    if len(slayer) != 58 or {row[2] for row in slayer} != {0, 1, 2}:
        raise AssertionError("S21 Master Slayer three-category tree shape drifted")
    xml_bytes = S21_SERVER_TREE.read_bytes()
    if hashlib.sha256(xml_bytes).hexdigest().upper() != S21_SERVER_TREE_SHA:
        raise AssertionError("S21 server class-tree XML hash drifted")
    xml_root = ET.fromstring(xml_bytes)
    master_slayer = next((node for node in xml_root.findall("Class")
                          if node.attrib.get("ID") == "512"), None)
    if master_slayer is None:
        raise AssertionError("S21 server Master Slayer class 512 is absent")
    xml_nodes = []
    for tree in master_slayer.findall("Tree"):
        group = int(tree.attrib["Type"])
        for node in tree.findall("Skill"):
            xml_nodes.append((int(node.attrib["Index"]), group,
                              int(node.attrib["ReqMinPoint"]),
                              int(node.attrib["MaxPoint"]),
                              int(node.attrib["ParentSkill1"]),
                              int(node.attrib["ParentSkill2"]),
                              int(node.attrib["MagicNumber"])))
    bmd_nodes = {(row[0], row[2]): row for row in slayer}
    if len(xml_nodes) != 58 or len(bmd_nodes) != 58:
        raise AssertionError("S21 server/client class-tree node count drifted")
    for index, group, points, maximum, parent1, parent2, magic in xml_nodes:
        row = bmd_nodes.get((index, group))
        if row is None or (row[3], row[4], row[6], row[7], row[8]) != (
                points, maximum, parent1, parent2, magic):
            raise AssertionError(f"S21 server/client Master Slayer node mismatch at {index}/{group}")
    skill_list = S21_SKILL_LIST.read_bytes()
    if hashlib.sha256(skill_list).hexdigest().upper() != S21_SKILL_LIST_SHA:
        raise AssertionError("S21 SkillList hash drifted")
    damage = {int(node.attrib["Index"]): int(node.attrib["Damage"])
              for node in ET.fromstring(skill_list).findall("Skill")}
    for row in slayer:
        if damage.get(row[8]) != row[9]:
            raise AssertionError(f"S21 Master Slayer BMD default-value mismatch for skill {row[8]}")
    tooltip_raw = S21_TOOLTIP.read_bytes()
    if hashlib.sha256(tooltip_raw).hexdigest().upper() != S21_TOOLTIP_SHA or len(tooltip_raw) != 4096 * 404 + 4:
        raise AssertionError("S21 MasterSkillTooltip schema/hash drifted")
    tooltips = {}
    for record_index in range(4096):
        encrypted = tooltip_raw[record_index * 404:(record_index + 1) * 404]
        decoded = bytes(value ^ BUX[byte_index % len(BUX)]
                        for byte_index, value in enumerate(encrypted))
        skill, class_code = struct.unpack_from("<iH", decoded)
        if skill > 0 and class_code & 512:
            tooltips.setdefault(skill, (record_index, decoded))
    for row in slayer:
        if row[8] not in tooltips:
            raise AssertionError(f"S21 Master Slayer tooltip missing skill {row[8]}")
    for skill, expected_record in ((781, 854), (782, 856)):
        record, decoded = tooltips[skill]
        if record != expected_record or not decoded[6:70].startswith(b"Rank %d, Skill Level:") or not decoded[326:358].startswith(b"Required Points: %d"):
            raise AssertionError(f"S21 Master Slayer tooltip row drifted for skill {skill}")
    owners = {}
    for class_node in xml_root.findall("Class"):
        class_id = int(class_node.attrib["ID"])
        for tree in class_node.findall("Tree"):
            for node in tree.findall("Skill"):
                owners.setdefault(int(node.attrib["MagicNumber"]), set()).add(class_id)
    new_exclusive = {magic for magic, classes in owners.items()
                     if classes == {512} and magic >= 631}
    if new_exclusive != {631, *range(779, 795)}:
        raise AssertionError(f"S21-only Slayer master ID guard drifted: {sorted(new_exclusive)}")
    if any(owners[magic] == {512} for magic in (300, 325, 347)):
        raise AssertionError("shared legacy mastery rows became Slayer-exclusive")
    legacy_tree = RISE52_SERVER_TREE.read_bytes()
    if hashlib.sha256(legacy_tree).hexdigest().upper() != RISE52_SERVER_TREE_SHA:
        raise AssertionError("5.2 private server mastery tree hash drifted")
    legacy_ids = [int(line.split()[0]) for line in legacy_tree.decode("ascii").splitlines()
                  if line.strip() and line.split()[0].isdigit()]
    if legacy_ids.count(631) != 1 or any(value in legacy_ids for value in range(779, 795)):
        raise AssertionError("5.2 tree no longer has the known 631 collision/779..794 gap")
    strengthener = s21[653]
    mastery = s21[655]
    if strengthener[:10] != (58, 512, 1, 1, 20, 3, 0, 0, 781, 22):
        raise AssertionError(f"S21 781 UI row drifted: {strengthener}")
    if mastery[:10] != (62, 512, 1, 10, 10, 0, 781, 0, 782, 23):
        raise AssertionError(f"S21 782 UI row drifted: {mastery}")
    if rise52[0][9] != 1094755287:
        raise AssertionError("5.2 final field no longer has its known float bits")
    print("PASS: pinned S21/5.2 mastery BMD hashes and 24-byte Bux records")
    print("PASS: S21 Master Slayer class bit=512, 58 nodes, categories=0/1/2")
    print("PASS: all 58 S21 server XML nodes match client BMD slot/group/points/max/parents/magic")
    print("PASS: all 58 Slayer BMD final integers equal pinned SkillList Damage and have S21 404-byte tooltip records")
    print("PASS: S21 tooltip 781/782 rows=854/856 with native Rank/Required Points strings")
    print("PASS: S21 class-512-only IDs=631,779..794; legacy 5.2 already uses 631, so only 779..794 are safe ID-only guards")
    print("PASS: S21 781 slot=58 and 782 slot=62 parent=781, records 653/655")
    print("PASS: 5.2 loader has 512 occupied records; S21 uses 2048")
    print("NOTE: S21 last field is SkillList integer Damage (781=22,782=23); 5.2 parses a float DefValue, so convert values rather than copy bits")


if __name__ == "__main__":
    main()
