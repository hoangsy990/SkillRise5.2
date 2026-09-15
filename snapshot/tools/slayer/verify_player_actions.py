#!/usr/bin/env python3
"""Verify the five S21 Slayer action mappings in the isolated player model.

Demolish uses S21 action 233. The supplied player dump proves that its keys
are byte-identical to Detection's 232, but the native updater still gates on
distinct E8/E9 action IDs, so the merged model preserves both slots.
"""

from __future__ import annotations

import importlib.util
import hashlib
import re
import sys
import zlib
from pathlib import Path


MERGER_PATH = Path(
    r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\tools\grow_lancer\merge_player_actions.py"
)
S21 = Path(r"D:\MU FICA Season 21\Data\Player\player.bmd")
MERGED = Path(
    r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild"
    r"\Client\Data\Player\player.bmd"
)
MAPPING = ((224, 284), (227, 285), (228, 286), (232, 287), (233, 288))
EXPECTED_MERGED_SHA256 = "bf6f5c0f029e1af6ae78c805e1aac56dcc2b3c6a37675a61caeed36c95d41d25"
CRC_SOURCE = Path(
    r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Main5.2_RISE"
    r"\RISE\ZzzToolKit.cpp"
)


def load_merger():
    spec = importlib.util.spec_from_file_location("slayer_merger_verify", MERGER_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load merger: {MERGER_PATH}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def main() -> None:
    merged_bytes = MERGED.read_bytes()
    merged_sha = hashlib.sha256(merged_bytes).hexdigest()
    assert merged_sha == EXPECTED_MERGED_SHA256, merged_sha
    merged_crc = zlib.crc32(merged_bytes)
    source_text = CRC_SOURCE.read_text(encoding="utf-8", errors="replace")
    player_crc_pins = re.findall(r"CheckFileCRC\(filePath,\s*0x([0-9a-fA-F]+)u\)", source_text)
    assert f"{merged_crc:08x}" in [value.lower() for value in player_crc_pins], (
        hex(merged_crc), player_crc_pins
    )
    print(f"PASS: five-action player.bmd sha256={merged_sha} CRC32=0x{merged_crc:08X} matches client guard")
    merger = load_merger()
    source = merger.parse(merger.decode(S21))
    merged = merger.parse(merger.decode(MERGED))
    assert merged.action_count == 289, merged.action_count
    source_check = merger.COMPARER.parse_zero_mesh_skeleton(source.payload)
    merged_check = merger.COMPARER.parse_zero_mesh_skeleton(merged.payload)
    for source_id, merged_id in MAPPING:
        assert source_check.action_hashes[source_id] == merged_check.action_hashes[merged_id], (
            source_id,
            merged_id,
        )
        print(
            f"PASS: S21 action {source_id} -> 5.2 action {merged_id} "
            f"sha256={merged_check.action_hashes[merged_id]}"
        )
    assert merged_check.action_hashes[287] == merged_check.action_hashes[288]
    print("PASS: Detection/Demolish clips match but action IDs 287/288 differ")
    # The native action record has a dedicated position-track flag at byte 2.
    # Pierce's seven-key clip carries no world-position track; any dash/return
    # must be resolved outside that player.bmd action record.
    assert int.from_bytes(source.action_records[228][:2], "little") == 7
    assert int.from_bytes(merged.action_records[286][:2], "little") == 7
    assert source.action_records[228][2] == 0
    assert merged.action_records[286][2] == 0
    print("PASS: Pierce action 228/286 keys=7 position-track=absent")


if __name__ == "__main__":
    main()
