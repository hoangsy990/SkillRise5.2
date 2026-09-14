#!/usr/bin/env python3
"""Verify the isolated Slayer QA client and its base data merge."""

from __future__ import annotations

import hashlib
import os
from pathlib import Path


ROOT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer")
SOURCE_CLIENT = Path(r"D:\RISE-CrossPlatform\Client")
PRIVATE_CLIENT = ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "Client"
TARGET = ROOT / "ExMain_RISE_PC" / "Tests" / "SlayerBuild" / "RuntimeQA" / "Client"


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def is_reparse(path: Path) -> bool:
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


def main() -> None:
    assert TARGET.resolve() == TARGET, "unexpected QA target resolution"
    assert not is_reparse(TARGET / "Data" / "RISE"), "RISE must be a private directory"
    assert not is_reparse(TARGET / "Data" / "Player"), "Player must be a private directory"
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
    print(f"PASS: complete private base Player tree ({player_count} base files plus Slayer player.bmd)")
    print(f"PASS: complete root Data file set ({root_data_count} files), including login keys")
    print("PASS: Engine-Slayer S21, Player/RISE roots, Mix.bmd and no GrowLancer overlay")


if __name__ == "__main__":
    main()
