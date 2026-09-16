"""Trace isolated native final-speed packet into client recalculation gap."""

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "ExMain_RISE_PC/Main5.2_RISE"
SERVER = ROOT / "ExGameServer/GameServer"


def read(path: Path) -> str:
    return path.read_text(encoding="latin-1")


def main() -> None:
    gs = read(SERVER / "ObjectManager.cpp")
    base = gs[gs.index("lpObj->PhysiSpeed = Dexterity / gServerInfo.m_DWPhysiSpeedConstA"):]
    base = base[:base.index("lpObj->PhysiSpeed += lpObj->DrinkSpeed")]
    assert "lpObj->Class == CLASS_RF" in base
    assert not re.search(r"lpObj->Class\s*==\s*CLASS_GL\b", base)
    ds = read(SERVER / "DSProtocol.cpp")
    assert "pMsg.SpeedAttack = (DWORD)(lpObj->PhysiSpeed);" in ds
    assert "pMsg.MagicAttack = (DWORD)(lpObj->MagicSpeed);" in ds
    protocol = read(SERVER / "Protocol.h")
    assert re.search(r"\bDWORD\s+SpeedAttack\s*;", protocol)
    assert re.search(r"\bDWORD\s+MagicAttack\s*;", protocol)

    ws = read(CLIENT / "WSclient.cpp")
    join = ws[ws.index("BOOL ReceiveJoinMapServer("):]
    assert join.index("CharacterAttribute->AttackSpeed") < join.index("Hero = c;")
    assert "CharacterAttribute->AttackSpeed\t\t\t= Data->SpeedAttack;" in join
    assert "CharacterAttribute->MagicSpeed\t\t\t= Data->MagicAttack;" in join
    after_hero = join[join.index("Hero = c;"):]
    assert "SetCharacterClass(c);" in after_hero[:after_hero.index("SetPlayerStop(c);")]
    actor = read(CLIENT / "ZzzCharacter.cpp")
    equipment = actor[actor.index("void SetCharacterClass("):actor.index("void SetChangeClass(")]
    assert "if (c == Hero)" in equipment and "CharacterMachine->CalculateAll();" in equipment

    info = read(CLIENT / "ZzzInfomation.cpp")
    speed = info[info.index("void CHARACTER_MACHINE::CalculateAttackSpeed()"):]
    speed = speed[:speed.index("void CHARACTER_MACHINE::CalculateSuccessfulBlocking()")]
    before_items = speed[:speed.index("ITEM* r =")]
    assert "case CLASS_RAGEFIGHTER: index = 12; break;" in before_items
    assert not re.search(r"case\s+CLASS_GROWLANCER\b", before_items)
    assert "if (index >= 0 && speed[index] != 0 && speed[index + 1] != 0)" in before_items
    assert "Character.AttackSpeed += RightWeaponSpeed;" in speed
    assert "Character.MagicSpeed += RightWeaponSpeed;" in speed
    all_calc = info[info.index("void CHARACTER_MACHINE::CalculateAll()"):]
    assert "CalculateAttackSpeed();" in all_calc[:all_calc.index("CalculateSuccessfulBlocking();")]
    print("PASS native GS computed final speed -> join DWORD -> Hero set -> SetCharacterClass/CalculateAll")
    print("GAP class7 base setter absent in both native GS/client; client additive recalc can reuse final join speed")
    print("Scope: source call paths; S21 base rate, join wire equivalence and actual runtime repetition remain OPEN")


if __name__ == "__main__":
    main()
