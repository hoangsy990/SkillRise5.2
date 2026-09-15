"""Pin S21 regular GL Lua expressions and the fail-closed native formula slice."""

from pathlib import Path
import hashlib
import re


ROOT = Path(__file__).resolve().parents[2]
LUA = Path(r"D:\GameServer S21\Data\Skills\RegularSkillCalc.lua")
LUA_SHA = "F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847"
HEADER = ROOT / "Shared/GrowLancerSkillContractData.h"
TEST = ROOT / "ExGameServer/Tests/GrowLancerServerContractTest.cpp"
MANAGER = ROOT / "ExGameServer/GameServer/SkillManager.cpp"
CATALOG = ROOT / "ExGameServer/GameServer/RISE/GrowLancerServerCatalog.h"


def normalized(value: str) -> str:
    return re.sub(r"\s+", "", value)


def source_function(lua: str, name: str) -> str:
    match = re.search(rf"(?ms)^function {name}\([^\n]+\)\n(.*?)^end\s*$", lua)
    assert match is not None, name
    return normalized(match.group(1))


def main() -> None:
    assert hashlib.sha256(LUA.read_bytes()).hexdigest().upper() == LUA_SHA
    lua = LUA.read_text(encoding="utf-8-sig")
    header = normalized(HEADER.read_text(encoding="utf-8"))
    tests = TEST.read_text(encoding="utf-8")
    source_contract = {
        "GrowLancerSpinStep": ("InDamage*1.1", "InDamage*0.7",
                               "Dexterity/10+97+SkillTreeBonus"),
        "GrowLancerHarshStrike": ("BarrageCount==1", "BarrageCount==2",
                                    "InDamage*1.0", "InDamage*1.1",
                                    "Strength/10+97+SkillTreeBonus"),
        "GrowLancerMagicPin": ("BarrageCount==1", "BarrageCount==2",
                               "BarrageCount==3", "InDamage*0.8",
                               "InDamage*1.0", "InDamage*1.1",
                               "Dexterity/10+97+SkillTreeBonus"),
        "GrowLancerBreche": ("InDamage*0.5",
                              "Strength/10+97+SkillTreeBonus"),
        "GrowLancerShiningPeak": ("Strength/10+97+SkillTreeBonus_Retailation",
                                   "Dexterity/10+97+SkillTreeBonus_Rage",
                                   "OutDamage=OutDamage/3"),
        "GrowLancerObsidian": ("Strength/20", "SkillTime=240"),
    }
    for name, fragments in source_contract.items():
        body = source_function(lua, name)
        for fragment in fragments:
            assert fragment in body, (name, fragment)
    assert "ApplyRegularDamageFormula" in header and "ApplyRegularDamageFormula" in tests
    assert "ApplyObsidianRegularFormula" in header and "ApplyObsidianRegularFormula" in tests
    for expression in ("hitIndex==0?1.1:0.7", "hitIndex==0?1.0:1.1",
                       "hitIndex<=0?0.8", "hitIndex==1?1.0:1.1",
                       "inDamage*0.5", "strength/20.0", "240.0",
                       "strengthTreeBonus", "dexterityTreeBonus"):
        assert expression in header, expression
    for guard in ("if(hitIndex<0||hitIndex>1)return0.0;",
                  "if(hitIndex<0||hitIndex>2)return0.0;"):
        assert guard in header, guard
    assert LUA_SHA in HEADER.read_text(encoding="utf-8")
    for boundary in ("Spin Step Lua returns exactly two damage outputs",
                     "Harsh Strike Lua default barrage damage is zero",
                     "Magic Pin Lua default barrage damage is zero",
                     "Obsidian Strength/20 retains Lua fractional result"):
        assert boundary in tests, boundary
    # This is a formula import only: no guessed hit producer/rounding is
    # allowed to activate the server-authoritative cast path yet.
    assert "HasProvenServerRuntimeHandler(lpSkill->m_index)" in MANAGER.read_text(
        encoding="utf-8")
    assert "info.RequireClass[n] = 0" in CATALOG.read_text(encoding="utf-8")
    print("PASS pinned S21 regular Lua source, existing native double formulas and barrage bounds")
    print("OPEN handler/hit sequencing, buff recipient/duration, integer bridge, GL class and cast authority")


if __name__ == "__main__":
    main()
