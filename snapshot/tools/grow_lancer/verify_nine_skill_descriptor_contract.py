"""Verify the complete nine-skill Grow Lancer descriptor contract.

This is a descriptor-integrity gate only.  It does not promote gameplay,
visual parity, class activation or GameServer authority.
"""

from __future__ import annotations

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
DESCRIPTOR = ROOT / "GrowLancer/grow_lancer_skills_descriptor.json"

EXPECTED = {
    271: ("Spin Step", 186, 285),
    272: ("Circle Shield", 187, 286),
    273: ("Obsidian", 191, 290),
    274: ("Magic Pin", 188, 287),
    275: ("Clash", 194, 293),
    276: ("Harsh Strike", 185, 284),
    277: ("Shining Peak", 189, 288),
    278: ("Wrath", 192, 291),
    279: ("Breche", 190, 289),
}


def main() -> None:
    descriptor = json.loads(DESCRIPTOR.read_text(encoding="utf-8"))
    rows = descriptor.get("skills")
    assert isinstance(rows, list) and len(rows) == len(EXPECTED), len(rows or [])
    by_id = {row.get("id"): row for row in rows}
    assert set(by_id) == set(EXPECTED), sorted(by_id)

    for skill_id, (name, action, rise_action) in EXPECTED.items():
        row = by_id[skill_id]
        assert row["name"] == name
        assert row["action"] == action
        assert row["riseAction"] == rise_action
        for field in ("mana", "ag", "damage", "range", "delayMs", "sound", "scroll", "reverseEvidence"):
            assert field in row, f"{skill_id}: missing {field}"
        assert isinstance(row["visualLayers"], list) and row["visualLayers"], f"{skill_id}: visualLayers"
        for layer in row["visualLayers"]:
            assert isinstance(layer, dict) and layer.get("role"), f"{skill_id}: malformed visual layer"

    print("PASS nine-skill descriptor contract: IDs 271..279, actions, costs, scrolls, sounds and visual layers")
    print("Scope: descriptor integrity only; runtime visual/server acceptance remains IN_PROCESS")


if __name__ == "__main__":
    main()
