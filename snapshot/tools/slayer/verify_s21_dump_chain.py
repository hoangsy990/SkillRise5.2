#!/usr/bin/env python3
"""Verify the Slayer dispatcher facts used by the isolated 5.2 port.

This is deliberately a provenance check, not a decompiler.  It pins the
captured S21 main image and asserts that the checked-in disassembly contains
the five skill compares/handlers, their direct effect roots, action values,
sound families, and the separate action-switch roots.  The 5.2 source remains
a native reimplementation; no S21 address is executed by the client.
"""

from __future__ import annotations

import hashlib
import re
import struct
import xml.etree.ElementTree as ET
from pathlib import Path


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
DISPATCH = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_full_skill_reverse\disasm_12CEB00_12D1A50.txt"
)
ACTION_SWITCH = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_full_skill_reverse\disasm_10DFF00_10ED000.txt"
)
S21_AOE = Path(r"D:\GameServer S21\Data\Skills\SkillAOETargetting.xml")
EXPECTED_AOE_SHA256 = "CE5C501CB82D2179DB5ED5407DC89BEA4CB118446BA444D8FF95D7DE96F984D9"
S21_SKILL_REQUIRE = Path(r"D:\GameServer S21\Data\Skills\SkillRequire.xml")
EXPECTED_SKILL_REQUIRE_SHA256 = "4E6B765FF7DCFB62E58AD9720F5E8C8E81DC8ECC5BA30C3BC3B842B390C9AC58"
S21_THIRD_TREE = Path(r"D:\GameServer S21\Data\Skills\SkillTreeData_3rd.xml")
EXPECTED_THIRD_TREE_SHA256 = "CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0"
S21_SKILL_LIST = Path(r"D:\GameServer S21\Data\Skills\SkillList.xml")
EXPECTED_SKILL_LIST_SHA256 = "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0"
S21_MASTER_CALC = Path(r"D:\GameServer S21\Data\Skills\MasterSkillCalc_3rd.lua")
EXPECTED_MASTER_CALC_SHA256 = "771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699"
S21_CALC = Path(r"D:\GameServer S21\Data\Skills\RegularSkillCalc.lua")
EXPECTED_CALC_SHA256 = "F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847"
S21_SCRIPT_CALC = Path(r"D:\GameServer S21\Data\Scripts\Skills\RegularSkillCalc.lua")
EXPECTED_SCRIPT_CALC_SHA256 = "78A1502C187D9EA248E682CCAA62EA8C4510D7119192510D1A966A18E6992E4D"
S21_FORMULA = Path(r"D:\GameServer S21\Data\FormulaData.xml")
EXPECTED_FORMULA_SHA256 = "9A513DD4375116CFD0FA599114B2103B4D21B194B775AE376E0C6289BA36E6FF"

EXPECTED_DUMP_SHA256 = (
    "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
)

# skill: compare VA, handler VA, direct root values, handler action (when the
# receive handler emits it), and sound.  292/294 set their player action in
# the separate action switch below, so their handler action is intentionally
# None.
DISPATCH_FACTS = {
    292: ("012CEF03", "012D0F0F", ("68a", "689"), None, "50d"),
    293: ("012CEF13", "012D10A0", ("682", "683", "684"), "e3", "50b"),
    # The 0x679 receive root/sound below are present only behind 0x816.
    294: ("012CEF23", "012D1209", ("679",), None, "509"),
    295: ("012CEE1E", "012D139B", ("692",), "e8", "511"),
    297: ("012CEF58", "012D1443", ("695",), "e9", "511"),
}

# The second switch is a call-site distinction.  These are direct roots in
# the relevant branch, not additional ReceiveMagic children.
ACTION_BRANCHES = {
    292: ("010E68A3", ("679", "68a", "689"), "e0"),
    293: ("010E6B96", ("682", "683", "684"), "e3"),
    # Base 0x126 always creates 0x68A; 0x679 needs skillStruct+8 == 0x816.
    294: ("010E6D1B", ("68a",), "e4"),
    295: ("010E6F2A", ("692",), "e8"),
    297: ("010E6FF8", ("695",), "e9"),
}


def require(text: str, needle: str, label: str) -> None:
    if needle.lower() not in text.lower():
        raise AssertionError(f"missing {label}: {needle}")


def handler_window(text: str, handler: str, next_handler: str | None) -> str:
    start = text.lower().find(handler.lower())
    if start < 0:
        raise AssertionError(f"missing handler {handler}")
    end = text.lower().find(next_handler.lower(), start + len(handler)) if next_handler else len(text)
    return text[start:end if end >= 0 else len(text)]


def main() -> int:
    if not DUMP.is_file():
        raise FileNotFoundError(DUMP)
    if not DISPATCH.is_file():
        raise FileNotFoundError(DISPATCH)
    if not ACTION_SWITCH.is_file():
        raise FileNotFoundError(ACTION_SWITCH)
    if not S21_AOE.is_file():
        raise FileNotFoundError(S21_AOE)
    if not S21_SKILL_REQUIRE.is_file():
        raise FileNotFoundError(S21_SKILL_REQUIRE)
    if not S21_THIRD_TREE.is_file():
        raise FileNotFoundError(S21_THIRD_TREE)
    if not S21_SKILL_LIST.is_file():
        raise FileNotFoundError(S21_SKILL_LIST)
    if not S21_MASTER_CALC.is_file():
        raise FileNotFoundError(S21_MASTER_CALC)
    if not S21_CALC.is_file():
        raise FileNotFoundError(S21_CALC)
    if not S21_SCRIPT_CALC.is_file():
        raise FileNotFoundError(S21_SCRIPT_CALC)
    if not S21_FORMULA.is_file():
        raise FileNotFoundError(S21_FORMULA)

    image = DUMP.read_bytes()
    digest = hashlib.sha256(image).hexdigest().upper()
    if digest != EXPECTED_DUMP_SHA256:
        raise AssertionError(f"S21 dump hash mismatch: {digest}")
    print(f"PASS: S21 main dump sha256={digest}")

    aoe_bytes = S21_AOE.read_bytes()
    aoe_digest = hashlib.sha256(aoe_bytes).hexdigest().upper()
    if aoe_digest != EXPECTED_AOE_SHA256:
        raise AssertionError(f"S21 AOE config hash mismatch: {aoe_digest}")
    aoe_root = ET.fromstring(aoe_bytes)
    aoe_rows = {int(row.attrib["Index"]): row.attrib.get("Name", "")
                for row in aoe_root.iter("Skill")}
    for skill, name in ((293, "Bat Flock"), (294, "Pierce Attack")):
        if aoe_rows.get(skill) != name:
            raise AssertionError(f"S21 AOE row mismatch: {skill} {aoe_rows.get(skill)}")
    print(f"PASS: S21 AOE config sha256={aoe_digest} skills=293,294 (geometry not disclosed)")

    require_bytes = S21_SKILL_REQUIRE.read_bytes()
    require_digest = hashlib.sha256(require_bytes).hexdigest().upper()
    if require_digest != EXPECTED_SKILL_REQUIRE_SHA256:
        raise AssertionError(f"S21 SkillRequire hash mismatch: {require_digest}")
    require_row = next((row.attrib for row in ET.fromstring(require_bytes).iter("Skill")
                        if row.attrib.get("Index") == "294"), None)
    if require_row is None or any(require_row.get(key) != value for key, value in (
        ("ReqIndex", "293"), ("ReqMasterSkillIndex", "782"),
        ("ReqMasterPoint", "10"), ("ItemGroup", "12"),
        ("ItemIndex", "479"))):
        raise AssertionError(f"S21 Pierce SkillRequire row drifted: {require_row}")
    print(f"PASS: S21 Pierce prereq sha256={require_digest} base=293 mastery=782/10 bead=12:479")

    tree_bytes = S21_THIRD_TREE.read_bytes()
    tree_digest = hashlib.sha256(tree_bytes).hexdigest().upper()
    if tree_digest != EXPECTED_THIRD_TREE_SHA256:
        raise AssertionError(f"S21 third mastery tree hash mismatch: {tree_digest}")
    master_slayer = next((row for row in ET.fromstring(tree_bytes).iter("Class")
                          if row.attrib.get("ID") == "512"), None)
    if master_slayer is None:
        raise AssertionError("S21 Master Slayer class=512 tree missing")
    mastery = {row.attrib.get("MagicNumber"): row.attrib
               for row in master_slayer.iter("Skill")}
    for number, expected in {
        "781": {"Name": "Bat Flock Strengthener", "Index": "58",
                "ReqMinPoint": "1", "MaxPoint": "20", "ParentSkill1": "0"},
        "782": {"Name": "Bat Flock Mastery", "Index": "62",
                "ReqMinPoint": "10", "MaxPoint": "10", "ParentSkill1": "781"},
    }.items():
        row = mastery.get(number)
        if row is None or any(row.get(key) != value for key, value in expected.items()):
            raise AssertionError(f"S21 Slayer mastery {number} row drifted: {row}")
    # Rush is a different S21 Slayer mastery skill. It cannot be treated as
    # the missing base Pierce Attack caster movement implementation.
    if mastery.get("631", {}).get("Name") != "Rush":
        raise AssertionError("S21 Master Slayer Rush=631 row missing")
    print(f"PASS: S21 Master Slayer tree sha256={tree_digest} Bat=781->782 Rush=631 (separate skill)")

    skill_list_bytes = S21_SKILL_LIST.read_bytes()
    skill_list_digest = hashlib.sha256(skill_list_bytes).hexdigest().upper()
    if skill_list_digest != EXPECTED_SKILL_LIST_SHA256:
        raise AssertionError(f"S21 SkillList hash mismatch: {skill_list_digest}")
    list_rows = {row.attrib.get("Index"): row.attrib
                 for row in ET.fromstring(skill_list_bytes).iter("Skill")}
    for number, expected in {
        "781": {"Name": "Bat Flock Strengthener", "Damage": "22",
                "ManaUsage": "25", "BPUsage": "9", "Distance": "6",
                "ReqLevel": "160", "ReqStrength": "100",
                "ReqDexterity": "380", "Slayer": "3", "UseType": "4",
                "Brand": "293"},
        "782": {"Name": "Bat Flock Mastery", "Damage": "23",
                "ManaUsage": "30", "BPUsage": "12", "Distance": "6",
                "ReqLevel": "160", "ReqStrength": "100",
                "ReqDexterity": "380", "Slayer": "3", "UseType": "4",
                "Brand": "781"},
    }.items():
        row = list_rows.get(number)
        if row is None or any(row.get(key) != value for key, value in expected.items()):
            raise AssertionError(f"S21 Bat mastery SkillList {number} drifted: {row}")
    master_calc_bytes = S21_MASTER_CALC.read_bytes()
    master_calc_digest = hashlib.sha256(master_calc_bytes).hexdigest().upper()
    if master_calc_digest != EXPECTED_MASTER_CALC_SHA256:
        raise AssertionError(f"S21 MasterSkillCalc hash mismatch: {master_calc_digest}")
    master_calc = master_calc_bytes.decode("utf-8", errors="replace")
    for name in ("SlayerBatFlock_MasterLevel1_Calc",
                 "SlayerBatFlock_MasterLevel2_Calc"):
        require(master_calc, f"function {name}(", f"S21 Bat mastery Lua {name}")
    print(f"PASS: S21 Bat mastery cast IDs=781,782 SkillList sha256={skill_list_digest} Lua sha256={master_calc_digest}")

    calc_bytes = S21_CALC.read_bytes()
    calc_digest = hashlib.sha256(calc_bytes).hexdigest().upper()
    if calc_digest != EXPECTED_CALC_SHA256:
        raise AssertionError(f"S21 regular-skill Lua hash mismatch: {calc_digest}")
    calc = calc_bytes.decode("utf-8", errors="replace")
    for name in ("SlayerSwordInertiaCalc", "SlayerBatFlockCalc",
                 "SlayerPierceAttackCalc"):
        start = calc.find(f"function {name}(")
        if start < 0:
            raise AssertionError(f"S21 damage calculator missing: {name}")
        end = calc.find("\nend", start)
        body = calc[start:end]
        if "((Strength / 8) + (Dexterity / 28) + 120) / 100" not in body:
            raise AssertionError(f"S21 fractional STR/DEX rate drifted: {name}")
        if name == "SlayerBatFlockCalc" and "OutDamage * 0.5" not in body:
            raise AssertionError("S21 Bat Flock half-strike drifted")
    print(f"PASS: S21 fractional Slayer damage Lua sha256={calc_digest} rate=STR/8+DEX/28+120 bat-half=0.5")

    script_bytes = S21_SCRIPT_CALC.read_bytes()
    script_digest = hashlib.sha256(script_bytes).hexdigest().upper()
    if script_digest != EXPECTED_SCRIPT_CALC_SHA256:
        raise AssertionError(f"S21 script-skill Lua hash mismatch: {script_digest}")
    script = script_bytes.decode("utf-8", errors="replace")
    start = script.find("function BatFlockDotDamage(")
    end = script.find("\nend", start)
    if start < 0 or end < 0:
        raise AssertionError("S21 Bat Flock DOT function missing")
    dot_body = script[start:end]
    if dot_body.count("OutDamage = DOT / 100") != 2:
        raise AssertionError("S21 player/monster Bat Flock DOT rates drifted")
    formula_bytes = S21_FORMULA.read_bytes()
    formula_digest = hashlib.sha256(formula_bytes).hexdigest().upper()
    if formula_digest != EXPECTED_FORMULA_SHA256:
        raise AssertionError(f"S21 FormulaData hash mismatch: {formula_digest}")
    character = ET.fromstring(formula_bytes).find("Character")
    if character is None:
        raise AssertionError("S21 FormulaData Character section missing")
    formula9 = next((row.attrib.get("Data") for row in character.findall("Formula")
                     if row.attrib.get("ID") == "9"), None)
    if formula9 != "((((((%d*%d)*0.6/24)+(((%d-800)*(%d-500))/200))/24)+2000))":
        raise AssertionError(f"S21 Bat Flock Character formula 9 drifted: {formula9}")
    print(f"PASS: S21 Bat Flock DOT script sha256={script_digest} FormulaData sha256={formula_digest} Character:9/100")

    # The supplementary 0x126 path is outside the earlier text-disassembly
    # capture. Assert its actual bytes in the pinned mapped main image,
    # including the skill compare, both roots, and the effect-list count.
    def at(va: int, size: int) -> bytes:
        offset = va - 0x400000
        return image[offset:offset + size]

    # The native 0x694 model is dispatched through the generic object draw
    # wrapper. Its ordinary body pass is flag 2 (texture), not an invented
    # additive pass; the E4 action-init also has a bounded actor-Z writer.
    native_model_bytes = {
        0x15B2BCA: bytes.fromhex("6aff6aff6a006a006a00ffb52cf6ffffe842aa1b0083c418"),
        0x1887EB0: bytes.fromhex("6a028b4df8e89e0d0e"),
        0x128A082: bytes.fromhex("81bdd8feffffe40000000f84821d0000"),
        0x128BE14: bytes.fromhex("6a028b4d0c81c158010000e8aa21aaff"),
        0x128BE30: bytes.fromhex("f30f1000f30f5805cce4b401"),
    }
    for va, expected in native_model_bytes.items():
        if at(va, len(expected)) != expected:
            raise AssertionError(f"S21 0x694/E4 action-init bytes drifted at {va:#x}")
    print("PASS: S21 0x694 ordinary body flag=2; E4 action-init actor Position Z +=5 (not XY rush proof)")

    pierce_action_child_codes = {
        0x128BED1: 0x81CD,
        0x147D5A3: 0x81CE,
        0x147D64F: 0x81CE,
        0x147D6FB: 0x81CE,
        0x147D7A2: 0x80BA,
        0x147D840: 0x8149,
        0x147D8DE: 0x81CF,
        0x147D97C: 0x5D8,
        0x147DB3D: 0x80BA,
        0x147DCFE: 0x80BA,
        0x147DEBA: 0x80BA,
    }
    for va, code in pierce_action_child_codes.items():
        if at(va, 5) != b"\x68" + code.to_bytes(4, "little"):
            raise AssertionError(f"S21 Pierce 0x81CD child code drifted at {va:#x}")
    if at(0x143F67F, 10) != bytes.fromhex("81bd9cceffffcd810000"):
        raise AssertionError("S21 0x81CD initializer selector drifted")
    if at(0x147D373, 10) != bytes.fromhex("8b85c8ceffff83786002"):
        raise AssertionError("S21 0x81CD subtype-2 branch drifted")
    print("PASS: Pierce 0xE4 subtype-2 0x81CD root and ten exact S21 child effect codes pinned")
    # Native S21 does not use a guessed Pierce texture for 0x81CE. Its
    # bitmap loader binds 0x81CE to NPC\\marks_m04.JPG; the high-code update
    # and render switches route 0x81CD/0x81CE independently.
    if at(0xAA997F, 5) != bytes.fromhex("68ce810000"):
        raise AssertionError("S21 0x81CE bitmap loader code drifted")
    if at(0xAA995F, 5) != bytes.fromhex("68cd810000"):
        raise AssertionError("S21 0x81CD bitmap loader code drifted")
    if not at(0x1B52784, 28).startswith(b"NPC\\marks_m04.JPG\x00"):
        raise AssertionError("S21 0x81CE bitmap loader filename drifted")
    if not at(0x1B52770, 28).startswith(b"NPC\\marks_m03.JPG\x00"):
        raise AssertionError("S21 0x81CD bitmap loader filename drifted")
    if at(0x15AE502, 5) != bytes.fromhex("68ce810000"):
        raise AssertionError("S21 0x81CE subtype-3 renderer bitmap drifted")
    for va in (0x15AE675, 0x15AE740):
        if at(va, 5) != bytes.fromhex("68cd810000"):
            raise AssertionError(f"S21 0x81CE subtype-4/5 renderer bitmap drifted at {va:#x}")
    dispatch_bytes = {
        0x14B8383: bytes.fromhex("81bd8cdbfeffcd810000"),
        0x14B82E2: bytes.fromhex("81bd8cdbfeffce810000"),
        0x15A1724: bytes.fromhex("81bd28f6ffffcd810000"),
        0x15A1734: bytes.fromhex("81bd28f6ffffce810000"),
    }
    for va, expected in dispatch_bytes.items():
        if at(va, len(expected)) != expected:
            raise AssertionError(f"S21 0x81CD/0x81CE update/render selector drifted at {va:#x}")
    print("PASS: S21 0x81CE subtype3=marks_m04, subtype4/5=marks_m03 and high-code dispatch pinned")
    pierce_mark_floats = {
        0x1B9E9F8: 2.86,
        0x1B9EA24: 4.55,
        0x1B9EA20: 4.35,
        0x1B4E6D4: 0.05,
        0x1B9EA80: 6000.0,
    }
    for va, expected in pierce_mark_floats.items():
        actual = struct.unpack("<f", at(va, 4))[0]
        if abs(actual - expected) > 0.0001:
            raise AssertionError(f"S21 Pierce 0x81CE scale/timer float drifted at {va:#x}: {actual}")
    if at(0x147E165, 6) != bytes.fromhex("837860030f85"):
        raise AssertionError("S21 Pierce 0x81CE subtype-3 init branch drifted")
    print("PASS: S21 Pierce 0x81CE scale=2.86/4.55 and 6000-ms refresh window pinned")
    # Native 0x8149 subtype 2 is an effect-pool object which submits a
    # ground_star sprite each draw, not a terrain-alpha tile. The root call
    # supplies light (.9,0,1) and scale 1.7; the native sprite jump table
    # routes subtype 2 to the 0x172760A allocator path.
    if at(0x18D234A, 5) != bytes.fromhex("6849810000"):
        raise AssertionError("S21 0x8149 ground_star loader ID drifted")
    if not at(0x1BAAE18, 30).startswith(b"Skill\\ground_star.jpg\x00"):
        raise AssertionError("S21 0x8149 ground_star loader filename drifted")
    if at(0x147D840, 5) != bytes.fromhex("6849810000"):
        raise AssertionError("S21 Pierce root 0x8149 child call drifted")
    if at(0x1476378, 6) != bytes.fromhex("83786002753e"):
        raise AssertionError("S21 0x8149 subtype-2 init selector drifted")
    if at(0x152BA00, 6) != bytes.fromhex("837860027549"):
        raise AssertionError("S21 0x8149 subtype-2 update selector drifted")
    render_targets = struct.unpack("<10I", at(0x15BDCAC, 40))
    if render_targets[2] != 0x15B145C or at(0x15B14BC, 5)[0] != 0xE8:
        raise AssertionError("S21 0x8149 subtype-2 sprite renderer drifted")
    for va, expected in ((0x1B4EDA4, 0.9), (0x1B6A954, 1.7)):
        if abs(struct.unpack("<f", at(va, 4))[0] - expected) > 0.0001:
            raise AssertionError(f"S21 0x8149 root light/scale drifted at {va:#x}")
    print("PASS: S21 Pierce 0x8149 subtype2 ground_star sprite, 1.7 scale, .9 light and 6000-ms clock pinned")
    # 0x81CF subtype 2 is the next direct Pierce parent, with a nested
    # 0x8012 subtype-17 child. The parent ring is ported; the nested
    # ShockWave effect remains separately open until its map gate is decoded.
    if at(0x147D8DE, 5) != bytes.fromhex("68cf810000"):
        raise AssertionError("S21 Pierce 0x81CF parent call drifted")
    if at(0x147E7CA, 4) != bytes.fromhex("83786002"):
        raise AssertionError("S21 0x81CF subtype-2 init selector drifted")
    if at(0x147E7DA, 7) != bytes.fromhex("c7406c0f000000"):
        raise AssertionError("S21 0x81CF subtype-2 15-tick initializer drifted")
    if at(0x147E912, 5) != bytes.fromhex("6812800000"):
        raise AssertionError("S21 0x81CF nested 0x8012 call drifted")
    if at(0x18BD19A, 5) != bytes.fromhex("6812800000"):
        raise AssertionError("S21 ShockWave bitmap loader ID drifted")
    if not at(0x1BB8758, 30).startswith(b"Effect\\ShockWave.jpg\x00"):
        raise AssertionError("S21 ShockWave bitmap loader filename drifted")
    for va, expected in ((0x1B4DF18, 0.8), (0x1B4E998, 0.6),
                         (0x1B4E6D8, 0.15), (0x1B4F6B0, 15.0)):
        if abs(struct.unpack("<f", at(va, 4))[0] - expected) > 0.0001:
            raise AssertionError(f"S21 0x81CF light/scale/alpha drifted at {va:#x}")
    print("PASS: S21 Pierce 0x81CF subtype2 ring parent and nested 0x8012 ShockWave child pinned")
    # Low model code 0x5D8 is dispatched by compact jump tables, not a
    # direct cmp. Its subtype-1 init is shared with subtype 0, but its
    # update branch does not refresh lifetime as subtype 0 does.
    if at(0x147D97C, 5) != bytes.fromhex("68d8050000"):
        raise AssertionError("S21 Pierce 0x5D8 direct model call drifted")
    init_index = 0x5D8 - 0x4F7
    if struct.unpack("<I", at(0x14B3224 + init_index * 4, 4))[0] != 0x147ED07:
        raise AssertionError("S21 0x5D8 initializer jump-table target drifted")
    update_index = 0x5D8 - 0x4D3
    selector = at(0x1594BE0 + update_index, 1)[0]
    if struct.unpack("<I", at(0x1594A6C + selector * 4, 4))[0] != 0x1534A58:
        raise AssertionError("S21 0x5D8 updater jump-table target drifted")
    if at(0x147ED19, 4) != bytes.fromhex("83786001"):
        raise AssertionError("S21 0x5D8 subtype-1 initializer selector drifted")
    if at(0x15AE9A2, 12) != bytes.fromhex("6aff6aff6a006a006a00ffb5"):
        raise AssertionError("S21 0x5D8 ordinary model render wrapper drifted")
    print("PASS: S21 Pierce 0x5D8 subtype1 marks_cylinder model init/update/render jump-table paths pinned")
    for va in (0x147DB3D, 0x147DCFE, 0x147DEBA):
        if at(va, 5) != bytes.fromhex("68ba800000"):
            raise AssertionError(f"S21 Pierce 0x80BA subtype-7 flare call drifted at {va:#x}")
    flare_lane_floats = {
        0x1B4E4E4: 180.0, 0x1B502B0: 0.38, 0x1B4DF08: 0.2,
        0x1B4E474: 2.0, 0x1B9EAB8: -126.71, 0x1B9EA38: 73.69,
        0x1B9EA44: 134.72, 0x1B9EA40: 84.32,
        0x1B9EABC: -144.92,
    }
    for va, expected in flare_lane_floats.items():
        if abs(struct.unpack("<f", at(va, 4))[0] - expected) > 0.001:
            raise AssertionError(f"S21 Pierce 0x80BA lane offset/light drifted at {va:#x}")
    if at(0x151F0D3, 4) != bytes.fromhex("83786007"):
        raise AssertionError("S21 0x80BA subtype-7 updater selector drifted")
    if at(0x151F1C8, 6) != bytes.fromhex("81c27e800000") or \
       at(0x1661781, 4) != bytes.fromhex("83781409") or \
       at(0x16D5AD6, 4) != bytes.fromhex("83781409"):
        raise AssertionError("S21 Pierce smokeline subtype-9 create/init/update drifted")
    print("PASS: S21 Pierce three 0x80BA subtype7 fixed-180 flare lanes and per-frame updater selector pinned")
    print("PASS: S21 Pierce smokelines 0x807E..0x8080 subtype-9 constructor/updater pinned")
    if at(0x145CBE0, 4) != bytes.fromhex("83786011"):
        raise AssertionError("S21 nested 0x8012 subtype-17 initializer drifted")
    if at(0x14FD81E, 4) != bytes.fromhex("83786011"):
        raise AssertionError("S21 nested 0x8012 subtype-17 updater drifted")
    for va, expected in ((0x1B4E8B0, 4.0), (0x1B4DF14, 0.5),
                         (0x1B52524, 0.28), (0x1B4FB08, 0.95)):
        if abs(struct.unpack("<f", at(va, 4))[0] - expected) > 0.0001:
            raise AssertionError(f"S21 0x8012 subtype-17 scale/light drifted at {va:#x}")
    if at(0x159D2D1, 7) != bytes.fromhex("833ddc85dd0118") or \
       at(0x159D2E3, 7) != bytes.fromhex("833ddc85dd0124"):
        raise AssertionError("S21 0x8012 Kalima map 24..29/36 gate drifted")
    call = at(0x159D3E7, 5)
    target = 0x159D3E7 + 5 + int.from_bytes(call[1:], "little", signed=True)
    if call[0] != 0xE8 or target != 0xE2BD4D:
        raise AssertionError("S21 0x8012 water-terrain renderer call drifted")
    print("PASS: S21 nested 0x8012 subtype17 scale4 ShockWave and Kalima-only water draw pinned")

    if at(0x10EEB92, 7) != bytes.fromhex("6a5768c1000000"):
        raise AssertionError("S21 shared C1:57 skill packet constructor drifted")
    for call_va in (0x10E68D7, 0x10E6D4F, 0x10E6F5B, 0x10E7029):
        call = at(call_va, 5)
        target = call_va + 5 + int.from_bytes(call[1:], "little", signed=True)
        if call[0] != 0xE8 or target != 0x10EEB1E:
            raise AssertionError(f"S21 shared C1:57 call drifted at {call_va:#x}")
    print("PASS: C1:57 is shared by Sword/Pierce/Detection/Demolish, not a Pierce-only position packet")

    pierce_bytes = {
        0x12A6235: bytes.fromhex("81bd08ebffff26010000"),
        0x12A7A26: bytes.fromhex("688a060000"),
        0x12A7A9A: bytes.fromhex("6889060000"),
        0x12A7B87: bytes.fromhex("8981e4010000"),
        0x154759C: bytes.fromhex("6a025966894856"),
        0x15466CF: bytes.fromhex("8b45080fbf405683f802"),
        0x14176F0: bytes.fromhex("8b4d0c668b492a668988a800"),
        0x143E92D: bytes.fromhex("6689482a"),
        0x132D0D6: bytes.fromhex("c645ff01837d0800"),
        0x133F0F0: bytes.fromhex("0fb7402a3de4000000"),
        0x13F2549: bytes.fromhex("0fb7402a3de4000000"),
        0x1B4E6E0: bytes.fromhex("9a99993e"),
        # The local 0x689 lane sends a per-target packet and calls the
        # 1..50 direction-byte counter immediately before appending it.
        0x15465E9: bytes.fromhex("50e86d841f0059"),
        0x173EA69: bytes.fromhex("66a13cf6510b6683c00166a33cf6510b"),
        0x173EA80: bytes.fromhex("83f8327e0933c04066a33cf6510b"),
        0x173EA8E: bytes.fromhex("8b45088a0d3cf6510b8808"),
        0x12A7A43: bytes.fromhex("0f57c0f30f110424"),
        0x1545EA1: bytes.fromhex("f30f1080a0000000"),
        0x1545EF8: bytes.fromhex("f30f1080a0000000"),
        0x154605F: bytes.fromhex("f30f1080a0000000"),
        0x143E7E1: bytes.fromhex("0f57c00f2f45207218"),
        0x143E7F0: bytes.fromhex("f30f1005a4edb401"),
        0x143E7F8: bytes.fromhex("f30f1180a0000000"),
        0x1B4EDA4: bytes.fromhex("6666663f"),
        # In contrast to 0x689, the 0x679 flank children intentionally
        # read their owning actor's scale via EFFECT+0x34C.
        0x1541607: bytes.fromhex("8b45088b804c030000"),
        0x1541611: bytes.fromhex("f30f1080a0000000"),
        0x1541AFB: bytes.fromhex("8b45088b804c030000"),
        0x1541B05: bytes.fromhex("f30f1080a0000000"),
        # Native SetAttackSpeed derives Slayer's term from raw attack speed
        # at .002, then writes distinct E0/E1/E2/E3/E4/E8/E9 bases.
        0x1408874: bytes.fromhex("f30f104508f30f590570edb401"),
        0x1408881: bytes.fromhex("f30f1145f0"),
        0x1B4ED70: bytes.fromhex("6f12033b"),
        0x140A401: bytes.fromhex("f30f1005e800b501"),
        0x140A40E: bytes.fromhex("68e0000000"),
        0x140A434: bytes.fromhex("f30f10050cdfb401"),
        0x140A441: bytes.fromhex("68e1000000"),
        0x140A599: bytes.fromhex("f30f100504dfb401"),
        0x140A5A6: bytes.fromhex("68e8000000"),
        0x140A60C: bytes.fromhex("68e9000000"),
        0x1B500E8: bytes.fromhex("f628dc3e"),
        0x1B4DF0C: bytes.fromhex("cdcccc3e"),
        0x1B4DF04: bytes.fromhex("cdcccc3d"),
        # Base Sword's 0x679 controller copies action E0 PlaySpeed into
        # EFFECT+0xBC; its own frame gates the three authored sword lanes.
        0x148EE0F: bytes.fromhex("68e0000000"),
        0x148EE26: bytes.fromhex("8b40048981bc000000"),
        0x149015B: bytes.fromhex("8b4004894104"),
        0x149016D: bytes.fromhex("8b40048981bc000000"),
        0x14902C2: bytes.fromhex("8b4004894104"),
        0x14902D4: bytes.fromhex("8b40048981bc000000"),
        0x149030A: bytes.fromhex("8b804c0300000fb7402a"),
        0x14903CD: bytes.fromhex("8b4004894104"),
        0x14903DF: bytes.fromhex("8b40048981bc000000"),
        # 0x689 construction snapshots the caster's selected target key;
        # update checks that fixed key before consuming list-bearing lanes.
        0x12A7A3D: bytes.fromhex("0fb7403e50"),
        0x1545A3C: bytes.fromhex("0fbf8064030000"),
        0x1545A70: bytes.fromhex("83f801"),
    }
    for va, expected in pierce_bytes.items():
        if at(va, len(expected)) != expected:
            raise AssertionError(f"Pierce list-bearing S21 bytes drifted at {va:#x}")
    print("PASS: base Pierce supplemental skill=0x126 roots=0x68A(remote),0x689(list) count=packet+8 child0x68B->BMD action0->root-state2->list-pop")
    print("PASS: native 0x689 snapshots caster cast-target+0x3E into effect+0x364 and validates it before list lanes")
    print("PASS: Pierce local 0x689 per-lane outbound counter=0x173EA5C range=1..50 wrap=1")
    print("PASS: Pierce 0x689 incoming scale=0 normalizes to 0.9 in S21 CreateEffect; children read EFFECT+0xA0")
    print("PASS: 0x679 flank children 0x67A/0x67C read owner actor+0xA0 scale via EFFECT+0x34C")
    print("PASS: Slayer E0=.43+AS*.002 E1/E2/E3/E4=.40+AS*.002 E8/E9=.10+AS*.002; 0x681 copies mode 0/1/2 player speeds and mode 3 owner-current-action speed")
    print("PASS: 0x679 controller copies E0 PlaySpeed to EFFECT+0xBC for its frame-4/frame-7 lanes")
    print("PASS: Pierce action=0xE4 renderer alpha=0.3 in both native character paths")

    # The allocator's .9 fallback is not always the final render scale.
    # These S21 initializers write the raw incoming argument back to +A0.
    scale_override_bytes = {
        0x1492259: bytes.fromhex("f30f104520"),
        0x149225E: bytes.fromhex("f30f1180a0000000"),
        0x147EA2A: bytes.fromhex("f30f104520"),
        0x147EA2F: bytes.fromhex("f30f1180a0000000"),
        0x147EA83: bytes.fromhex("f30f104520"),
        0x147EA88: bytes.fromhex("f30f1180a0000000"),
    }
    for va, expected in scale_override_bytes.items():
        if at(va, len(expected)) != expected:
            raise AssertionError(f"Slayer scale-override S21 bytes drifted at {va:#x}")
    print("PASS: 0x691/0x81CF raw zero scale overrides allocator .9; 0x693/0x696 keep allocator scale")

    sword_draw_bytes = {
        0x133F083: bytes.fromhex("0fb7402a3de0000000"),
        0x13F24EB: bytes.fromhex("0fb7402a3de0000000"),
        0x1B4E4CC: bytes.fromhex("0000a04000002041"),
        0x1B4ED34: bytes.fromhex("00006040"),
        0x1546BEA: bytes.fromhex("e8f2004e00996a0659f7f9"),
    }
    for va, expected in sword_draw_bytes.items():
        if at(va, len(expected)) != expected:
            raise AssertionError(f"Sword/S21 0x68A opacity bytes drifted at {va:#x}")
    print("PASS: Sword action=0xE0 draw fade frame<=5/denom10 and 0x68A root half-frame=3.5 child0x691 one-in-six")

    dispatch_text = DISPATCH.read_text(encoding="utf-8", errors="replace")
    action_text = ACTION_SWITCH.read_text(encoding="utf-8", errors="replace")
    ordered_handlers = [facts[1] for facts in DISPATCH_FACTS.values()]
    for skill, (compare, handler, roots, action, sound) in DISPATCH_FACTS.items():
        require(dispatch_text, f"{compare}  ", f"skill {skill} compare VA")
        require(dispatch_text, f"cmp      dword ptr [ebp - 0x1650], 0x{skill:x}", f"skill {skill} compare")
        require(dispatch_text, handler, f"skill {skill} handler VA")
        next_handler = ordered_handlers[ordered_handlers.index(handler) + 1] if handler != ordered_handlers[-1] else None
        window = handler_window(dispatch_text, handler, next_handler)
        for root in roots:
            require(window, f"push     0x{root}", f"skill {skill} root 0x{root}")
        if skill == 292:
            require(window, "cmp      dword ptr [ebp - 0x164c], 0x818",
                    "Sword receive upgrade-only guard")
            require(window, "jne      0x12d109b",
                    "Sword receive skips 0x68A/0x689 for base 0x124")
        if skill == 294:
            require(window, "cmp      dword ptr [ebp - 0x164c], 0x816",
                    "Pierce receive upgrade-only guard")
            require(window, "jne      0x12d1396",
                    "Pierce receive skip for base 0x126")
        if action is not None:
            require(window, f"push     0x{action}", f"skill {skill} action 0x{action}")
        require(window, f"push     0x{sound}", f"skill {skill} sound 0x{sound}")
        root_label = (
            "upgrade0x816-only:" + ','.join('0x' + r for r in roots)
            if skill == 294 else
            "upgrade0x818-only:" + ','.join('0x' + r for r in roots)
            if skill == 292 else ','.join('0x' + r for r in roots)
        )
        print(
            f"PASS: skill={skill} compare=0x{compare} handler=0x{handler} "
            f"roots={root_label} "
            f"handlerAction={'0x' + action if action else 'action-switch'} "
            f"sound={'upgrade0x816-only:' if skill == 294 else 'upgrade0x818-only:' if skill == 292 else ''}0x{sound}"
        )

    for skill, (branch, roots, action) in ACTION_BRANCHES.items():
        require(action_text, branch, f"skill {skill} action branch")
        start = action_text.lower().find(branch.lower())
        next_starts = [
            action_text.lower().find(other[0].lower(), start + len(branch))
            for other in ACTION_BRANCHES.values()
            if action_text.lower().find(other[0].lower(), start + len(branch)) >= 0
        ]
        end = min(next_starts) if next_starts else len(action_text)
        window = action_text[start:end]
        for root in roots:
            require(window, f"push     0x{root}", f"skill {skill} action root 0x{root}")
        if skill == 292:
            require(window, "push     0x679", "base Sword action root")
            require(window, "push     0x509", "base Sword action sound")
            require(window, "cmp      dword ptr [eax + 8], 0x818",
                    "Sword action upgraded-root guard")
            require(window, "jne      0x10e6b91",
                    "base Sword skips 0x68A/0x689")
        if skill == 294:
            require(window, "cmp      dword ptr [eax + 8], 0x816",
                    "Pierce action upgrade-only guard")
            require(window, "jne      0x10e6f25",
                    "Pierce action base skip over 0x679")
            require(window, "push     0x679",
                    "Pierce action guarded 0x679 child")
            require(window, "push     0x50d",
                    "Pierce base action sound")
        require(window, f"push     0x{action}", f"skill {skill} action id 0x{action}")
        print(f"PASS: action-switch skill={skill} branch=0x{branch} roots={','.join('0x'+r for r in roots)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
