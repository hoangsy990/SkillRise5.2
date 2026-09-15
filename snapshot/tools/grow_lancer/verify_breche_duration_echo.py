"""Static guard for the isolated Breche target-bearing 5.2 duration echo.

This verifies transport shape and fail-closed skill behavior, not GS damage or
runtime acceptance.  Existing SS6 0x1E packets must keep their original size.
"""
from pathlib import Path

root = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
server_h = (root / "ExGameServer/GameServer/SkillManager.h").read_text(errors="replace")
server = (root / "ExGameServer/GameServer/SkillManager.cpp").read_text(errors="replace")
client_h = (root / "ExMain_RISE_PC/Main5.2_RISE/WSclient.h").read_text(errors="replace")
client = (root / "ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp").read_text(errors="replace")
protocol = (root / "ExGameServer/GameServer/Protocol.cpp").read_text(errors="replace")

legacy = server_h.split("struct PMSG_DURATION_SKILL_ATTACK_SEND", 1)[1].split("};", 1)[0]
extended = server_h.split("struct PMSG_GROW_LANCER_DURATION_SKILL_ATTACK_SEND", 1)[1].split("};", 1)[0]
assert "BYTE target[2]" not in legacy
assert "BYTE target[2]" in extended
assert extended.index("BYTE dir;") < extended.index("BYTE target[2];")
assert "pMsg.header.set(0x1E, sizeof(pMsg));" in server.split(
    "void CSkillManager::GCGrowLancerDurationSkillAttackSend", 1)[1]
assert "GCGrowLancerDurationSkillAttackSend(lpObj, lpSkill->m_index," in server
assert "lpSkill->m_skill == SKILL_SHINING_PEAK || lpSkill->m_skill == SKILL_BRECHE" in server
assert "this->GCDurationSkillAttackSend(lpObj, lpSkill->m_index, x, y, dir);" in server
duration = server.split("void CSkillManager::UseDurationSkillAttack(", 1)[1].split(
    "bool CSkillManager::RunningSkill(", 1)[0]
assert duration.index("this->RunningSkill(aIndex, bIndex, lpSkill, x, y, angle, combo) == 0") < duration.index(
    "this->GCGrowLancerDurationSkillAttackSend(lpObj, lpSkill->m_index,")
assert duration.index("this->GCGrowLancerDurationSkillAttackSend(lpObj, lpSkill->m_index,") < duration.index(
    "this->GCDurationSkillAttackSend(lpObj, lpSkill->m_index, x, y, dir);")
assert "!rise::growlancer::HasProvenServerRuntimeHandler" in server
assert "return false;" in server.split("case SKILL_BRECHE:", 1)[1].split(
    "case SKILL_BLOOD_STORM:", 1)[0]
assert "case 0x1E:" in protocol and "case 0x57:" in protocol
assert "CGGuildMasterCreateCancel(aIndex)" in protocol or "CGGuildMasterCreateCancel" in protocol

prefix = client_h.split("} PRECEIVE_MAGIC_CONTINUE", 1)[0].split("//receive magic continue", 1)[1]
extension = client_h.split("} PRECEIVE_GROW_LANCER_MAGIC_CONTINUE", 1)[0].split(
    "typedef struct {", 1)[1]
for field in ("MagicH", "MagicL", "KeyH", "KeyL", "PositionX", "PositionY", "Angle"):
    assert prefix.index(field) < prefix.index("Angle") or field == "Angle"
    assert field in extension
assert extension.index("Angle") < extension.index("TargetKeyH") < extension.index("TargetKeyL")
receive = client.split("BOOL ReceiveMagicContinue(", 1)[1].split("// ChainLightning", 1)[0]
assert "Size < static_cast<int>(sizeof(PRECEIVE_GROW_LANCER_MAGIC_CONTINUE))" in receive
assert "MagicNumber == rise::growlancer::kBrecheSkill" in receive
assert "TargetIndex == MAX_CHARACTERS_CLIENT" in receive
assert "if (!to->Live)" in receive
assert "CreateBrecheAction(*so);" in receive and "CreateBrecheHit(*to);" in receive
assert receive.index("MagicNumber == rise::growlancer::kBrecheSkill") < receive.index(
    "if (so->Type == MODEL_PLAYER)")
assert receive.index("Size < static_cast<int>(sizeof(PRECEIVE_GROW_LANCER_MAGIC_CONTINUE))") < receive.index(
    "FindCharacterIndex(Key)")
breche = receive.split("// S21 Breche's positional cast", 1)[1].split(
    "if (so->Type == MODEL_PLAYER)", 1)[0]
assert breche.index("if (!so->Live)") < breche.index("sc->Skill = MagicNumber;")
assert breche.index("if (!to->Live)") < breche.index("sc->Skill = MagicNumber;")
assert breche.index("sc->Skill = MagicNumber;") < breche.index("CreateBrecheAction(*so);")
print("PASS: Breche-only target-bearing native C1:1E echo, legacy prefix intact, GS damage still fail-closed")
