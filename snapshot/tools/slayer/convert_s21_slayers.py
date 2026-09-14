#!/usr/bin/env python3
"""Build the hash-pinned Slayer S21 asset overlay for the isolated 5.2 port.

The Season 21 files are read-only inputs.  The output is restricted to the
private SlayerBuild client so a normal 5.2 checkout can never load these
assets accidentally.
"""

from __future__ import annotations

import importlib.util
import json
from pathlib import Path


SOURCE_ROOT = Path(r"D:\MU FICA Season 21\Data\Effect")
WORKTREE_ROOT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer")
TARGET_ROOT = WORKTREE_ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "Client" / "Data" / "RISE" / "Slayer"
CONVERTER = Path(
    r"C:\Users\DELL\Documents\Codex\2026-09-14\do\work\SkillRise5.2\snapshot\tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py"
)

ASSETS = {
    "ak_skill_sword.bmd": "E7EA146D4B6433A8F12D53E872804B53438986E76D39CF3999D494136A7DFA33",
    "ak_skill_sword_s01.bmd": "7EFEE893EA9E803DF72A9694D3F4C59C353295D80B6D9B40F6658EEDECD11C4F",
    "Bat_van01.bmd": "A01A9F4C767811CF5B5E2E855354A6DE2C63976C26BBE8A40DE886808E713C7E",
    "van_object02_skill.bmd": "EC45FCBADBC129D1056D64EBFB70EA32D3AD3C0A29F47AF7D341A5A762DF5A5B",
    "Van_object04_skill.bmd": "E223A0D578DEB234AE30CB4CC67FBA1809B7D640CC86B2A7D7031754D9A0EC6D",
    "van_swordline01.bmd": "3B14028356C76C3E0BE6C4548A4BB0B33D479E76EC07EE7C18CA6DB8DE6235EC",
    "van_object03_skill.bmd": "EC83F31E6AA0D3FA8468A54DEA562BF3AA6A96161FD8603CA479CA292BEC75DB",
}

TEXTURES = {
    "sword_ak.OZJ": "AFD13120F4BF24427DB5012D52A913D532D390970D83D6DF04F0838F9A7D0FDF",
    "volcano_of_stone2_render.OZJ": "8235C8CCC386EC5B6990D481B69F67ED12F717E89E53B87084FE3D2CAEA25CC4",
    "Swordeff_mono2_temp.OZJ": "4197369C2212C49F5B16D68F39777EF868DC79871B2B22D86A07479F6909A315",
    "alpha_RingX128_mono.OZJ": "37BBD0E324F174676B42D6C193079ACE5781BEA0663FA5B7031F8A9F7BB090CE",
    "bet_grilsshot2.OZJ": "A32EE489023F6EE3E30388A2BCCC1C40EA779150FEA021BA3EC9160EB36B477A",
    "marks_m03.OZJ": "AAB0CFA2A69CCEA87CB9B81341F134B08814648C0D2AE30FF193320D44173233",
    "empact01.OZJ": "BBBF1FB4EDD1CB2492674CD321A8B0C240CA4A68131772924FD60749046628D7",
    "macardkmono.OZJ": "EF8FCA967ABD49618D195950A3E957AF0533CE88A35CF82D5BD80C4EA4305763",
    "Elite_monster_ground02.OZJ": "ED8265FB90F5B6FBC5371EC5CCCCE8E703972A3DA1AB0BEB9062BFC54E6B4A1E",
    "ark.OZJ": "96B04ACCF070BABC8911D586A78E243A6672E926D15B755F4B8AD6DA1DC40D2A",
}

SOUNDS = {
    "SwordInertiastart.wav": "2D87DCD8BC518F6FB50A23CDA182437B0C240A04E2D2B65BA27CE87925B7357D",
    "SwordInertiaAttack.wav": "0FEB4AD8716C839276B51C29CA52D70FFABAB1FF9F2FF6AE5EE4B441F2C4B070",
    "BatFlockstart.wav": "D79211099EDD59BE8056BBBFE9E04B05B3D47E88CB7A091670B1F4F002E0F374",
    "BatFlockAttack.wav": "045DE62960CABEE8139CC412F74F3FF8C3D0FD6657377319DF0FCE881EE1C82F",
    "FierceAttackstart.wav": "E6D42AF732E2AD54DF8C33AA9FC54BC277E63ED619294ACC526D5DF3A5545F6E",
    "FierceAttack_Attack_1.wav": "19D1ABF7AA3D3D473400F99946036BB20E64B4D4B91AAF2DDC04F2E29B436C2E",
    "FierceAttack_Attack_2.wav": "1AA18550E48930E7EAC483CACFB4FABA1F1E0757306F9C266E46D18AB9721780",
    "FierceAttack_Attack_3.wav": "832D2F84EB06797D688945DF9DA91B048F54B8A72B39D5B07FBD6E917A84B3FF",
    "Detection_3.wav": "798BC237214A3A81152B648BA9F355B33CDF8DC02511BD000BDFAA26BF740473",
}


def load_converter():
    spec = importlib.util.spec_from_file_location("s21_bmd_converter", CONVERTER)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load converter: {CONVERTER}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    converter = load_converter()
    converter.verify_reference_vector()
    effect_root = TARGET_ROOT / "Effect"
    sound_root = TARGET_ROOT / "Sound"
    effect_root.mkdir(parents=True, exist_ok=True)
    sound_root.mkdir(parents=True, exist_ok=True)
    report = []
    for filename, expected_hash in ASSETS.items():
        source = SOURCE_ROOT / filename
        if not source.is_file():
            raise FileNotFoundError(source)
        source_bytes = source.read_bytes()
        actual_hash = converter.sha256(source_bytes)
        if actual_hash != expected_hash:
            raise ValueError(f"source hash mismatch for {filename}: {actual_hash}")
        plaintext = converter.decrypt_s21_payload(
            converter.parse_s21_container(source_bytes)
        )
        identity = converter.inspect_plaintext(plaintext)
        output = effect_root / filename
        encoded = converter.encrypt_rise_v0c(plaintext)
        if converter.decrypt_rise_v0c(encoded) != plaintext:
            raise RuntimeError(f"v0C round-trip failed for {filename}")
        output.write_bytes(encoded)
        report.append(
            {
                "file": filename,
                "sourceSha256": actual_hash,
                "outputSha256": converter.sha256(encoded),
                "plaintextSha256": converter.sha256(plaintext),
                **identity,
            }
        )
    for filename, expected_hash in TEXTURES.items():
        source = SOURCE_ROOT / filename
        source_bytes = source.read_bytes()
        actual_hash = converter.sha256(source_bytes)
        if actual_hash != expected_hash:
            raise ValueError(f"source hash mismatch for {filename}: {actual_hash}")
        (effect_root / filename).write_bytes(source_bytes)
        report.append({"file": filename, "sourceSha256": actual_hash, "kind": "texture"})

    sound_source_root = SOURCE_ROOT.parent / "Sound"
    for filename, expected_hash in SOUNDS.items():
        source = sound_source_root / filename
        source_bytes = source.read_bytes()
        actual_hash = converter.sha256(source_bytes)
        if actual_hash != expected_hash:
            raise ValueError(f"source hash mismatch for {filename}: {actual_hash}")
        (sound_root / filename).write_bytes(source_bytes)
        report.append({"file": filename, "sourceSha256": actual_hash, "kind": "sound"})

    (TARGET_ROOT / "asset_conversion_report.json").write_text(
        json.dumps(report, indent=2) + "\n", encoding="utf-8"
    )
    print("Status=PASS")
    print(f"Target={TARGET_ROOT}")
    for item in report:
        if item.get("kind"):
            print(f"{item['file']} kind={item['kind']} sourceSha256={item['sourceSha256']}")
            continue
        print(
            f"{item['file']} meshes={item['meshCount']} bones={item['boneCount']} "
            f"actions={item['actionCount']} outputSha256={item['outputSha256']}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
