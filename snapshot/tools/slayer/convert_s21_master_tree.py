#!/usr/bin/env python3
"""Build isolated 5.2 Master Slayer tree/tooltip overlays from pinned S21 data.

S21 inputs are read-only. This converts the complete 58-node class-512 tree,
not merely the Pierce 781/782 prerequisite, into private 5.2 record layouts.
It does not install or load these files into the shared 5.2 client.
"""

from __future__ import annotations

import hashlib
import struct
import xml.etree.ElementTree as ET
import zlib
from pathlib import Path

from verify_s21_master_tree_shape import (
    BUX, S21, S21_SHA, S21_SKILL_LIST, S21_SKILL_LIST_SHA,
    S21_TOOLTIP, S21_TOOLTIP_SHA, rows,
)


TARGET = Path(
    r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests"
    r"\SlayerBuild\Client\Data\RISE\Slayer\Config"
)
S21_ICON = Path(r"D:\MU FICA Season 21\Data\Interface\new_Master_Icon.OZJ")
S21_ICON_SHA = "DF3D1F863741E720EFC7B9ECC90117BB1BE9CA49852A86420FA8CA3E396D9F74"
TREE_RECORD = struct.Struct("<HHBBBBiiif")  # 5.2 _MASTER_SKILLTREE_DATA, 24 bytes
S21_TOOLTIP_RECORD = struct.Struct("<iH64s256s32s46s")  # native S21, 404 bytes
RISE_TOOLTIP_RECORD = struct.Struct("<iH64s256s32s64s64s64s64s2x")  # 5.2, 616 bytes
RISE_SKILL_RECORD = struct.Struct("<H32sHHHHIiiHBIBHBiiBBH")  # private 77-byte metadata
if TREE_RECORD.size != 24 or S21_TOOLTIP_RECORD.size != 404 or RISE_TOOLTIP_RECORD.size != 616:
    raise AssertionError("master-tree record layout drifted")


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def bux(data: bytes) -> bytes:
    return bytes(value ^ BUX[index % 3] for index, value in enumerate(data))


def packet(records: list[bytes], size: int) -> bytes:
    if len(records) != 58 or any(len(record) != size for record in records):
        raise AssertionError("private Master Slayer record count/size drifted")
    plaintext = b"".join(records)
    encrypted = b"".join(bux(record) for record in records)
    return encrypted + struct.pack("<I", zlib.crc32(plaintext))


def source_tooltips() -> dict[int, tuple[int, bytes]]:
    raw = S21_TOOLTIP.read_bytes()
    if digest(raw) != S21_TOOLTIP_SHA or len(raw) != 4096 * 404 + 4:
        raise AssertionError("native S21 tooltip hash/schema drifted")
    result: dict[int, tuple[int, bytes]] = {}
    for index in range(4096):
        plain = bux(raw[index * 404:(index + 1) * 404])
        skill, class_code, *_ = S21_TOOLTIP_RECORD.unpack(plain)
        if skill > 0 and class_code & 512:
            result.setdefault(skill, (index, plain))
    return result


def build() -> tuple[bytes, bytes, bytes]:
    s21_rows = [row for row in rows(S21, S21_SHA, 2048)
                if row[0] and row[1] & 512]
    if len(s21_rows) != 58 or len({row[8] for row in s21_rows}) != 58:
        raise AssertionError("native Master Slayer 58-node tree drifted")
    skill_list = S21_SKILL_LIST.read_bytes()
    if digest(skill_list) != S21_SKILL_LIST_SHA:
        raise AssertionError("S21 SkillList hash drifted")
    skill_nodes = {int(node.attrib["Index"]): node.attrib
                   for node in ET.fromstring(skill_list).findall("Skill")}
    damage = {skill: int(node["Damage"]) for skill, node in skill_nodes.items()}
    native_tooltips = source_tooltips()
    tree_records: list[bytes] = []
    tooltip_records: list[bytes] = []
    skill_records: list[bytes] = []
    for row in s21_rows:
        (slot, _class_code, group, min_points, max_points, arrow,
         parent1, parent2, skill, default_integer) = row
        if damage.get(skill) != default_integer or skill not in native_tooltips:
            raise AssertionError(f"S21 Slayer default/tooltip mismatch at skill {skill}")
        tree_records.append(TREE_RECORD.pack(
            slot, 512, group, min_points, max_points, arrow,
            parent1, parent2, skill, float(default_integer),
        ))
        source_index, native = native_tooltips[skill]
        source_skill, source_class, info1, info2, info3, info4 = (
            S21_TOOLTIP_RECORD.unpack(native)
        )
        if source_skill != skill or not (source_class & 512):
            raise AssertionError(f"S21 tooltip provenance drifted at {source_index}")
        tooltip_records.append(RISE_TOOLTIP_RECORD.pack(
            skill, 512, info1, info2, info3, info4.ljust(64, b"\0"),
            b"\0" * 64, b"\0" * 64, b"\0" * 64,
        ))
        source_skill = skill_nodes[skill]
        if source_skill["Slayer"] != "3":
            raise AssertionError(f"S21 master skill {skill} lost class-9 provenance")
        name = source_skill["Name"].encode("utf-8")[:31].ljust(32, b"\0")
        skill_records.append(RISE_SKILL_RECORD.pack(
            skill, name,
            int(source_skill["ReqLevel"]), int(source_skill["Damage"]),
            int(source_skill["ManaUsage"]), int(source_skill["BPUsage"]),
            int(source_skill["Distance"]), int(source_skill["Delay"]),
            int(source_skill["ReqEnergy"]), int(source_skill["ReqCommand"]),
            int(source_skill["UseType"]), int(source_skill["Brand"]),
            int(source_skill["Rank"]), int(source_skill["Group"]),
            int(source_skill["Type"]) & 0xFF, int(source_skill["ReqStrength"]),
            int(source_skill["ReqDexterity"]), int(source_skill["ItemSkill"]),
            int(source_skill["isDamage"]), int(source_skill["IconNumber"]),
        ))
    return (packet(tree_records, TREE_RECORD.size),
            packet(tooltip_records, RISE_TOOLTIP_RECORD.size),
            packet(skill_records, RISE_SKILL_RECORD.size))


def verify_packet(encoded: bytes, size: int) -> None:
    if len(encoded) != 58 * size + 4:
        raise AssertionError("private Master Slayer output size drifted")
    plain = b"".join(bux(encoded[index * size:(index + 1) * size])
                     for index in range(58))
    if struct.unpack_from("<I", encoded, 58 * size)[0] != zlib.crc32(plain):
        raise AssertionError("private Master Slayer CRC/round-trip drifted")


def main() -> None:
    tree, tooltip, skills = build()
    verify_packet(tree, TREE_RECORD.size)
    verify_packet(tooltip, RISE_TOOLTIP_RECORD.size)
    verify_packet(skills, RISE_SKILL_RECORD.size)
    TARGET.mkdir(parents=True, exist_ok=True)
    for name, value in (("MasterSlayerTree.bmd", tree),
                        ("MasterSlayerTooltip.bmd", tooltip),
                        ("MasterSlayerSkills.bmd", skills)):
        output = TARGET / name
        if output.exists() and output.read_bytes() != value:
            raise AssertionError(f"refusing to overwrite divergent private asset {output}")
        if not output.exists():
            output.write_bytes(value)
        print(f"PASS: {output} records=58 sha256={digest(value)}")
    icon = S21_ICON.read_bytes()
    if digest(icon) != S21_ICON_SHA or icon[24:26] != b"\xff\xd8":
        raise AssertionError("native S21 Master Icon OZJ hash/header drifted")
    icon_output = TARGET.parent / "Interface" / "new_Master_Icon.OZJ"
    icon_output.parent.mkdir(parents=True, exist_ok=True)
    if icon_output.exists() and icon_output.read_bytes() != icon:
        raise AssertionError(f"refusing to overwrite divergent private asset {icon_output}")
    if not icon_output.exists():
        icon_output.write_bytes(icon)
    print(f"PASS: {icon_output} native S21 atlas sha256={digest(icon)}")


if __name__ == "__main__":
    main()
