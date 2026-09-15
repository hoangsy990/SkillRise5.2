#!/usr/bin/env python3
"""Verify the isolated QA client is a complete base-plus-private overlay."""

from __future__ import annotations

import hashlib
import os
import re
import zlib
from pathlib import Path


ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
SOURCE_CLIENT = ROOT / "Client"
PRIVATE_CLIENT = ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "Client"
TARGET = ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "RuntimeQA" / "Client"
ENGINE_SHA = "0E16EA7059B2FEA8DC89DAFE00F43983CABB3269A0666D9C1EADEC87F9B3E437"
PLAYER_SHA = "0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63"
PLAYER_CRC32 = 0xE51E1780


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def is_reparse(path: Path) -> bool:
    if hasattr(os.path, "isjunction") and os.path.isjunction(path):
        return True
    attributes = getattr(path.stat(), "st_file_attributes", 0)
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


def main(expected_engine_sha: str = ENGINE_SHA) -> None:
    expected = TARGET.resolve()
    assert expected == TARGET, "unexpected QA target resolution"
    assert not is_reparse(TARGET / "Data" / "RISE"), "RISE must be a private directory"
    assert not is_reparse(TARGET / "Data" / "Player"), "Player must be a private directory"
    assert not (TARGET / "Engine.exe").exists(), "legacy Engine.exe must not remain in isolated QA client"
    assert sha256(TARGET / "Engine-Port S21.exe") == expected_engine_sha
    player = TARGET / "Data" / "Player" / "player.bmd"
    assert sha256(player) == PLAYER_SHA
    assert zlib.crc32(player.read_bytes()) == PLAYER_CRC32

    capacity_header = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerRuntimeCapacity.h"
    crc_match = re.search(
        r"kMergedPlayerBmdCrc32\s*=\s*0x([0-9A-Fa-f]+)u",
        capacity_header.read_text(encoding="utf-8"),
    )
    assert crc_match, "merged Player.bmd CRC contract is missing"
    assert int(crc_match.group(1), 16) == PLAYER_CRC32

    expected_junctions = {
        source_dir.name
        for source_dir in (SOURCE_CLIENT / "Data").iterdir()
        if source_dir.is_dir() and source_dir.name not in {"Player", "RISE"}
    }
    actual_junctions = {
        target_dir.name
        for target_dir in (TARGET / "Data").iterdir()
        if target_dir.is_dir() and is_reparse(target_dir)
    }
    assert actual_junctions == expected_junctions, (
        f"QA Data junction set mismatch; extra={sorted(actual_junctions - expected_junctions)}, "
        f"missing={sorted(expected_junctions - actual_junctions)}"
    )
    junction_count = 0
    for source_dir in (SOURCE_CLIENT / "Data").iterdir():
        if source_dir.name not in expected_junctions:
            continue
        target_dir = TARGET / "Data" / source_dir.name
        assert is_reparse(target_dir), f"frozen Data junction missing: {source_dir.name}"
        assert target_dir.resolve() == source_dir.resolve(), (
            f"Data junction escaped frozen worktree snapshot: {source_dir.name}"
        )
        junction_count += 1

    base_count = verify_tree(
        SOURCE_CLIENT / "Data" / "RISE",
        TARGET / "Data" / "RISE",
        "base RISE",
    )
    overlay_count = verify_tree(
        PRIVATE_CLIENT / "Data" / "RISE" / "GrowLancer",
        TARGET / "Data" / "RISE" / "GrowLancer",
        "Grow Lancer overlay",
    )
    root_data_count = 0
    current_overlay_count = verify_tree(
        ROOT / "Client" / "Data" / "RISE" / "GrowLancer",
        TARGET / "Data" / "RISE" / "GrowLancer",
        "current Grow Lancer assets",
    )
    print(f"PASS: current Grow Lancer assets ({current_overlay_count} files)")
    for source_file in (SOURCE_CLIENT / "Data").iterdir():
        if not source_file.is_file():
            continue
        target_file = TARGET / "Data" / source_file.name
        assert target_file.is_file(), f"root Data file missing: {source_file.name}"
        assert sha256(target_file) == sha256(source_file), (
            f"root Data hash mismatch: {source_file.name}"
        )
        root_data_count += 1
    assert (TARGET / "Data" / "Enc1.dat").is_file()
    assert (TARGET / "Data" / "Dec2.dat").is_file()
    assert (TARGET / "Data" / "RISE" / "Config" / "Mix.bmd").is_file()
    print(f"PASS: complete private base RISE tree ({base_count} files)")
    print(f"PASS: complete private Grow Lancer overlay ({overlay_count} files)")
    print(f"PASS: complete root Data file set ({root_data_count} files), including login keys")
    print(f"PASS: {junction_count} Data junctions target the frozen worktree snapshot")
    print("PASS: Engine/player hashes, merged-player CRC and non-reparse Player/RISE roots")


if __name__ == "__main__":
    main()
