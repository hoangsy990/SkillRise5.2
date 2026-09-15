#!/usr/bin/env python3
"""Build the hash-pinned Slayer S21 asset overlay for the isolated 5.2 port.

The Season 21 files are read-only inputs.  The output is restricted to the
private SlayerBuild client so a normal 5.2 checkout can never load these
assets accidentally.
"""

from __future__ import annotations

import importlib.util
import json
import re
from pathlib import Path


SOURCE_ROOT = Path(r"D:\MU FICA Season 21\Data\Effect")
WORKTREE_ROOT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer")
TARGET_ROOT = WORKTREE_ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "Client" / "Data" / "RISE" / "Slayer"
CONVERTER = Path(
    r"C:\Users\DELL\Documents\Codex\2026-09-14\do\work\SkillRise5.2\snapshot\tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py"
)

ASSETS = {
    "Bat_van01.bmd": "A01A9F4C767811CF5B5E2E855354A6DE2C63976C26BBE8A40DE886808E713C7E",
    "van_object02_skill.bmd": "EC45FCBADBC129D1056D64EBFB70EA32D3AD3C0A29F47AF7D341A5A762DF5A5B",
    "Van_object04_skill.bmd": "E223A0D578DEB234AE30CB4CC67FBA1809B7D640CC86B2A7D7031754D9A0EC6D",
    "van_swordline01.bmd": "3B14028356C76C3E0BE6C4548A4BB0B33D479E76EC07EE7C18CA6DB8DE6235EC",
    "van_object03_skill.bmd": "EC83F31E6AA0D3FA8468A54DEA562BF3AA6A96161FD8603CA479CA292BEC75DB",
    # S21 0x5D8 registration at 0xAA99ED names this model directly.
    "marks_cylinder.bmd": "F91EA00CFC10DC3E36935FE80AD6CA391EAED64142E7D572E5AB2939A7689BE3",
}

TEXTURES = {
    "alpha_RingX128_mono.OZJ": "37BBD0E324F174676B42D6C193079ACE5781BEA0663FA5B7031F8A9F7BB090CE",
    "bet_grilsshot2.OZJ": "A32EE489023F6EE3E30388A2BCCC1C40EA779150FEA021BA3EC9160EB36B477A",
    "marks_m03.OZJ": "AAB0CFA2A69CCEA87CB9B81341F134B08814648C0D2AE30FF193320D44173233",
    # S21 loader at 0xAA997F binds bitmap 0x81CE to NPC\\marks_m04.JPG.
    # The read-only NPC and Effect OZJ copies are byte-identical.
    "marks_m04.OZJ": "42DDB35AB1EB1F34E4EF6194959BB1B19E577ADC832BF272C1FAEBCAA89A4AB7",
    # marks_cylinder.bmd mesh material recovered from the BMD plaintext.
    "lines2.OZJ": "79D2A20143B15E406344F43DCDAF6232DD131986BC28603B10BF70DA4F208F7A",
    "empact01.OZJ": "BBBF1FB4EDD1CB2492674CD321A8B0C240CA4A68131772924FD60749046628D7",
    "macardkmono.OZJ": "EF8FCA967ABD49618D195950A3E957AF0533CE88A35CF82D5BD80C4EA4305763",
    "Elite_monster_ground02.OZJ": "ED8265FB90F5B6FBC5371EC5CCCCE8E703972A3DA1AB0BEB9062BFC54E6B4A1E",
    "ark.OZJ": "96B04ACCF070BABC8911D586A78E243A6672E926D15B755F4B8AD6DA1DC40D2A",
    "magic_ground12.OZJ": "D64CEB7F2F9DBEFEADDCC381D8854DCB746895ED9C9EC6A67A241C424D727EE1",
    "ring_of_gradation2.OZJ": "E53C5C8ECFB0F33C5A848DE85A78C788BE248ADA097404151454493B41E8C941",
    "pin_star03.OZJ": "2C8D577F6FCBD71B34736DCFFB108EF485C393F427CA4A882232833BC1908B94",
    "flare01_red.OZJ": "8878E3200C74BFDDC7B56AB07FAA30AC5E6E1995B9671C204B509B865C7C6190",
    # S21 0x7EF7, submitted by 0x80BA subtypes 6/7 at 0x15A98B7.
    "flare01.OZJ": "874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8",
    # Native 0x8012 loader at 0x18BD19A binds Effect\\ShockWave.jpg.
    "ShockWave.OZJ": "F8152B6F1A247578AABA09B2FD55B98AB098AADCFD7048616E685E90B1246B7F",
    # Pierce 0x80BA subtype 7 emits native 0x7FFD particle subtype 17.
    # S21 loader at 0x18BDB86 names Effect\\Clud64.jpg.
    "Clud64.OZJ": "64C359CE760E517B1D5201DF10AE3A0EFFCA4F08A3690CAB6FEEF1C34FFE379D",
    "enemy_ring01.OZJ": "3CA241B6C04A28ABCE73A24302106869AE4B38AAB982A0333BE42E36E27CA82A",
    "gostmark02_red.OZJ": "75B4EAA0C28D7A7A46509294BF0C39511DA28C518838CBA309C1BA2858342EFC",
    "smoke01.OZJ": "05D93565B3E9DC6ADE98A4F6C50BEC676E7E205CE1CFD464B7173359B397DA58",
    "smokelines01.OZJ": "C713032288572AE4413931A01F99E2C27F2067A71DCAF8C5AB00AEA00CDB5566",
    "smokelines02.OZJ": "81D6B3A97B7F92DD699A66FCADC80F056AACBFD420EC2149327434ADF2982F29",
    "smokelines03.OZJ": "8CAFF0978BDC9418F5B62AEB744A8C228D95443CB04CFB9DB2371820F3C599DA",
    "pin_star02_red.OZJ": "4E285498D44A466773224901D0910201FF97E56D49C215958D59999792BF06DA",
    "firehik01_magenta.OZJ": "DBD2EC397B1A53D888ECA4CCC3BD273576532EF2EA9DF779B31A2DBC09F7B87D",
    "gostmark02.OZJ": "58B0579B0AF41BF7A3D779B25E35DC472119BF3E8188933AFB8DBB1154ED0BCA",
    "alpha_RingX256_1.OZJ": "1EDD0918435A806B0B4BA2FDA7F3FD715D9AC67C7534AD90FA1C3F93C3CC27D3",
    "Damage1mono.OZJ": "A095432CB1BAA4FE21BBAB6D7A3E4422C8C114C1E9BC4B0AE563FBE7D1078D31",
    # Exact 0x692 and 0x695 initializer sprites (S21 ids 0x7FDD/0x7F78).
    "flareBlue.OZJ": "E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32",
    "Flare.OZJ": "5778EA278113B8A1E33905EB8519A782CF3D141FF78229C66219E368EC99647B",
    # Exact consecutive S21 sprite block 0x82F0..0x82F4 and 0x82F9.
    "water_board_red.OZJ": "3C9FB55B65E0999BB8754E22D5E056FE6C7024E43F8775EBFA39AFDA5BB5EC42",
    "water_wall.OZT": "70C1659B1EDD931315944067613F9F34F4588423C9DD91DAA4DA01D848B59F35",
    "smoke01_strong.OZJ": "20D63B6AF959B008F7EE257E0D0727963AA34739DDE24FDE9E003F53B2E6C303",
    "blur02_mono_long_van2.OZJ": "7818B68969D2B134123CE58A81B7E2950ED6C2B6853F4560631A238033E8F689",
    "blur02_mono_long_van.OZJ": "055F4D6F94807F3F507DC84A9CBED509F05E4F3C65E1CF0D49E0BF040AB18FB9",
    "bet_grilsshot2red.OZJ": "E70B6AA393E9EED346F4B3CD423B1EE5F942E2BB22920D1ABEA461F72B6ADD40",
    # S21 0x678 subtype 4 overlays native bitmap 0x82FA, registered by
    # loader 0x18C067C from Effect\\bet_grilsshot2gold.jpg.
    "bet_grilsshot2gold.OZJ": "9CA43326D7261352C245DA4BE4C0D45272E026C7060D321B966753C5EAD96269",
    "Impack03.OZJ": "22F9F2481C6B7CB6CE4A4D674E72A35D72DC20D20B850674B3C06AE3BE84019D",
    "pin_star.OZJ": "07638DBE1BC4D993915E51EF121A03E8977AF57F107D9698C91EF2C1B0270549",
    # S21 loader VA 0x018BE786 maps native bitmap 0x8020 to WATERFALL4.
    "waterFall4.OZJ": "4650F6571447C1A4D4CCA81E9387A70B9B489AA4989820AF7D375855895B44D2",
}

ITEM_TEXTURES = {
    # S21 id 0x80F0 is intentionally an Item texture reused by effect 0x67E.
    "partCharge3/jujug_R.OZJ": "CE84410D9E044B42F0B1379FA9E39A062D3AD5F927D0B594354FF187662A3AD9",
}

SKILL_TEXTURES = {
    "ground_star.OZJ": "00FD18620AF56137C1EBDBF18DECD32AB93AC2529A46FECE011A1482039C7E4C",
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


def mesh_texture_references(plaintext: bytes, mesh_count: int) -> list[str]:
    """Read the authored mesh texture names from the decrypted S21 BMD."""
    names = sorted({
        match.decode("ascii")
        for match in re.findall(
            rb"[ -~]{3,64}\.(?:bmp|tga|jpg|png)", plaintext, re.I
        )
    })
    if mesh_count and not names:
        raise ValueError("S21 model has meshes but no embedded texture names")
    if len(names) > mesh_count:
        raise ValueError(
            f"S21 model has {mesh_count} meshes but {len(names)} texture names"
        )
    for name in names:
        source_name = Path(name).name
        if Path(source_name).suffix.lower() != ".jpg":
            raise ValueError(f"unmapped S21 mesh texture format: {name}")
        staged_name = Path(source_name).stem + ".OZJ"
        if staged_name.lower() not in {key.lower() for key in TEXTURES}:
            raise ValueError(f"S21 mesh texture absent from Slayer manifest: {name}")
    return names


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
        mesh_textures = mesh_texture_references(
            plaintext, identity["meshCount"]
        )
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
                "meshTextures": mesh_textures,
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

    skill_source_root = SOURCE_ROOT.parent / "Skill"
    for filename, expected_hash in SKILL_TEXTURES.items():
        source = skill_source_root / filename
        source_bytes = source.read_bytes()
        actual_hash = converter.sha256(source_bytes)
        if actual_hash != expected_hash:
            raise ValueError(f"source hash mismatch for {filename}: {actual_hash}")
        (effect_root / filename).write_bytes(source_bytes)
        report.append({"file": filename, "sourceSha256": actual_hash, "kind": "texture"})

    item_source_root = SOURCE_ROOT.parent / "Item"
    for relative, expected_hash in ITEM_TEXTURES.items():
        source = item_source_root / relative
        source_bytes = source.read_bytes()
        actual_hash = converter.sha256(source_bytes)
        if actual_hash != expected_hash:
            raise ValueError(f"source hash mismatch for {relative}: {actual_hash}")
        output_name = Path(relative).name
        (effect_root / output_name).write_bytes(source_bytes)
        report.append({"file": output_name, "sourceSha256": actual_hash, "kind": "texture"})

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
