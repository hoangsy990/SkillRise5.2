#!/usr/bin/env python3
"""Verify the isolated Slayer QA client and its base data merge."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path


ROOT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer")
SOURCE_CLIENT = ROOT / "Client"
PRIVATE_CLIENT = ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "Client"
TARGET = ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "RuntimeQA" / "Client"
MASTER_SLAYER_HASHES = {
    "Config/MasterSlayerTree.bmd": "00C02A1E4CAA84BFAB603DCAC7545C2B65E05390615BF5A81F867807656CA0DC",
    "Config/MasterSlayerTooltip.bmd": "11ECF321341659F14CC606DDADC1B463D4F3A95B637B2904C2E58F38DFA4CCF3",
    "Config/MasterSlayerSkills.bmd": "1A8932E3C3CDE1155F3DBFF426F627758293132CAA7ED75C54B9CAB559CC806C",
    "Interface/new_Master_Icon.OZJ": "DF3D1F863741E720EFC7B9ECC90117BB1BE9CA49852A86420FA8CA3E396D9F74",
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def is_reparse(path: Path) -> bool:
    # stat() follows a junction and hides its reparse-point attribute.
    attributes = getattr(path.lstat(), "st_file_attributes", 0)
    return bool(attributes & getattr(os.stat_result, "FILE_ATTRIBUTE_REPARSE_POINT", 0x400))


def verify_tree(source: Path, target: Path, label: str) -> int:
    count = 0
    for source_file in source.rglob("*"):
        if not source_file.is_file():
            continue
        relative = source_file.relative_to(source)
        target_file = target / relative
        assert target_file.is_file(), f"{label} missing: {relative}"
        assert sha256(target_file) == sha256(source_file), f"{label} hash mismatch: {relative}"
        count += 1
    return count


def main() -> None:
    assert TARGET.resolve() == TARGET, "unexpected QA target resolution"
    assert not is_reparse(TARGET / "Data" / "RISE"), "RISE must be a private directory"
    assert not is_reparse(TARGET / "Data" / "Player"), "Player must be a private directory"
    source_data = SOURCE_CLIENT / "Data"
    stage_data = TARGET / "Data"
    source_links = {
        entry.name: entry for entry in source_data.iterdir()
        if entry.is_dir() and entry.name not in {"Player", "RISE"}
    }
    staged_links = {
        entry.name: entry for entry in stage_data.iterdir()
        if entry.is_dir() and is_reparse(entry)
    }
    assert staged_links.keys() == source_links.keys(), (
        f"QA junction set mismatch: expected={len(source_links)} staged={len(staged_links)} "
        f"extra={sorted(staged_links.keys() - source_links.keys())} "
        f"missing={sorted(source_links.keys() - staged_links.keys())}"
    )
    for name, source_dir in source_links.items():
        assert not is_reparse(source_dir), f"source asset directory is not frozen: {name}"
        assert staged_links[name].resolve().as_posix().casefold() == (
            source_dir.resolve().as_posix().casefold()
        ), f"QA asset junction escapes Slayer worktree: {name}"
    assert not (TARGET / "Engine.exe").exists(), "legacy Engine.exe must not remain in isolated QA client"
    assert (TARGET / "Engine-Slayer S21.exe").is_file(), "Engine-Slayer S21.exe missing"

    player = PRIVATE_CLIENT / "Data" / "Player" / "player.bmd"
    assert sha256(TARGET / "Data" / "Player" / "player.bmd") == sha256(player)
    player_count = 0
    for source_file in (SOURCE_CLIENT / "Data" / "Player").rglob("*"):
        if not source_file.is_file() or source_file.name.lower() == "player.bmd":
            continue
        relative = source_file.relative_to(SOURCE_CLIENT / "Data" / "Player")
        target_file = TARGET / "Data" / "Player" / relative
        assert target_file.is_file(), f"base Player missing: {relative}"
        assert sha256(target_file) == sha256(source_file), f"base Player hash mismatch: {relative}"
        player_count += 1
    base_count = verify_tree(SOURCE_CLIENT / "Data" / "RISE", TARGET / "Data" / "RISE", "base RISE")
    slayer_count = verify_tree(
        PRIVATE_CLIENT / "Data" / "RISE" / "Slayer",
        TARGET / "Data" / "RISE" / "Slayer",
        "private Slayer",
    )
    for relative, expected in MASTER_SLAYER_HASHES.items():
        asset = TARGET / "Data" / "RISE" / "Slayer" / relative
        assert asset.is_file() and sha256(asset) == expected, (
            f"private Master Slayer SS21 asset missing/drifted: {relative}"
        )

    root_data_count = 0
    for source_file in (SOURCE_CLIENT / "Data").iterdir():
        if not source_file.is_file():
            continue
        target_file = TARGET / "Data" / source_file.name
        assert target_file.is_file(), f"root Data file missing: {source_file.name}"
        assert sha256(target_file) == sha256(source_file), f"root Data hash mismatch: {source_file.name}"
        root_data_count += 1

    assert (TARGET / "Data" / "Enc1.dat").is_file()
    assert (TARGET / "Data" / "Dec2.dat").is_file()
    assert (TARGET / "Data" / "RISE" / "Config" / "Mix.bmd").is_file()
    assert not (TARGET / "Data" / "RISE" / "GrowLancer").exists(), "GrowLancer overlay leaked into Slayer QA"
    print(f"PASS: complete private base RISE tree ({base_count} files)")
    print(f"PASS: complete private Slayer overlay ({slayer_count} files)")
    print("PASS: S21 Master Slayer tree/tooltip/metadata/icon exact hashes")
    print(f"PASS: complete private base Player tree ({player_count} base files plus Slayer player.bmd)")
    print(f"PASS: complete root Data file set ({root_data_count} files), including login keys")
    print(f"PASS: {len(staged_links)} asset junctions target only frozen Slayer Data")
    print("PASS: Engine-Slayer S21, Player/RISE roots, Mix.bmd and no GrowLancer overlay")


if __name__ == "__main__":
    main()
