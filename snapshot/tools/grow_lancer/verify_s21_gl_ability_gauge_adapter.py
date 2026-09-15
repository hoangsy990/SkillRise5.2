"""Pin the S21 Lua GL AG expression against an isolated native adapter."""

import hashlib
from pathlib import Path

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
LUA = Path(r"D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua")
LUA_SHA = "9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70"


def main() -> None:
    data = LUA.read_bytes()
    assert hashlib.sha256(data).hexdigest().upper() == LUA_SHA
    source = data.decode("utf-8-sig")
    assert "CLASS_GROWLANCER = 7" in source
    function = source.split("function CalcAbilityGauge(", 1)[1].split("\nend", 1)[0]
    selected = function.split("elseif(Class == CLASS_GROWLANCER) then", 1)[1].split("elseif", 1)[0]
    assert [line.strip() for line in selected.splitlines() if line.strip()] == [
        "AG = (Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0)"
    ]
    assert "return AG" in function
    header = (ROOT / "ExGameServer/GameServer/RISE/GrowLancerAbilityGauge.h").read_text()
    assert "strength * .15 + dexterity * .2 + vitality * .3 + energy * 1.0" in header
    assert "effective" in header and "MaxBP rounding" in header
    server = (ROOT / "ExGameServer/GameServer/ObjectManager.cpp").read_text()
    assert "CalcGrowLancerAbilityGauge(" not in server
    print("PASS: S21 GL AG Lua branch pinned and native numeric expression adapter exact")
    print("OPEN: effective-stat bridge, MaxBP rounding, class activation and runtime server parity")


if __name__ == "__main__":
    main()
