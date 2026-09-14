#!/usr/bin/env python3
"""Verify the five S21 Slayer action mappings in the isolated player model.

Demolish uses S21 action 233.  The supplied player dump proves that 233 is
byte-identical to 232, so the merged model intentionally reuses the Detection
clip at 287 instead of adding a duplicate ABI slot.
"""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


MERGER_PATH = Path(
    r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\tools\grow_lancer\merge_player_actions.py"
)
S21 = Path(r"D:\MU FICA Season 21\Data\Player\player.bmd")
MERGED = Path(
    r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild"
    r"\Client\Data\Player\player.bmd"
)
MAPPING = ((224, 284), (227, 285), (228, 286), (232, 287), (233, 287))


def load_merger():
    spec = importlib.util.spec_from_file_location("slayer_merger_verify", MERGER_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load merger: {MERGER_PATH}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def main() -> None:
    merger = load_merger()
    source = merger.parse(merger.decode(S21))
    merged = merger.parse(merger.decode(MERGED))
    assert merged.action_count == 288, merged.action_count
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


if __name__ == "__main__":
    main()
