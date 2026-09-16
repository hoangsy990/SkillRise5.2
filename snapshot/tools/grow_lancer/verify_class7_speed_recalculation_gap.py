"""Pin the native class-7 speed initialization gap without selecting a rate."""
import hashlib
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
S21 = Path(r"D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua")
source = S21.read_bytes()
assert hashlib.sha256(source).hexdigest().upper() == (
    "9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70"
)
lua = source.decode("utf-8-sig")
speed = lua.split("function CalcAttackSpeed(", 1)[1].split("\nend", 1)[0]
gl = speed.split("elseif(Class == CLASS_GROWLANCER) then", 1)[1].split("elseif", 1)[0]
assert "AttackSpeed = Dexterity / 20" in gl
assert "MagicSpeed = Dexterity / 20" in gl
assert "Anti-Hack purpose only" in lua.split("function CalcAttackSpeed(", 1)[0][-250:]

client = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzInfomation.cpp").read_text(
    encoding="latin-1"
)
body = client.split("void CHARACTER_MACHINE::CalculateAttackSpeed()", 1)[1].split(
    "void CHARACTER_MACHINE::CalculateSuccessfulBlocking()", 1
)[0]
before_equipment = body.split("ITEM* r =", 1)[0]
assert "int index = -1;" in before_equipment
assert "case CLASS_RAGEFIGHTER: index = 12; break;" in before_equipment
assert not re.search(r"case\s+CLASS_GROWLANCER\b", before_equipment)
assert "if (index >= 0 && speed[index] != 0 && speed[index + 1] != 0)" in before_equipment
assert "Character.AttackSpeed += 20;" in body

server = (ROOT / "ExGameServer/GameServer/ObjectManager.cpp").read_text(
    encoding="latin-1"
)
start = server.index("lpObj->PhysiSpeed = Dexterity / gServerInfo.m_DWPhysiSpeedConstA")
end = server.index("lpObj->PhysiSpeed += lpObj->DrinkSpeed", start)
base = server[start:end]
assert "lpObj->Class == CLASS_RF" in base
assert not re.search(r"lpObj->Class\s*==\s*CLASS_GL\b", base)
assert "lpObj->PhysiSpeed =" in base and "lpObj->MagicSpeed =" in base

print("PASS pinned S21 CalcAttackSpeed GL DEX/20 and its anti-hack qualification")
print("PASS native client and GS base-speed setters omit class7 before additive terms")
print("OPEN rate/rounding, S21 GS caller, native client stat source and class7 activation")
