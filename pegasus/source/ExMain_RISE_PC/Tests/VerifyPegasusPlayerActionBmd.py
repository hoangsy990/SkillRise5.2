"""Verify the isolated player-action carrier against the pinned Pegasus BMD.

This is an action-payload provenance gate, not an ingame animation PASS.
"""

from __future__ import annotations

import hashlib
from pathlib import Path

from CompareBmdActions import read_actions


PEGASUS = Path(r"D:\Pegasus\Data\Player\player.bmd")
RISE = Path(r"D:\RISE-CrossPlatform\Client\Data\Player\player.bmd")
STAGED = Path(__file__).parent / (
    r"PegasusBuild\Client\Data\RISE\Pegasus\Recovered\Data\Player\player.bmd"
)
PEGASUS_SHA256 = "FF75CA280C6633015F11D1A1304286FA6362D5A82BFD9CB1119E2FAD2D69B1B5"
RISE_SHA256 = "2964E7C41DCF686B79E53A74261192EBFC6879D36BC030C2E503EF0B8CEF38EF"
NATIVE_ACTIONS = 284
PEGASUS_ACTIONS = 287

REQUIRED_ACTIONS = {
    157: "Spiral Charge Uniria",
    158: "Spiral Charge Dinorant",
    159: "Spiral Charge Fenrir",
    67: "buff family and Sword Wrath",
    71: "Sword Blow",
    80: "Spearstorm",
    145: "Darkness",
    172: "Deathside summon",
    173: "Deathside summon Uniria",
    174: "Deathside summon Dinorant",
    175: "Deathside summon Fenrir",
    183: "Chaos Blade and Havoc Spear",
    185: "Deathside repeat command",
    253: "Dragon Violent",
}


def digest(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> int:
    if digest(PEGASUS) != PEGASUS_SHA256:
        raise SystemExit("pinned Pegasus player.bmd hash changed")
    if digest(RISE) != RISE_SHA256:
        raise SystemExit("native RISE player.bmd hash changed")

    pegasus_bones, pegasus = read_actions(PEGASUS)
    rise_bones, rise = read_actions(RISE)
    if pegasus_bones != 60 or rise_bones != 60:
        raise SystemExit("player skeleton bone count changed")
    if len(rise) != NATIVE_ACTIONS or len(pegasus) != PEGASUS_ACTIONS:
        raise SystemExit("player action count changed")
    if rise != pegasus[:NATIVE_ACTIONS]:
        raise SystemExit("Pegasus is no longer an exact action superset of RISE")
    for action, role in REQUIRED_ACTIONS.items():
        if rise[action] != pegasus[action]:
            raise SystemExit(f"action {action} mismatch for {role}")

    if pegasus[286][0] != 6:
        raise SystemExit("Raining Arrow action 286 key count changed")
    if not STAGED.is_file() or digest(STAGED) != PEGASUS_SHA256:
        raise SystemExit("isolated staged Pegasus player-action carrier is missing")

    print("PASS: first 284 actions are exact; staged Pegasus tail adds 284..286")
    print("PASS: Raining Arrow action 286 has 6 keys; Deathside actions are exact")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
