#!/usr/bin/env python3
"""Append recovered Slayer S21 actions to the isolated Slayer player BMD.

The implementation is the already-audited Grow Lancer merger, with its output
boundary rebound to this Slayer worktree.  Inputs stay hash-pinned and the
merger proves that every legacy 5.2 action remains byte-identical.
"""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


SLAYER_ROOT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer").resolve()
MERGER = Path(
    r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\tools\grow_lancer"
    r"\merge_player_actions.py"
)


def main() -> int:
    spec = importlib.util.spec_from_file_location("slayer_player_merger", MERGER)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load audited merger: {MERGER}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    module.WORKTREE_ROOT = SLAYER_ROOT
    return module.main()


if __name__ == "__main__":
    raise SystemExit(main())
