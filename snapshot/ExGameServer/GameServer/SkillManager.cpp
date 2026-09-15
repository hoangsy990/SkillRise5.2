#include "stdafx.h"
#include "SkillManager.h"
#include "..\..\Include\Math.h"
#include "Attack.h"
#include "CastleSiege.h"
#include "DoubleGoer.h"
#include "Duel.h"
#include "EffectManager.h"
#include "EventKillAll.h"
#include "Fruit.h"
#include "GensSystem.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "Map.h"
#include "MasterSkillTree.h"
#include "MemScript.h"
#include "Monster.h"
#include "Move.h"
#include "MoveSummon.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Party.h"
#include "Quest.h"
#include "ServerInfo.h"
#include "SkillHitBox.h"
#include "Util.h"
#include "Viewport.h"
#include "RISE/RISE_MessLang.h"
#include "CustomMonster.h"
#include "RISE/RISE_StatsInfo.h"
#include <commctrl.h>
#include "resource.h"
#include "Path.h"
#include "GameServer.h"
#include "../../Addon/DualColor.h"
#include "RISE/SlayerServerCatalog.h"
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerSkillContractData.h"
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerBatFanoutWire.h"
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerPierceFanoutWire.h"
#include "../../ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerDetectionWire.h"
#include <map>
#include <mutex>
#include <deque>

namespace {

struct PendingSlayerPierce
{
	DWORD connectedAt;
	DWORD openedAt;
	BYTE serial;
	int map;
	int castX;
	int castY;
	int count;
	int target[rise::slayer::kPierceFanoutMaxTargets];
	bool consumed[rise::slayer::kPierceFanoutMaxTargets];
};

std::map<int, std::deque<PendingSlayerPierce> > gPendingSlayerPierce;
std::mutex gPendingSlayerPierceMutex;
BYTE gSlayerPierceSerial = 0;
const DWORD kSlayerPierceLaneWindowMs = 15000;
const size_t kSlayerPierceMaxPendingPerCaster = 32;

void PruneSlayerPierceCasts(std::deque<PendingSlayerPierce>& casts,
	DWORD connectedAt, DWORD now)
{
	for (auto it = casts.begin(); it != casts.end(); )
	{
		// Keep fully consumed and previous-map sessions as serial tombstones
		// until the replay window closes. Their lanes fail the normal map /
		// one-use checks, but an old packet cannot match a new cast serial.
		if (it->connectedAt != connectedAt ||
			now - it->openedAt > kSlayerPierceLaneWindowMs)
			it = casts.erase(it);
		else
			++it;
	}
}

}
CSkillManager gSkillManager;
CSkillManager::CSkillManager()
{
	this->m_SkillInfo.clear();
}
void CSkillManager::Load(char* path)
{
	CMemScript* lpMemScript = new CMemScript;
	if (lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR, path);
		return;
	}
	if (lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}
	this->m_SkillInfo.clear();
	try
	{
		while (true)
		{
			if (lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
			if (strcmp("end", lpMemScript->GetString()) == 0)
			{
				break;
			}
			SKILL_INFO info;
			info.Index = lpMemScript->GetNumber();
			char* tempName = lpMemScript->GetAsString();
			memcpy(info.Name, tempName, sizeof(info.Name));
			info.Name[sizeof(info.Name)-1] = '\0';
			info.Damage = lpMemScript->GetAsNumber();
			info.Mana = lpMemScript->GetAsNumber();
			info.BP = lpMemScript->GetAsNumber();
			info.Range = lpMemScript->GetAsNumber();
			info.Radio = lpMemScript->GetAsNumber();
			info.Delay = lpMemScript->GetAsNumber();
			info.Type = lpMemScript->GetAsNumber();
			info.Effect = lpMemScript->GetAsNumber();
			info.RequireLevel = lpMemScript->GetAsNumber();
			info.RequireEnergy = lpMemScript->GetAsNumber();
			info.RequireLeadership = lpMemScript->GetAsNumber();
			info.RequireKillCount = lpMemScript->GetAsNumber();
			info.RequireGuildStatus = lpMemScript->GetAsNumber();
			for (int n = 0; n < MAX_CLASS; n++) { info.RequireClass[n] = lpMemScript->GetAsNumber(); }
			this->m_SkillInfo.insert(std::pair<int, SKILL_INFO>(info.Index, info));
		}
	}
	catch (...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}
	delete lpMemScript;
	rise::slayerserver::ApplyServerCatalog(this->m_SkillInfo);
}
bool CSkillManager::GetInfo(int index, SKILL_INFO* lpInfo)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return 0;
	}
	else
	{
		(*lpInfo) = it->second;
		return 1;
	}
}
int CSkillManager::GetSkillDamage(int index)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Damage;
	}
}
int CSkillManager::GetSkillMana(int index)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Mana;
	}
}
int CSkillManager::GetSkillBP(int index)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.BP;
	}
}
int CSkillManager::GetSkillType(int index)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Type;
	}
}
int CSkillManager::GetSkillEffect(int index)
{
	std::map<int, SKILL_INFO>::iterator it = this->m_SkillInfo.find(index);
	if (it == this->m_SkillInfo.end())
	{
		return -1;
	}
	else
	{
		return it->second.Effect;
	}
}
int CSkillManager::GetSkillAngle(int x, int y, int tx, int ty)
{
	float dx = (float)(x - tx);
	float dy = (float)(y - ty);
	float rad = atan2(dy, dx);
	int angle = (int)(((rad * 180) / 3.141592741012573) + 90);
	return ((angle < 0) ? (angle + 360) : angle);
}
int CSkillManager::GetSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, BYTE angle, int x, int y, float sx, float sy, float tx, float ty)
{
	vec3_t p[4];
	Vector(-sx, sy, 0.0f, p[0]);
	Vector(sx, sy, 0.0f, p[1]);
	Vector(tx, ty, 0.f, p[2]);
	Vector(-tx, ty, 0.f, p[3]);
	vec3_t Angle;
	Vector(0.0f, 0.0f, (vec_t)((angle * 360) / 255), Angle);
	float Matrix[3][4];
	AngleMatrix(Angle, Matrix);
	vec3_t vFrustrum[4];
	for (int n = 0; n < 4; n++)
	{
		VectorRotate(p[n], Matrix, vFrustrum[n]);
		SkillFrustrumX[n] = (int)vFrustrum[n][0] + x;
		SkillFrustrumY[n] = (int)vFrustrum[n][1] + y;
	}
	return 1;
}
int CSkillManager::GetSkillNumber(int index, int level)
{
	if (index >= GET_ITEM(15, 0) && index <= GET_ITEM(15, 15))
	{
		return ((index % MAX_ITEM_TYPE) + 1);
	}
	switch (index)
	{
	case GET_ITEM(12, 7):
		return SKILL_TWISTING_SLASH;
	case GET_ITEM(12, 8):
		return SKILL_HEAL;
	case GET_ITEM(12, 9):
		return SKILL_GREATER_DEFENSE;
	case GET_ITEM(12, 10):
		return SKILL_GREATER_DAMAGE;
	case GET_ITEM(12, 11):
		return (SKILL_SUMMON1 + level);
	case GET_ITEM(12, 12):
		return SKILL_RAGEFUL_BLOW;
	case GET_ITEM(12, 13):
		return SKILL_IMPALE;
	case GET_ITEM(12, 14):
		return SKILL_GREATER_LIFE;
	case GET_ITEM(12, 16):
		return SKILL_FIRE_SLASH;
	case GET_ITEM(12, 17):
		return SKILL_PENETRATION;
	case GET_ITEM(12, 18):
		return SKILL_ICE_ARROW;
	case GET_ITEM(12, 19):
		return SKILL_DEATH_STAB;
	case GET_ITEM(12, 21):
		return SKILL_FIRE_BURST;
	case GET_ITEM(12, 22):
		return SKILL_SUMMON_PARTY;
	case GET_ITEM(12, 23):
		return SKILL_GREATER_CRITICAL_DAMAGE;
	case GET_ITEM(12, 24):
		return SKILL_ELECTRIC_SPARK;
	case GET_ITEM(12, 35):
		return SKILL_FIRE_SCREAM;
	case GET_ITEM(12, 44):
		return SKILL_FROZEN_STAB;
	case GET_ITEM(12, 45):
		return SKILL_FIVE_SHOT;
	case GET_ITEM(12, 46):
		return SKILL_SHIELD_RECOVER;
	case GET_ITEM(12, 47):
		return SKILL_SWORD_SLASH;
	case GET_ITEM(12, 48):
		return SKILL_BIRDS;
	case GET_ITEM(15, 16):
		return SKILL_DECAY;
	case GET_ITEM(15, 17):
		return SKILL_ICE_STORM;
	case GET_ITEM(15, 18):
		return SKILL_NOVA;
	case GET_ITEM(15, 19):
		return SKILL_CHAIN_LIGHTNING;
	case GET_ITEM(15, 20):
		return SKILL_DRAIN_LIFE;
	case GET_ITEM(15, 21):
		return SKILL_RED_STORM;
	case GET_ITEM(15, 22):
		return SKILL_DAMAGE_REFLECT;
	case GET_ITEM(15, 23):
		return SKILL_SWORD_POWER;
	case GET_ITEM(15, 24):
		return SKILL_SLEEP;
	case GET_ITEM(15, 26):
		return SKILL_LESSER_DAMAGE;
	case GET_ITEM(15, 27):
		return SKILL_LESSER_DEFENSE;
	case GET_ITEM(15, 28):
		return SKILL_MAGIC_CIRCLE;
	case GET_ITEM(15, 29):
		return SKILL_LIGHTNING_STORM;
	case GET_ITEM(15, 30):
		return SKILL_CHAIN_DRIVER;
	case GET_ITEM(15, 31):
		return SKILL_DARK_SIDE;
	case GET_ITEM(15, 32):
		return SKILL_DRAGON_LORE;
	case GET_ITEM(15, 33):
		return SKILL_DRAGON_SLAYER;
	case GET_ITEM(15, 34):
		return SKILL_GREATER_IGNORE_DEFENSE_RATE;
	case GET_ITEM(15, 35):
		return SKILL_FITNESS;
	case GET_ITEM(15, 36):
		return SKILL_GREATER_DEFENSE_SUCCESS_RATE;
	}
	return -1;
}
bool CSkillManager::CheckSkillMana(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (lpObj->Mana < ((SkillInfo.Mana * lpObj->MPConsumptionRate) / 100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
bool CSkillManager::CheckSkillBP(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (lpObj->BP < ((SkillInfo.BP * lpObj->BPConsumptionRate) / 100))
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
bool CSkillManager::CheckSkillRange(int index, int x, int y, int tx, int ty)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (SkillInfo.Range == 0)
	{
		return 0;
	}
	if (sqrt(pow(((float)x - (float)tx), 2) + pow(((float)y - (float)ty), 2)) <= SkillInfo.Range)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRadio(int index, int x, int y, int tx, int ty)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (SkillInfo.Radio == 0)
	{
		return 0;
	}
	if (sqrt(pow(((float)x - (float)tx), 2) + pow(((float)y - (float)ty), 2)) <= SkillInfo.Radio)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillFrustrum(int* SkillFrustrumX, int* SkillFrustrumY, int x, int y)
{
	for (int n = 0, i = 3; n < 4; i = n, n++)
	{
		if ((((SkillFrustrumX[n] - x) * (SkillFrustrumY[i] - y)) - ((SkillFrustrumX[i] - x) * (SkillFrustrumY[n] - y))) < 0)
		{
			return 0;
		}
	}
	return 1;
}
bool CSkillManager::CheckSkillDelay(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (lpObj->RageFighterSkillIndex == index && lpObj->RageFighterSkillCount > 0)
	{
		return 1;
	}
	if ((GetTickCount() - lpObj->SkillDelay[index]) < ((DWORD)SkillInfo.Delay))
	{
		return 0;
	}
	lpObj->SkillDelay[index] = GetTickCount();
	return 1;
}
bool CSkillManager::CheckSkillTarget(LPOBJ lpObj, int aIndex, int bIndex, int type)
{
	if (OBJECT_RANGE(aIndex) == 0)
	{
		return 0;
	}
	if (type == OBJECT_NPC)
	{
		return 0;
	}
	if (gObj[aIndex].Live == 0 || gObj[aIndex].State != OBJECT_PLAYING || gObj[aIndex].Teleport != 0)
	{
		return 0;
	}
	if (lpObj->Type == OBJECT_MONSTER && type == OBJECT_USER)
	{
		return 1;
	}
	if (aIndex == bIndex)
	{
		return 1;
	}
	int SummonIndex = aIndex;
	if (type == OBJECT_MONSTER)
	{
		if ((gObj[aIndex].Class >= 100 && gObj[aIndex].Class <= 110) || gObj[aIndex].Class == 523)
		{
			return 0;
		}
		if (OBJECT_RANGE(gObj[aIndex].SummonIndex) == 0)
		{
			return 1;
		}
		else
		{
			SummonIndex = gObj[aIndex].SummonIndex;
		}
	}
	if (gDuel.CheckDuel(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}
	if (gGensSystem.CheckGens(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}
	if (gObjTargetGuildWarCheck(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}
	if (lpObj->KillAll != 0 && gObj[SummonIndex].KillAll != 0 && gEventKillAll.CheckPlayerTarget(lpObj, &gObj[SummonIndex]) != 0)
	{
		return 1;
	}
	if (lpObj->PvP != 0 && gObj[SummonIndex].PvP != 0)
	{
		return 1;
	}
#if(GAMESERVER_TYPE==1)
	if (gObj[SummonIndex].Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE)
	{
		if (lpObj->Type == OBJECT_USER && gObj[SummonIndex].Type == OBJECT_USER)
		{
			if (gServerInfo.m_CastleSiegeDamageRate2 != 0 || lpObj->CsJoinSide == 0 || gObj[SummonIndex].CsJoinSide == 0 || lpObj->CsJoinSide != gObj[SummonIndex].CsJoinSide)
			{
				return 1;
			}
		}
		else
		{
			return 1;
		}
	}
#endif
	return 0;
}
bool CSkillManager::CheckSkillRequireLevel(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (lpObj->Level >= SkillInfo.RequireLevel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireEnergy(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if ((lpObj->Energy + lpObj->AddEnergy) >= SkillInfo.RequireEnergy)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireLeadership(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if ((lpObj->Leadership + lpObj->AddLeadership) >= SkillInfo.RequireLeadership)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireKillPoint(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (lpObj->KillCount >= SkillInfo.RequireKillCount)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireGuildStatus(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	if (SkillInfo.RequireGuildStatus == 0 || (SkillInfo.RequireGuildStatus & 0x20) == lpObj->GuildStatus || (SkillInfo.RequireGuildStatus & 0x40) == lpObj->GuildStatus || (SkillInfo.RequireGuildStatus & 0x80) == lpObj->GuildStatus)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireClass(LPOBJ lpObj, int index)
{
	SKILL_INFO SkillInfo;
	if (this->GetInfo(index, &SkillInfo) == 0)
	{
		return 0;
	}
	// Slayer rows intentionally have no legacy RequireClass[0..6] column.
	// Their class identity is carried by the persisted DB class, while
	// CharacterInfoSet keeps lpObj->Class on a safe legacy array slot.
	if (rise::slayerserver::IsSlayerSkill(index))
	{
		if (!rise::slayerserver::IsSlayerDbClass(lpObj->DBClass))
			return 0;
		const rise::slayer::SkillSeed* seed = rise::slayer::FindSkillSeed(index);
		if (!seed)
			return 0;
		const int classStage = lpObj->DBClass -
			rise::slayerserver::kS21SlayerDbClass + 1;
		if (classStage < seed->classRequirement)
			return 0;
		if (index == rise::slayerserver::kPierceAttack)
		{
			// Pinned S21 SkillRequire.xml row 294: Pierce bead/use requires
			// base Bat Flock plus 10 points in mastery skill 782.  Neither
			// condition is represented by the legacy RequireClass array.
			// Native 5.2 mastery learning can replace the active base
			// Skill[] slot with 781/782.  Accept that stored Bat lineage,
			// while still requiring the separate learned 782/10 node.
			if (this->GetSkill(lpObj, rise::slayerserver::kBatFlock) == 0 &&
				this->GetSkill(lpObj,
					rise::slayerserver::kBatFlockStrengthener) == 0 &&
				this->GetSkill(lpObj,
					rise::slayerserver::kBatFlockMastery) == 0)
				return 0;
			if (gMasterSkillTree.GetMasterSkillLevel(lpObj,
					rise::slayerserver::kBatFlockMastery) < 10)
				return 0;
		}
		return rise::slayer::MeetsStats(index, lpObj->Level,
			lpObj->Strength + lpObj->AddStrength,
			lpObj->Dexterity + lpObj->AddDexterity) ? 1 : 0;
	}
	if (rise::slayerserver::IsSlayerBatMasterySkill(index))
	{
		// S21 SkillList.xml 781/782: Master Slayer stage 3, level 160,
		// STR 100 and DEX 380. The legacy class-array slot is DK for ABI
		// compatibility, so never authorize these rows by RequireClass[1].
		if (!rise::slayerserver::IsSlayerDbClass(lpObj->DBClass) ||
			lpObj->DBClass < rise::slayerserver::kS21MasterSlayerDbClass ||
			lpObj->Level < 160 ||
			lpObj->Strength + lpObj->AddStrength < 100 ||
			lpObj->Dexterity + lpObj->AddDexterity < 380)
			return 0;
		// The S21 Master Slayer third tree makes 782 depend on 781.
		// 5.2 stores an acquired 10-point node as m_level=9.
		if (index == rise::slayerserver::kBatFlockMastery &&
			gMasterSkillTree.GetMasterSkillLevel(lpObj,
				rise::slayerserver::kBatFlockStrengthener) < 10)
			return 0;
		return 1;
	}
	if (CHECK_RANGE(lpObj->Class, MAX_CLASS) == 0)
	{
		return 0;
	}
	if (SkillInfo.RequireClass[lpObj->Class] == 0)
	{
		return 0;
	}
	if ((lpObj->ChangeUp + 1) >= SkillInfo.RequireClass[lpObj->Class])
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool CSkillManager::CheckSkillRequireWeapon(LPOBJ lpObj, int index)
{
	if (index != SKILL_DEFENSE && index != SKILL_FALLING_SLASH && index != SKILL_LUNGE && index != SKILL_UPPERCUT && index != SKILL_CYCLONE && index != SKILL_SLASH && index != SKILL_TRIPLE_SHOT && index != SKILL_FIRE_BREATH && index != SKILL_POWER_SLASH && index != SKILL_EARTHQUAKE && index != SKILL_PLASMA_STORM && index != SKILL_SAHAMUTT && index != SKILL_NEIL && index != SKILL_GHOST_PHANTOM && index != SKILL_LARGE_RING_BLOWER && index != SKILL_UPPER_BEAST && index != SKILL_PHOENIX_SHOT)
	{
		return 1;
	}
	for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Option1 != 0)
			{
				if (lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}
int CSkillManager::AddSkillWeapon(LPOBJ lpObj, int index, int level)
{
	int slot = 0, skill = 0;
	if ((slot = this->AddSkill(lpObj, (skill = gMasterSkillTree.GetMasterSkillWeapon(lpObj, index)), level)) >= 0)
	{
		this->GCSkillAddSend(lpObj->Index, slot, skill, (BYTE)level, 0); return 1;
	}
	else
	{
		return 0;
	}
}
int CSkillManager::DelSkillWeapon(LPOBJ lpObj, int index, int level)
{
	int slot = 0, skill = 0, count = 0;
	for (int n = 0; n < INVENTORY_WEAR_SIZE; n++)
	{
		if (lpObj->Inventory[n].IsItem() != 0)
		{
			if (lpObj->Inventory[n].m_Option1 != 0)
			{
				if (lpObj->Inventory[n].m_SpecialIndex[SPECIAL_OPTION1] == index)
				{
					count++;
				}
			}
		}
	}
	if (count == 1 && (slot = this->DelSkill(lpObj, (skill = gMasterSkillTree.GetMasterSkillWeapon(lpObj, index)))) >= 0)
	{
		this->GCSkillDelSend(lpObj->Index, slot, skill, (BYTE)level, 0); return 1;
	}
	else
	{
		return 0;
	}
}
int CSkillManager::AddSkill(LPOBJ lpObj, int index, int level)
{
	if (lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj, index) == 0 || this->CheckSkillRequireEnergy(lpObj, index) == 0 || this->CheckSkillRequireLeadership(lpObj, index) == 0 || this->CheckSkillRequireClass(lpObj, index) == 0))
	{
		return -1;
	}
	if (this->GetSkill(lpObj, index) != 0)
	{
		return -1;
	}
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			return ((lpObj->Skill[n].Set(index, level) == 0) ? -1 : n);
		}
	}
	return -1;
}
int CSkillManager::DelSkill(LPOBJ lpObj, int index)
{
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() != 0)
		{
			if (lpObj->Skill[n].m_index == index)
			{
				lpObj->Skill[n].Clear();
				return n;
			}
		}
	}
	return -1;
}
CSkill* CSkillManager::GetSkill(LPOBJ lpObj, int index)
{
	index = ((index == SKILL_NOVA_START) ? SKILL_NOVA : index);
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() != 0)
		{
			if (lpObj->Skill[n].m_index == index || lpObj->Skill[n].m_skill == index)
			{
				return &lpObj->Skill[n];
			}
		}
	}
	return 0;
}
void CSkillManager::Save(char* path)
{
	FILE* file = fopen(path, "w");
	if (file == NULL) return;
	char line[2048];
	auto WriteField = [](char* b, const char* val, int width) {
		int len = (int)strlen(val);
		strcat(b, val);
		for (int i = 0; i < width - len; i++) strcat(b, " ");
	};
	line[0] = 0;
	WriteField(line, "//Index", 10);
	WriteField(line, "Name", 36);
	WriteField(line, "Damage", 10);
	WriteField(line, "MP", 10);
	WriteField(line, "BP", 10);
	WriteField(line, "Range", 10);
	WriteField(line, "Radio", 10);
	WriteField(line, "Delay", 10);
	WriteField(line, "Type", 8);
	WriteField(line, "Effect", 10);
	WriteField(line, "ReqLevel", 12);
	WriteField(line, "ReqEnergy", 12);
	WriteField(line, "ReqLead", 12);
	WriteField(line, "ReqKill", 12);
	WriteField(line, "ReqGuild", 12);
	WriteField(line, "DW", 6);
	WriteField(line, "DK", 6);
	WriteField(line, "FE", 6);
	WriteField(line, "MG", 6);
	WriteField(line, "DL", 6);
	WriteField(line, "SU", 6);
	WriteField(line, "RF", 6);
	fprintf(file, "%s\n", line);
	for(auto it = this->m_SkillInfo.begin(); it != this->m_SkillInfo.end(); it++)
	{
		SKILL_INFO* info = &it->second;
		line[0] = 0;
		char buf[64];
		sprintf_s(buf, "%d", info->Index); WriteField(line, buf, 10);
		sprintf_s(buf, "\"%s\"", info->Name); WriteField(line, buf, 36);
		sprintf_s(buf, "%d", info->Damage); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->Mana); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->BP); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->Range); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->Radio); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->Delay); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->Type); WriteField(line, buf, 8);
		sprintf_s(buf, "%d", info->Effect); WriteField(line, buf, 10);
		sprintf_s(buf, "%d", info->RequireLevel); WriteField(line, buf, 12);
		sprintf_s(buf, "%d", info->RequireEnergy); WriteField(line, buf, 12);
		sprintf_s(buf, "%d", info->RequireLeadership); WriteField(line, buf, 12);
		sprintf_s(buf, "%d", info->RequireKillCount); WriteField(line, buf, 12);
		sprintf_s(buf, "%d", info->RequireGuildStatus); WriteField(line, buf, 12);
		sprintf_s(buf, "%d", info->RequireClass[0]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[1]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[2]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[3]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[4]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[5]); WriteField(line, buf, 6);
		sprintf_s(buf, "%d", info->RequireClass[6]); WriteField(line, buf, 6);
		fprintf(file, "%s\n", line);
	}
	fprintf(file, "end\n");
	fclose(file);
}
#ifndef ListView_SetItemTextW
#define ListView_SetItemTextW(hwnd, i, iSubItem_, pszText_) \
{ LVITEMW _MS_LVI; memset(&_MS_LVI, 0, sizeof(_MS_LVI)); _MS_LVI.iSubItem = iSubItem_; _MS_LVI.pszText = pszText_; \
  SendMessageW((hwnd), LVM_SETITEMTEXTW, (WPARAM)(i), (LPARAM)(LVITEMW *)&_MS_LVI); }
#endif
#ifndef ListView_GetItemTextW
#define ListView_GetItemTextW(hwnd, i, iSubItem_, pszText_, cchTextMax_) \
{ LVITEMW _MS_LVI; memset(&_MS_LVI, 0, sizeof(_MS_LVI)); _MS_LVI.iSubItem = iSubItem_; _MS_LVI.pszText = pszText_; _MS_LVI.cchTextMax = cchTextMax_; \
  SendMessageW((hwnd), LVM_GETITEMTEXTW, (WPARAM)(i), (LPARAM)(LVITEMW *)&_MS_LVI); }
#endif
struct SkillEditInfo {
	HWND hListView;
	int iItem;
	int iSubItem;
	int OriginalIndex;
};
static SkillEditInfo g_SkillEdit = { 0 };
static LRESULT CALLBACK SkillEditSubclass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (message) {
	case WM_GETDLGCODE: return DLGC_WANTALLKEYS;
	case WM_KEYDOWN:
		if (wParam == VK_RETURN) { SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(IDC_SKILL_EDIT, EN_KILLFOCUS), (LPARAM)hWnd); return 0; }
		if (wParam == VK_ESCAPE) { ShowWindow(hWnd, SW_HIDE); SetFocus(g_SkillEdit.hListView); return 0; }
		break;
	}
	return DefSubclassProc(hWnd, message, wParam, lParam);
}
void RefreshSkillUI(HWND hList) {
	ListView_DeleteAllItems(hList);
	int i = 0;
	for (auto& pair : gSkillManager.m_SkillInfo) {
		SKILL_INFO* info = &pair.second;
		LVITEMW lvi; lvi.mask = LVIF_TEXT | LVIF_PARAM; lvi.iItem = i; lvi.iSubItem = 0;
		WCHAR szIndex[16]; swprintf_s(szIndex, L"%d", info->Index);
		lvi.pszText = szIndex; lvi.lParam = i;
		SendMessageW(hList, LVM_INSERTITEMW, i, (LPARAM)&lvi);
		auto setSub = [&](int sub, int val) {
			WCHAR s[32]; swprintf_s(s, L"%d", val);
			LVITEMW si; si.iSubItem = sub; si.pszText = s;
			SendMessageW(hList, LVM_SETITEMTEXTW, i, (LPARAM)&si);
		};
		auto setSubStr = [&](int sub, char* val) {
			WCHAR s[64]; MultiByteToWideChar(CP_UTF8, 0, val, -1, s, 64);
			LVITEMW si; si.iSubItem = sub; si.pszText = s;
			SendMessageW(hList, LVM_SETITEMTEXTW, i, (LPARAM)&si);
		};
		setSubStr(1, info->Name);
		setSub(2, info->Damage); setSub(3, info->Mana); setSub(4, info->BP);
		setSub(5, info->Range); setSub(6, info->Radio); setSub(7, info->Delay);
		setSub(8, info->Type); setSub(9, info->Effect);
		setSub(10, info->RequireLevel); setSub(11, info->RequireEnergy); setSub(12, info->RequireLeadership);
		setSub(13, info->RequireKillCount); setSub(14, info->RequireGuildStatus);
		for(int k=0; k<7; k++) setSub(15+k, info->RequireClass[k]);
		i++;
	}
}
std::map<int, SKILL_INFO>::iterator GetSkillIterator(int index) {
	auto it = gSkillManager.m_SkillInfo.begin();
	std::advance(it, index);
	return it;
}
INT_PTR CALLBACK RISE_SkillProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG: {
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL)));
		RECT rcDlg, rcScreen; GetWindowRect(hDlg, &rcDlg); GetWindowRect(GetDesktopWindow(), &rcScreen);
		SetWindowPos(hDlg, NULL, (rcScreen.right - (rcDlg.right - rcDlg.left)) / 2, (rcScreen.bottom - (rcDlg.bottom - rcDlg.top)) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		HWND hList = GetDlgItem(hDlg, IDC_SKILL_LIST);
		static HFONT hFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, VIETNAMESE_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Roboto Condensed");
		SendMessage(hList, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		const WCHAR* cols[] = { L"Index", L"Name", L"Damage", L"MP", L"BP", L"Range", L"Radio", L"Delay", L"Type", L"Effect", L"ReqLevel", L"ReqEnergy", L"ReqDL", L"ReqKill", L"ReqGuild", L"DW", L"DK", L"FE", L"MG", L"DL", L"SU", L"RF" };
		int widths[] = { 40, 200, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 30, 30, 30, 30, 30, 30, 30 };
		for (int i = 0; i < 22; i++) {
			LVCOLUMNW col; col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT; col.pszText = (WCHAR*)cols[i]; col.cx = widths[i]; col.fmt = LVCFMT_LEFT;
			SendMessageW(hList, LVM_INSERTCOLUMNW, i, (LPARAM)&col);
		}
		HWND hEdit = GetDlgItem(hDlg, IDC_SKILL_EDIT);
		SetWindowSubclass(hEdit, SkillEditSubclass, 0, 0);
		SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
		RefreshSkillUI(hList);
		return TRUE;
	}
	case WM_NOTIFY: {
		LPNMHDR nm = (LPNMHDR)lParam;
		if (nm->idFrom == IDC_SKILL_LIST) {
			if (nm->code == NM_DBLCLK) {
				LPNMLISTVIEW p = (LPNMLISTVIEW)lParam; if (p->iItem == -1) return 0;
				HWND hList = nm->hwndFrom; RECT r; ListView_GetSubItemRect(hList, p->iItem, p->iSubItem, LVIR_BOUNDS, &r);
				if(p->iSubItem == 0) r.right = r.left + ListView_GetColumnWidth(hList, 0);
				POINT pt = { r.left, r.top }; MapWindowPoints(hList, hDlg, &pt, 1);
				HWND hEdit = GetDlgItem(hDlg, IDC_SKILL_EDIT);
				auto it = GetSkillIterator(p->iItem);
				if(it != gSkillManager.m_SkillInfo.end()) g_SkillEdit.OriginalIndex = it->first;
				g_SkillEdit.hListView = hList; g_SkillEdit.iItem = p->iItem; g_SkillEdit.iSubItem = p->iSubItem;
				WCHAR sz[128]; ListView_GetItemTextW(hList, p->iItem, p->iSubItem, sz, 128);
				SetWindowTextW(hEdit, sz); SetWindowPos(hEdit, HWND_TOP, pt.x, pt.y, r.right - r.left, r.bottom - r.top, SWP_SHOWWINDOW);
				SetFocus(hEdit); SendMessage(hEdit, EM_SETSEL, 0, -1);
			} else if (nm->code == NM_RCLICK) {
				HMENU m = CreatePopupMenu();
				AppendMenuW(m, MF_STRING, IDM_SKILL_INSERT, L"Insert Item");
				LPNMLISTVIEW p = (LPNMLISTVIEW)lParam;
				if (p->iItem != -1) AppendMenuW(m, MF_STRING, IDM_SKILL_DELETE, L"Delete Item");
				POINT pt; GetCursorPos(&pt); TrackPopupMenu(m, TPM_LEFTALIGN, pt.x, pt.y, 0, hDlg, NULL); DestroyMenu(m);
			}
		}
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wParam) == IDC_SKILL_EDIT && HIWORD(wParam) == EN_KILLFOCUS) {
			HWND hEdit = (HWND)lParam; WCHAR sz[128]; GetWindowTextW(hEdit, sz, 128);
			ListView_SetItemTextW(g_SkillEdit.hListView, g_SkillEdit.iItem, g_SkillEdit.iSubItem, sz); ShowWindow(hEdit, SW_HIDE);
			auto it = gSkillManager.m_SkillInfo.find(g_SkillEdit.OriginalIndex);
			if (it != gSkillManager.m_SkillInfo.end()) {
				SKILL_INFO info = it->second;
				char szAnsi[128]; WideCharToMultiByte(CP_UTF8, 0, sz, -1, szAnsi, 128, NULL, NULL);
				int val = atoi(szAnsi);
				if(g_SkillEdit.iSubItem == 0) {
					if(val != info.Index && gSkillManager.m_SkillInfo.find(val) == gSkillManager.m_SkillInfo.end()) {
						gSkillManager.m_SkillInfo.erase(it);
						info.Index = val;
						gSkillManager.m_SkillInfo[val] = info;
						g_SkillEdit.OriginalIndex = val;
					} else {
						WCHAR szOld[16]; swprintf_s(szOld, L"%d", info.Index);
						ListView_SetItemTextW(g_SkillEdit.hListView, g_SkillEdit.iItem, 0, szOld);
					}
				} else {
					switch(g_SkillEdit.iSubItem) {
						case 1: strcpy_s(info.Name, szAnsi); break;
						case 2: info.Damage = val; break; case 3: info.Mana = val; break;
						case 4: info.BP = val; break; case 5: info.Range = val; break;
						case 6: info.Radio = val; break; case 7: info.Delay = val; break;
						case 8: info.Type = val; break; case 9: info.Effect = val; break;
						case 10: info.RequireLevel = val; break; case 11: info.RequireEnergy = val; break;
						case 12: info.RequireLeadership = val; break; case 13: info.RequireKillCount = val; break;
						case 14: info.RequireGuildStatus = val; break;
						default: if(g_SkillEdit.iSubItem >= 15 && g_SkillEdit.iSubItem <= 21) info.RequireClass[g_SkillEdit.iSubItem-15] = val; break;
					}
					it->second = info;
				}
			}
		}
		switch (LOWORD(wParam)) {
		case IDC_SKILL_SAVE:
			gSkillManager.Save(gPath.GetFullPath("Skill\\Skill.txt"));
			gSkillManager.Load(gPath.GetFullPath("Skill\\Skill.txt"));
			LogAdd(LOG_BLUE, "[CSkillManager] Skill configuration reloaded.");
			RefreshSkillUI(GetDlgItem(hDlg, IDC_SKILL_LIST));
			MessageBoxW(hDlg, MSGBOX_03, L"Info", MB_OK);
			break;
		case IDM_SKILL_INSERT: {
			if (gSkillManager.m_SkillInfo.size() < MAX_SKILL) {
				int newIndex = 1;
				while(gSkillManager.m_SkillInfo.find(newIndex) != gSkillManager.m_SkillInfo.end()) newIndex++;
				SKILL_INFO info = { newIndex, "New Skill", 0,0,0,0,0,0,0,0,0,0,0,0,0, {0,0,0,0,0,0,0} };
				gSkillManager.m_SkillInfo[newIndex] = info;
				HWND hList = GetDlgItem(hDlg, IDC_SKILL_LIST);
				int newItem = ListView_GetItemCount(hList);
				LVITEMW lvi; lvi.mask = LVIF_TEXT | LVIF_PARAM; lvi.iItem = newItem; lvi.iSubItem = 0;
				WCHAR szIdx[16]; swprintf_s(szIdx, L"%d", newIndex);
				lvi.pszText = szIdx; lvi.lParam = newItem;
				SendMessageW(hList, LVM_INSERTITEMW, newItem, (LPARAM)&lvi);
				auto setSub = [&](int sub, int val) {
					WCHAR s[32]; swprintf_s(s, L"%d", val);
					LVITEMW si; si.iSubItem = sub; si.pszText = s;
					SendMessageW(hList, LVM_SETITEMTEXTW, newItem, (LPARAM)&si);
				};
				auto setSubStr = [&](int sub, char* val) {
					WCHAR s[64]; MultiByteToWideChar(CP_UTF8, 0, val, -1, s, 64);
					LVITEMW si; si.iSubItem = sub; si.pszText = s;
					SendMessageW(hList, LVM_SETITEMTEXTW, newItem, (LPARAM)&si);
				};
				setSubStr(1, info.Name);
				setSub(2, 0); setSub(3, 0); setSub(4, 0); setSub(5, 0); setSub(6, 0); setSub(7, 0);
				setSub(8, 0); setSub(9, 0); setSub(10, 0); setSub(11, 0); setSub(12, 0); setSub(13, 0); setSub(14, 0);
				for(int k=0; k<7; k++) setSub(15+k, 0);
				ListView_EnsureVisible(hList, newItem, FALSE);
				ListView_SetItemState(hList, newItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
			break;
		}
		case IDM_SKILL_DELETE: {
			HWND hList = GetDlgItem(hDlg, IDC_SKILL_LIST);
			int sel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
			if (sel != -1) {
				auto it = GetSkillIterator(sel);
				if(it != gSkillManager.m_SkillInfo.end()) {
					gSkillManager.m_SkillInfo.erase(it);
					ListView_DeleteItem(hList, sel);
				}
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE: EndDialog(hDlg, 0); break;
	}
	return FALSE;
}
int CSkillManager::AddMasterSkill(LPOBJ lpObj, int index, int level)
{
	if (lpObj->Type == OBJECT_USER && (this->CheckSkillRequireLevel(lpObj, index) == 0 || this->CheckSkillRequireEnergy(lpObj, index) == 0 || this->CheckSkillRequireLeadership(lpObj, index) == 0 || this->CheckSkillRequireClass(lpObj, index) == 0))
	{
		return -1;
	}
	if (this->GetMasterSkill(lpObj, index) != 0)
	{
		return -1;
	}
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() == 0)
		{
			return ((lpObj->MasterSkill[n].Set(index, level) == 0) ? -1 : n);
		}
	}
	return -1;
}
int CSkillManager::DelMasterSkill(LPOBJ lpObj, int index)
{
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			if (lpObj->MasterSkill[n].m_index == index)
			{
				lpObj->MasterSkill[n].Clear();
				return n;
			}
		}
	}
	return -1;
}
CSkill* CSkillManager::GetMasterSkill(LPOBJ lpObj, int index)
{
	index = ((index == SKILL_NOVA_START) ? SKILL_NOVA : index);
	for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
	{
		if (lpObj->MasterSkill[n].IsMasterSkill() != 0)
		{
			if (lpObj->MasterSkill[n].m_index == index || lpObj->MasterSkill[n].m_skill == index)
			{
				return &lpObj->MasterSkill[n];
			}
		}
	}
	return 0;
}
bool CSkillManager::ConvertSkillByte(CSkill* lpSkill, BYTE* lpMsg)
{
	WORD index = MAKE_NUMBERW(((lpMsg[0] == 0xFF && lpMsg[2] == 0x00) ? 0xFF : lpMsg[2]), lpMsg[0]);
	if (index == 0xFFFF)
	{
		return 0;
	}
	lpSkill->m_level = lpMsg[1];
	lpSkill->m_skill = index;
	lpSkill->m_index = index;
	return 1;
}
void CSkillManager::SkillByteConvert(BYTE* lpMsg, CSkill* lpSkill)
{
	lpMsg[0] = SET_NUMBERLB(lpSkill->m_index);
	lpMsg[1] = lpSkill->m_level;
	lpMsg[2] = SET_NUMBERHB(lpSkill->m_index);
}
void CSkillManager::UseAttackSkill(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type == OBJECT_USER && this->CheckSkillRequireWeapon(lpObj, lpSkill->m_skill) == 0)
	{
		return;
	}
	if (lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TWISTING_SLASH && lpObj->Inventory[0].IsItem() == 0)
	{
		return;
	}
	bool combo = 0;
	if ((gServerInfo.m_EnableComboToAllSwitch == 1 || gQuest.CheckQuestListState(lpObj, 3, QUEST_FINISH) != 0) && lpObj->ComboSkill.CheckCombo(lpSkill->m_skill) != 0)
	{
		if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
		{
			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}
	if (lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj, SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex, 0, this->GetSkill(lpObj, SKILL_NOVA), (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo);
		return;
	}
	if (lpSkill->m_skill == SKILL_NOVA && bIndex != SKILL_NOVA_START && bIndex != MASTER_SKILL_ADD_NOVA_START_IMPROVED)
	{
		this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo);
		return;
	}
	if ((lpSkill->m_skill == SKILL_UPPER_BEAST || lpSkill->m_skill == SKILL_CHAIN_DRIVER || lpSkill->m_skill == SKILL_DARK_SIDE) && lpObj->RageFighterSkillIndex == lpSkill->m_index && (lpObj->RageFighterSkillCount++) > 0)
	{
		this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo);
		return;
	}
	if (lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj, lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj, lpSkill->m_index) != 0))
	{
		if (this->RunningSkill(aIndex, bIndex, lpSkill, (BYTE)lpObj->X, (BYTE)lpObj->Y, 0, combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100);
			GCManaSend(aIndex, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}
	}
}
void CSkillManager::UseDurationSkillAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE dir, BYTE angle)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type == OBJECT_USER && this->CheckSkillRequireWeapon(lpObj, lpSkill->m_skill) == 0)
	{
		return;
	}
	if (lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TWISTING_SLASH && lpObj->Inventory[0].IsItem() == 0)
	{
		return;
	}
	if (lpObj->Teleport != 0)
	{
		return;
	}
	bool combo = 0;
	if (lpSkill->m_skill != SKILL_FLAME && lpSkill->m_skill != SKILL_TWISTER && lpSkill->m_skill != SKILL_EVIL_SPIRIT && lpSkill->m_skill != SKILL_HELL_FIRE && lpSkill->m_skill != SKILL_AQUA_BEAM && lpSkill->m_skill != SKILL_BLAST && lpSkill->m_skill != SKILL_INFERNO && lpSkill->m_skill != SKILL_TRIPLE_SHOT && lpSkill->m_skill != SKILL_IMPALE && lpSkill->m_skill != SKILL_MONSTER_AREA_ATTACK && lpSkill->m_skill != SKILL_PENETRATION && lpSkill->m_skill != SKILL_FIRE_SLASH && lpSkill->m_skill != SKILL_FIRE_SCREAM)
	{
		if ((gServerInfo.m_EnableComboToAllSwitch == 1 || gQuest.CheckQuestListState(lpObj, 3, QUEST_FINISH) != 0) && lpObj->ComboSkill.CheckCombo(lpSkill->m_skill) != 0)
		{
			if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
			{
				combo = 1;
				lpObj->ComboTime = GetTickCount();
			}
		}
	}
	if (lpObj->SkillNovaState != 0 && lpSkill->m_skill != SKILL_NOVA && this->GetSkill(lpObj, SKILL_NOVA) != 0)
	{
		this->RunningSkill(aIndex, 0, this->GetSkill(lpObj, SKILL_NOVA), x, y, 0, combo);
		return;
	}
	this->GCDurationSkillAttackSend(lpObj, lpSkill->m_index, x, y, dir);
	if (lpObj->Type != OBJECT_USER || (this->CheckSkillMana(lpObj, lpSkill->m_index) != 0 && this->CheckSkillBP(lpObj, lpSkill->m_index) != 0))
	{
		if (this->RunningSkill(aIndex, bIndex, lpSkill, x, y, angle, combo) != 0 && lpObj->Type == OBJECT_USER)
		{
			lpObj->Mana -= ((this->GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100);
			lpObj->BP -= ((this->GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100);
			GCManaSend(aIndex, 0xFF, (int)lpObj->Mana, lpObj->BP);
		}
	}
}
bool CSkillManager::RunningSkill(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	// Keep the imported S21 rows fail-closed until the legacy class/persistence
	// ABI has a real class-9 mapping.  This protects the server even if a client
	// sends a direct skill packet instead of passing through AddSkill checks.
	if (lpSkill != 0 && rise::slayerserver::IsSlayerSkill(lpSkill->m_skill) &&
		!rise::slayerserver::IsSlayerClass(lpObj->Class) &&
		!rise::slayerserver::IsSlayerDbClass(lpObj->DBClass))
	{
		return 0;
	}
	lpObj->ShieldDamageReductionTime = 0;
	gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);
	switch (lpSkill->m_skill)
	{
	case SKILL_FLAME:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_TWISTER:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_EVIL_SPIRIT:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_HELL_FIRE:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_AQUA_BEAM:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_BLAST:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_INFERNO:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_MANA_SHIELD:
		return this->SkillManaShield(aIndex, bIndex, lpSkill);
	case SKILL_DEFENSE:
		return this->SkillDefense(aIndex, bIndex, lpSkill);
	case SKILL_TRIPLE_SHOT:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_HEAL:
		return this->SkillHeal(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DEFENSE:
		return this->SkillGreaterDefense(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DAMAGE:
		return this->SkillGreaterDamage(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON1:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON2:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON3:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON4:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON5:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON6:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON7:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_DECAY:
		return this->SkillDecay(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_ICE_STORM:
		return this->SkillIceStorm(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_NOVA:
		return this->SkillNova(aIndex, bIndex, lpSkill, combo);
	case SKILL_TWISTING_SLASH:
		return this->SkillTwistingSlash(aIndex, bIndex, lpSkill, combo);
	case SKILL_RAGEFUL_BLOW:
		return this->SkillRagefulBlow(aIndex, bIndex, lpSkill, combo);
	case SKILL_DEATH_STAB:
		return this->SkillDeathStab(aIndex, bIndex, lpSkill, combo);
	case SKILL_CRESCENT_MOON_SLASH:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_MANA_GLAIVE:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_STAR_FALL:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_IMPALE:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_GREATER_LIFE:
		return this->SkillGreaterLife(aIndex, bIndex, lpSkill);
	case SKILL_MONSTER_AREA_ATTACK:
		return this->SkillMonsterAreaAttack(aIndex, bIndex, lpSkill);
	case SKILL_PENETRATION:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_FIRE_SLASH:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_POWER_SLASH:
		return this->SkillPowerSlash(aIndex, bIndex, lpSkill, angle, combo);
	case SKILL_SPIRAL_SLASH:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_FORCE:
		return this->SkillForce(aIndex, bIndex, lpSkill, combo);
	case SKILL_FIRE_BURST:
		return this->SkillFireBurst(aIndex, bIndex, lpSkill, combo);
	case SKILL_EARTHQUAKE:
		return this->SkillEarthquake(aIndex, bIndex, lpSkill, combo);
	case SKILL_SUMMON_PARTY:
		return this->SkillSummonParty(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_CRITICAL_DAMAGE:
		return this->SkillGreaterCriticalDamage(aIndex, bIndex, lpSkill);
	case SKILL_ELECTRIC_SPARK:
		return this->SkillElectricSpark(aIndex, bIndex, lpSkill, angle, combo);
	case SKILL_STERN:
		return this->SkillStern(aIndex, bIndex, lpSkill, angle);
	case SKILL_REMOVE_STERN:
		return this->SkillRemoveStern(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_MANA:
		return this->SkillGreaterMana(aIndex, bIndex, lpSkill);
	case SKILL_INVISIBILITY:
		return this->SkillInvisibility(aIndex, bIndex, lpSkill);
	case SKILL_REMOVE_INVISIBILITY:
		return this->SkillRemoveInvisibility(aIndex, bIndex, lpSkill);
	case SKILL_REMOVE_ALL_EFFECT:
		return this->SkillRemoveAllEffect(aIndex, bIndex, lpSkill);
	case SKILL_MANA_RAYS:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_FIRE_BLAST:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_BRAND:
		return this->SkillBrand(aIndex, bIndex, lpSkill);
	case SKILL_PLASMA_STORM:
		return this->SkillPlasmaStorm(aIndex, bIndex, lpSkill);
	case SKILL_INFINITY_ARROW:
		return this->SkillInfinityArrow(aIndex, bIndex, lpSkill);
	case SKILL_FIRE_SCREAM:
		return this->MultiSkillAttack(aIndex, bIndex, lpSkill, combo);
	case SKILL_MONSTER_SUMMON:
		return this->SkillMonsterSummon(aIndex, bIndex, lpSkill);
	case SKILL_MAGIC_DAMAGE_IMMUNITY:
		return this->SkillMagicDamageImmunity(aIndex, bIndex, lpSkill);
	case SKILL_PHYSI_DAMAGE_IMMUNITY:
		return this->SkillPhysiDamageImmunity(aIndex, bIndex, lpSkill);
	case SKILL_CHAIN_LIGHTNING:
		return this->SkillChainLightning(aIndex, bIndex, lpSkill, combo);
	case SKILL_DAMAGE_REFLECT:
		return this->SkillDamageReflect(aIndex, bIndex, lpSkill);
	case SKILL_SWORD_POWER:
		return this->SkillSwordPower(aIndex, bIndex, lpSkill, combo);
	case SKILL_SLEEP:
		return this->SkillSleep(aIndex, bIndex, lpSkill);
	case SKILL_LESSER_DEFENSE:
		return this->SkillLesserDefense(aIndex, bIndex, lpSkill);
	case SKILL_LESSER_DAMAGE:
		return this->SkillLesserDamage(aIndex, bIndex, lpSkill);
	case SKILL_SAHAMUTT:
		return this->SkillSahamutt(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_NEIL:
		return this->SkillNeil(aIndex, bIndex, lpSkill, x, y);
	case SKILL_GHOST_PHANTOM:
		return this->SkillGhostPhantom(aIndex, bIndex, lpSkill, x, y);
	case SKILL_RED_STORM:
		return this->SkillRedStorm(aIndex, bIndex, lpSkill, combo);
	case SKILL_FROZEN_STAB:
		return this->SkillFrozenStab(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_MAGIC_CIRCLE:
		return this->SkillMagicCircle(aIndex, bIndex, lpSkill);
	case SKILL_SHIELD_RECOVER:
		return this->SkillShieldRecover(aIndex, bIndex, lpSkill);
	case SKILL_FIVE_SHOT:
		return this->SkillFiveShot(aIndex, bIndex, lpSkill, angle, combo);
	case SKILL_SWORD_SLASH:
		return this->SkillSwordSlash(aIndex, bIndex, lpSkill, angle, combo);
	case SKILL_LIGHTNING_STORM:
		return this->SkillLightningStorm(aIndex, bIndex, lpSkill, combo);
	case SKILL_BIRDS:
		return this->SkillBirds(aIndex, bIndex, lpSkill, angle, combo);
	case SKILL_SELF_EXPLOSION:
		return this->SkillSelfExplosion(aIndex, bIndex, lpSkill);
	case SKILL_LARGE_RING_BLOWER:
		return this->SkillLargeRingBlower(aIndex, bIndex, lpSkill, combo);
	case SKILL_UPPER_BEAST:
		return this->SkillUpperBeast(aIndex, bIndex, lpSkill, combo);
	case SKILL_CHAIN_DRIVER:
		return this->SkillChainDriver(aIndex, bIndex, lpSkill, combo);
	case SKILL_DARK_SIDE:
		return this->SkillDarkSide(aIndex, bIndex, lpSkill, combo);
	case SKILL_DRAGON_LORE:
		return this->SkillDragonLore(aIndex, bIndex, lpSkill, combo);
	case SKILL_GREATER_IGNORE_DEFENSE_RATE:
		return this->SkillGreaterIgnoreDefenseRate(aIndex, bIndex, lpSkill);
	case SKILL_FITNESS:
		return this->SkillFitness(aIndex, bIndex, lpSkill);
	case SKILL_GREATER_DEFENSE_SUCCESS_RATE:
		return this->SkillGreaterDefenseSuccessRate(aIndex, bIndex, lpSkill);
	case SKILL_CHARGE:
		return this->SkillCastleSiege(aIndex, bIndex, lpSkill, combo);
	case SKILL_PHOENIX_SHOT:
		return this->SkillPhoenixShot(aIndex, bIndex, lpSkill, combo);
	case rise::slayerserver::kSwordInertia:
		return this->SkillSlayerSwordInertia(aIndex, bIndex, lpSkill);
	case rise::slayerserver::kBatFlock:
		return this->SkillSlayerBatFlock(aIndex, bIndex, lpSkill);
	case rise::slayerserver::kBatFlockStrengthener:
	case rise::slayerserver::kBatFlockMastery:
		// S21 lists 781/782 as separate castable mastery skills. Their
		// client dispatch/effect branches are not yet recovered; do not let
		// the 5.2 default BasicSkillAttack impersonate either skill.
		return false;
	case rise::slayerserver::kPierceAttack:
		return this->SkillSlayerPierceAttack(aIndex, bIndex, lpSkill);
	case rise::slayerserver::kDetection:
		return this->SkillSlayerDetection(aIndex, bIndex, lpSkill);
	case rise::slayerserver::kDemolish:
		return this->SkillSlayerDemolish(aIndex, bIndex, lpSkill);
	case SKILL_BLOOD_STORM:
		return this->SkillBloodStorm(aIndex, bIndex, lpSkill, x, y, combo);
	case SKILL_CURE:
		return this->SkillCure(aIndex, bIndex, lpSkill);
	case SKILL_PARTY_HEAL:
		return this->SkillPartyHeal(aIndex, bIndex, lpSkill);
	case SKILL_POISON_ARROW:
		return this->SkillPoisonArrow(aIndex, bIndex, lpSkill, combo);
	case SKILL_BLESS:
		return this->SkillBless(aIndex, bIndex, lpSkill);
	case SKILL_SUMMON_SATYROS:
		return this->SkillSummon(aIndex, bIndex, lpSkill);
	case SKILL_BLIND:
		return this->SkillBlind(aIndex, bIndex, lpSkill);
	case SKILL_EARTH_PRISON:
		return this->SkillEarthPrison(aIndex, bIndex, lpSkill, combo);
	case SKILL_IRON_DEFENSE:
		return this->SkillIronDefense(aIndex, bIndex, lpSkill);
	case SKILL_BLOOD_HOWLING:
		return this->SkillBloodHowling(aIndex, bIndex, lpSkill);
	default:
		return this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);
	}
	return 0;
}
bool CSkillManager::BasicSkillAttack(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 1;
	}
	if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}
	gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 1, 0, 0, 0, combo);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (lpObj->Type == OBJECT_USER && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
		{
			continue;
		}
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::MultiSkillAttack(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type == OBJECT_USER)
	{
		lpObj->MultiSkillIndex = lpSkill->m_index;
		lpObj->MultiSkillCount = 0;
		switch (lpSkill->m_skill)
		{
			case SKILL_FLAME:		this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);			break;
			case SKILL_BLAST:		this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);			break;
			case SKILL_IMPALE:		this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);			break;
			case SKILL_FIRE_SLASH:	this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);			break;
			case SKILL_FIRE_SCREAM:	this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo);			break;
		}
	}
	else
	{
		switch (lpSkill->m_skill)
		{
			case SKILL_EVIL_SPIRIT:	this->SkillEvilSpirit(aIndex, bIndex, lpSkill, combo);	break;
		}
	}
	return 1;
}
bool CSkillManager::SkillChangeUse(int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	int change = -1;
	for (int n = 0; n < 2; n++)
	{
		CItem* Ring = &lpObj->Inventory[10 + n];
		if (Ring->IsItem() == 0 || Ring->m_Durability == 0 || change != -1)
		{
			continue;
		}
		if (Ring->m_Index == GET_ITEM(13, 10))
		{
			change = Ring->m_Level;
		}
		else if (Ring->m_Index == GET_ITEM(13, 39))
		{
			change = 372;
		}
		else if (Ring->m_Index == GET_ITEM(13, 40))
		{
			change = 373;
		}
		else if (Ring->m_Index == GET_ITEM(13, 41))
		{
			change = 374;
		}
		else if (Ring->m_Index == GET_ITEM(13, 42) && lpObj->Authority == 32)
		{
			change = 378;
		}
		else if (Ring->m_Index == GET_ITEM(13, 68))
		{
			change = 477;
		}
		else if (Ring->m_Index == GET_ITEM(13, 76))
		{
			change = 503;
		}
		else if (Ring->m_Index == GET_ITEM(13, 77))
		{
			change = 616;
		}
		else if (Ring->m_Index == GET_ITEM(13, 78))
		{
			change = 617;
		}
		else if (Ring->m_Index == GET_ITEM(13, 122))
		{
			change = 548;
		}
		else if (Ring->m_Index == GET_ITEM(13, 163))
		{
			change = 625;
		}
		else if (Ring->m_Index == GET_ITEM(13, 164))
		{
			change = 626;
		}
		else if (Ring->m_Index == GET_ITEM(13, 165))
		{
			change = 642;
		}
	}
	switch (change)
	{
	case 0:
		change = gServerInfo.m_TransformationRing1;
		break;
	case 1:
		change = gServerInfo.m_TransformationRing2;
		break;
	case 2:
		change = gServerInfo.m_TransformationRing3;
		break;
	case 3:
		change = gServerInfo.m_TransformationRing4;
		break;
	case 4:
		change = gServerInfo.m_TransformationRing5;
		break;
	case 5:
		change = gServerInfo.m_TransformationRing6;
		break;
	}
	if (IT_MAP_RANGE(lpObj->Map) != 0 && (gIllusionTemple.GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_STAND || gIllusionTemple.GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_START || gIllusionTemple.GetState(GET_IT_LEVEL(lpObj->Map)) == IT_STATE_CLEAN))
	{
		switch (gIllusionTemple.GetUserJoinTeam(lpObj))
		{
		case IT_TEAM_Y:
			change = 404;
			break;
		case IT_TEAM_B:
			change = 405;
			break;
		}
	}
	if (lpObj->Change == change)
	{
		return 0;
	}
	else
	{
		lpObj->Change = change; return 1;
	}
}
bool CSkillManager::SkillEvilSpirit(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillManaShield(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value1 = gServerInfo.m_ManaShieldConstA + ((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.m_ManaShieldConstB) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ManaShieldConstC);
	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_IMPROVED);
	value1 = ((value1 > gServerInfo.m_ManaShieldMaxRate) ? gServerInfo.m_ManaShieldMaxRate : value1);
	value1 = (value1 * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_ManaShieldRate[lpTarget->Class] : 100)) / 100;
	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_MASTERED);
	int count = gServerInfo.m_ManaShieldTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ManaShieldTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MANA_SHIELD_ENHANCED);
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillDefense(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Inventory[1].IsItem() != 0)
	{
		if (lpObj->Inventory[1].m_Index >= GET_ITEM(6, 0) && lpObj->Inventory[1].m_Index < GET_ITEM(7, 0))
		{
			if (lpObj->Inventory[1].m_Option1 != 0)
			{
				lpObj->ShieldDamageReductionTime = GetTickCount();
				GCActionSend(lpObj, SKILL_DEFENSE, lpObj->Index, bIndex);
				return 1;
			}
		}
	}
	return 0;
}
bool CSkillManager::SkillHeal(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = gServerInfo.m_HealConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_HealConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_HEAL_IMPROVED)) / 100;
	if ((lpTarget->Life + value) > (lpTarget->MaxLife + lpTarget->AddLife))
	{
		lpTarget->Life = lpTarget->MaxLife + lpTarget->AddLife;
	}
	else
	{
		lpTarget->Life += value;
	}
	if (lpTarget->Type == OBJECT_USER)
	{
		GCLifeSend(bIndex, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	}
	if (lpTarget->Type == OBJECT_MONSTER && OBJECT_RANGE(lpTarget->SummonIndex) != 0)
	{
		GCSummonLifeSend(lpTarget->SummonIndex, (int)lpTarget->Life, (int)lpTarget->MaxLife);
	}
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 40));
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillGreaterDefense(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = gServerInfo.m_GreaterDefenseConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_IMPROVED)) / 100;
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_ENHANCED)) / 100;
	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_GreaterDefenseRate[lpTarget->Class] : 100)) / 100;
	int count = gServerInfo.m_GreaterDefenseTimeConstA;
	count += gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_ENHANCED);
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, (value * 2), 0, 0, 0);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 10));
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillGreaterDamage(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = gServerInfo.m_GreaterDamageConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDamageConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_IMPROVED)) / 100;
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_ENHANCED)) / 100;
	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_GreaterDamageRate[lpTarget->Class] : 100)) / 100;
	int count = gServerInfo.m_GreaterDamageTimeConstA;
	count += gMasterSkillTree.GetMasterSkillLevel(lpObj, MASTER_SKILL_ADD_GREATER_DAMAGE_ENHANCED);
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 10));
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillSummon(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map == MAP_ICARUS || CC_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}
	int monster = 0;
	switch (lpSkill->m_index)
	{
	case SKILL_SUMMON1:
		monster = gServerInfo.m_SummonMonster1;
		break;
	case SKILL_SUMMON2:
		monster = gServerInfo.m_SummonMonster2;
		break;
	case SKILL_SUMMON3:
		monster = gServerInfo.m_SummonMonster3;
		break;
	case SKILL_SUMMON4:
		monster = gServerInfo.m_SummonMonster4;
		break;
	case SKILL_SUMMON5:
		monster = gServerInfo.m_SummonMonster5;
		break;
	case SKILL_SUMMON6:
		monster = gServerInfo.m_SummonMonster6;
		break;
	case SKILL_SUMMON7:
		monster = gServerInfo.m_SummonMonster7;
		break;
	case SKILL_SUMMON_SATYROS:
		monster = gServerInfo.m_SummonMonster8;
		break;
	default:
		return 0;
	}
	if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonKill(aIndex);
		GCSummonLifeSend(aIndex, 0, 1);
		return 0;
	}
	int index = gObjAddSummon();
	if (OBJECT_RANGE(index) == 0)
	{
		return 0;
	}
	lpObj->SummonIndex = index;
	LPOBJ lpSummon = &gObj[index];
	lpSummon->X = lpObj->X - 1;
	lpSummon->Y = lpObj->Y + 1;
	lpSummon->MTX = lpSummon->X;
	lpSummon->MTY = lpSummon->Y;
	lpSummon->Dir = 2;
	lpSummon->Map = lpObj->Map;
	gObjSetMonster(index, monster);
	lpSummon->Life += (float)((__int64)lpSummon->Life * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->MaxLife += (float)((__int64)lpSummon->MaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->ScriptMaxLife += (float)((__int64)lpSummon->ScriptMaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
	lpSummon->Defense += ((__int64)lpSummon->Defense * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DEFENSE)) / 100;
	lpSummon->PhysiDamageMin += ((__int64)lpSummon->PhysiDamageMin * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;
	lpSummon->PhysiDamageMax += ((__int64)lpSummon->PhysiDamageMax * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;
	lpSummon->SummonIndex = aIndex;
	lpSummon->Attribute = 100;
	lpSummon->TargetNumber = -1;
	lpSummon->ActionState.Attack = 0;
	lpSummon->ActionState.Emotion = 0;
	lpSummon->ActionState.EmotionCount = 0;
	lpSummon->PathCount = 0;
	lpSummon->MoveRange = 15;
	GCSummonLifeSend(lpSummon->SummonIndex, (int)lpSummon->Life, (int)lpSummon->MaxLife);
	return 1;
}
bool CSkillManager::SkillDecay(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillIceStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillNova(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (bIndex != SKILL_NOVA_START && bIndex != MASTER_SKILL_ADD_NOVA_START_IMPROVED)
	{
		if (lpObj->SkillNovaState == 0)
		{
			return 0;
		}
	}
	else
	{
		if (lpObj->SkillNovaState != 0)
		{
			return 0;
		}
		lpObj->SkillNovaState = 1;
		lpObj->SkillNovaCount = 0;
		lpObj->SkillNovaTime = GetTickCount();
		this->GCSkillAttackSend(lpObj, bIndex, aIndex, 1);
		return 1;
	}
	lpObj->SkillNovaState = 0;
	lpObj->SkillNovaTime = 0;
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 600, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillTwistingSlash(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillRagefulBlow(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 500, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillDeathStab(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, combo);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillGreaterLife(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	int value1 = gServerInfo.m_GreaterLifeConstA + ((lpObj->Vitality + lpObj->AddVitality) / gServerInfo.m_GreaterLifeConstB) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterLifeConstC);
	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_IMPROVED);
	value1 = ((value1 > gServerInfo.m_GreaterLifeMaxRate) ? gServerInfo.m_GreaterLifeMaxRate : value1);
	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_ENHANCED);
	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_LIFE_MASTERED);
	int count = gServerInfo.m_GreaterLifeTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterLifeTimeConstB);
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		value1 = (value1 * ((lpObj->Type == OBJECT_USER) ? gServerInfo.m_GreaterLifeRate[lpObj->Class] : 100)) / 100;
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];
			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}
			if (lpObj->Map != gObj[index].Map)
			{
				continue;
			}
			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}
			value1 = (value1 * ((gObj[index].Type == OBJECT_USER) ? gServerInfo.m_GreaterLifeRate[gObj[index].Class] : 100)) / 100;
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillMonsterAreaAttack(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillPowerSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int SkillFrustrumX[4], SkillFrustrumY[4];
	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 6.0f, 6.0f, 1.0f, 0.0f);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillForce(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	if (lpObj->SkillLongSpearChange == 0)
	{
		gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, lpTarget->Index, 1);
		}
		return 1;
	}
	int angle = this->GetSkillAngle(lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (gSkillHitBox.HitCheck(angle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillFireBurst(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 0, combo);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		if (lpObj->VpPlayer2[n].index == bIndex)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillEarthquake(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 500, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillSummonParty(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	if (OBJECT_RANGE(lpObj->PartyNumber) != 0)
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];
			if (OBJECT_RANGE(index) == 0 || aIndex == index)
			{
				continue;
			}
			if (gMoveSummon.CheckMoveSummon(&gObj[index], lpObj->Map, lpObj->X, lpObj->Y) == 0)
			{
				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, pMessage->Text(268));
				continue;
			}
#if(GAMESERVER_TYPE==1)
			if (lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiege.GetCastleState() == CASTLESIEGE_STATE_STARTSIEGE && lpObj->CsJoinSide != gObj[index].CsJoinSide)
			{
				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, pMessage->Text(268));
				continue;
			}
#endif
			int px = lpObj->X;
			int py = lpObj->Y;
			if (gObjGetRandomFreeLocation(lpObj->Map, &px, &py, 4, 4, 10) == 0)
			{
				gObj[index].SkillSummonPartyTime = 5;
				gObj[index].SkillSummonPartyMap = lpObj->Map;
				gObj[index].SkillSummonPartyX = (BYTE)lpObj->X;
				gObj[index].SkillSummonPartyY = (BYTE)lpObj->Y;
				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, pMessage->Text(273), gObj[index].SkillSummonPartyTime);
			}
			else
			{
				gObj[index].SkillSummonPartyTime = 5;
				gObj[index].SkillSummonPartyMap = lpObj->Map;
				gObj[index].SkillSummonPartyX = (BYTE)px;
				gObj[index].SkillSummonPartyY = (BYTE)py;
				gNotice.GCNoticeSend(index, 1, 0, 0, 0, 0, 0, pMessage->Text(273), gObj[index].SkillSummonPartyTime);
			}
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillGreaterCriticalDamage(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	int value1 = ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterCriticalDamageConstA) + ((lpObj->Leadership + lpObj->AddLeadership) / gServerInfo.m_GreaterCriticalDamageConstB);
	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_IMPROVED);
	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_MASTERED);
	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_EXTENDED);
	int count = gServerInfo.m_GreaterCriticalDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterCriticalDamageTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_CRITICAL_DAMAGE_ENHANCED);
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];
			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}
			if (lpObj->Map != gObj[index].Map)
			{
				continue;
			}
			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillElectricSpark(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int x = lpObj->X - (8 - (angle & 15));
	int y = lpObj->Y - (8 - ((angle & 240) / 16));
	int nangle = this->GetSkillAngle(lpObj->X, lpObj->Y, x, y);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (gSkillHitBox.HitCheck(nangle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1000, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillCastleSiege(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gServerInfo.m_EnableCsSkillsAllMaps == 0 && (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS))
	{
		return 0;
	}
	if(bIndex > MAX_OBJECT) return 0;
	if(this->CheckSkillRange(lpSkill->m_index,lpObj->X,lpObj->Y,gObj[bIndex].X,gObj[bIndex].Y) == 0)
	{
		return 0;
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	if (lpSkill->m_skill == SKILL_CRESCENT_MOON_SLASH || lpSkill->m_skill == SKILL_SPIRAL_SLASH || lpSkill->m_skill == SKILL_MANA_RAYS)
	{
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, combo);
	}
	else if (lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_FIRE_BLAST)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, combo);
	}
	else if (lpSkill->m_skill == SKILL_MANA_GLAIVE || lpSkill->m_skill == SKILL_STAR_FALL)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, combo);
	}
	else if (lpSkill->m_skill == SKILL_SPIRAL_SLASH)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 50, bIndex, 500, lpSkill->m_index, combo);
	}
	else if (lpSkill->m_skill == SKILL_CHARGE)
	{
		gAttack.Attack(lpObj, &gObj[bIndex], lpSkill, 0, 0, 0, 0, combo);
	}
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillStern(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	int x = lpObj->X - (8 - (angle & 15));
	int y = lpObj->Y - (8 - ((angle & 240) / 16));
	int nangle = this->GetSkillAngle(lpObj->X, lpObj->Y, x, y);
	gObjSetKillCount(aIndex, 0);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (gSkillHitBox.HitCheck(nangle, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			if (gObj[index].Teleport == 0 && gEffectManager.CheckEffect(&gObj[index], EFFECT_IRON_DEFENSE) == 0 && gEffectManager.CheckEffect(&gObj[index], EFFECT_IRON_DEFENSE_IMPROVED) == 0)
			{
				gEffectManager.AddEffect(&gObj[index], 0, EFFECT_STERN, gServerInfo.m_SternTimeConstA, 0, 0, 0, 0);
				gObjSetPosition(index, gObj[index].X, gObj[index].Y);
				this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
			}
		}
	}
	return 1;
}
bool CSkillManager::SkillRemoveStern(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.DelEffect(&gObj[index], EFFECT_STERN);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillGreaterMana(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_GreaterManaTimeConstA, gServerInfo.m_GreaterManaConstA, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (lpObj->GuildNumber <= 0 || gObj[index].GuildNumber <= 0)
		{
			continue;
		}
		if (lpObj->GuildNumber != gObj[index].GuildNumber)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_GreaterManaTimeConstA, gServerInfo.m_GreaterManaConstA, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillInvisibility(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	gEffectManager.AddEffect(lpTarget, 0, EFFECT_INVISIBILITY, gServerInfo.m_InvisibilityTimeConstA, 0, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillRemoveInvisibility(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.DelEffect(&gObj[index], EFFECT_INVISIBILITY);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillRemoveAllEffect(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.ClearAllEffect(&gObj[index]);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillBrand(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Map != MAP_CASTLE_SIEGE && lpObj->Map != MAP_LAND_OF_TRIALS)
	{
		return 0;
	}
	gObjSetKillCount(aIndex, 0);
	gEffectManager.AddEffect(lpObj, 0, EFFECT_GUILD_STATE5, gServerInfo.m_BrandTimeConstA, 0, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].type != OBJECT_USER)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) != 0)
		{
			gEffectManager.AddEffect(&gObj[index], 0, EFFECT_GUILD_STATE5, gServerInfo.m_BrandTimeConstA, 0, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillPlasmaStorm(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 500, lpSkill->m_index, 0);
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillInfinityArrow(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int value = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFINITY_ARROW_IMPROVED);
	int count = gServerInfo.m_InfinityArrowTimeConstA;
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillMonsterSummon(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if ((GetLargeRand() % 10) == 0)
	{
		return 0;
	}
	int MonsterClass = 0;
	switch (lpObj->Class)
	{
	case 161:
		MonsterClass = 147 + GetLargeRand() % 2;
		break;
	case 181:
		MonsterClass = 177 + GetLargeRand() % 2;
		break;
	case 189:
		MonsterClass = 185 + GetLargeRand() % 2;
		break;
	case 197:
		MonsterClass = 193 + GetLargeRand() % 2;
		break;
	case 267:
		MonsterClass = 263 + GetLargeRand() % 2;
		break;
	case 275:
		MonsterClass = 271 + GetLargeRand() % 2;
		break;
	default:
		return 0;
	}
	bool success = 0;
	int x = lpObj->X;
	int y = lpObj->Y;
	if (gObjGetRandomFreeLocation(lpObj->Map, &x, &y, 2, 2, 10) != 0)
	{
		int index = gObjAddMonster(lpObj->Map);
		if (OBJECT_RANGE(index) != 0)
		{
			LPOBJ lpSummon = &gObj[index];
			lpSummon->PosNum = -1;
			lpSummon->X = x;
			lpSummon->Y = y;
			lpSummon->TX = x;
			lpSummon->TY = y;
			lpSummon->OldX = x;
			lpSummon->OldY = y;
			lpSummon->StartX = x;
			lpSummon->StartY = y;
			lpSummon->Dir = GetLargeRand() % 8;
			lpSummon->Map = lpObj->Map;
			gObjSetMonster(index, MonsterClass);
			lpSummon->Attribute = 60;
			lpSummon->MaxRegenTime = 0;
			success = 1;
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, ((y * 256) + x), success);
	return success;
}
bool CSkillManager::SkillMagicDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	gEffectManager.AddEffect(lpObj, 0, EFFECT_MAGIC_DAMAGE_IMMUNITY, gServerInfo.m_MagicDamageImmunityTimeConstA, 0, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);
	return 1;
}
bool CSkillManager::SkillPhysiDamageImmunity(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	gEffectManager.AddEffect(lpObj, 0, EFFECT_PHYSI_DAMAGE_IMMUNITY, gServerInfo.m_PhysiDamageImmunityTimeConstA, 0, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, lpObj->Index, 1);
	return 1;
}
bool CSkillManager::SkillOrderOfProtection(int aIndex, int bIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	gEffectManager.AddEffect(lpObj, 0, EFFECT_ORDER_OF_PROTECTION, 15, SKILL_ORDER_OF_PROTECTION, 0, 0, 0);
	return 1;
}
bool CSkillManager::SkillOrderOfRestraint(int aIndex, int bIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(SKILL_ORDER_OF_RESTRAINT, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}
	gEffectManager.AddEffect(&gObj[bIndex], 0, EFFECT_ORDER_OF_RESTRAINT, 15, SKILL_ORDER_OF_RESTRAINT, 0, 0, 0);
	gObjSetPosition(bIndex, gObj[bIndex].X, gObj[bIndex].Y);
	return 1;
}
bool CSkillManager::SkillOrderOfTracking(int aIndex, int bIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0 || aIndex == bIndex)
	{
		return 0;
	}
	if (gEffectManager.CheckStunEffect(lpObj) != 0)
	{
		return 0;
	}
	int sx = gObj[bIndex].X;
	int sy = gObj[bIndex].Y;
	for (int n = 0; n < 50; n++)
	{
		int tx = gObj[bIndex].X + ((GetLargeRand() % 3) - 1);
		int ty = gObj[bIndex].Y + ((GetLargeRand() % 3) - 1);
		if (gObjCheckTeleportArea(aIndex, tx, ty) != 0)
		{
			sx = tx;
			sy = ty;
			break;
		}
	}
	gObjTeleportMagicUse(aIndex, sx, sy);
	return 1;
}
bool CSkillManager::SkillOrderOfWeaken(int aIndex, int bIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(SKILL_ORDER_OF_WEAKEN, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) == 0)
	{
		return 0;
	}
	int ShieldDamage = (gObj[bIndex].Shield * 50) / 100;
	if (gObj[bIndex].Shield < ShieldDamage)
	{
		ShieldDamage = gObj[bIndex].Shield;
		gObj[bIndex].Shield = 0;
	}
	else
	{
		ShieldDamage = ShieldDamage;
		gObj[bIndex].Shield -= ShieldDamage;
	}
	gGetInfoCharacter.GCDamageSend(aIndex, bIndex, 0, 0, 0, ShieldDamage);
	return 1;
}
bool CSkillManager::SkillChainLightning(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int count = 1;
	int target[3] = { bIndex,bIndex,bIndex };
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (bIndex == index)
		{
			continue;
		}
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		target[count++] = index;
		if (count >= _countof(target))
		{
			break;
		}
	}
	BYTE send[256];
	PMSG_SKILL_CHAIN_SKILL_SEND pMsg;
	pMsg.header.set(0xBF, 0x0A, 0);
	int size = sizeof(pMsg);
	pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	pMsg.index = aIndex;
	pMsg.count = 0;
	PMSG_SKILL_CHAIN_SKILL info;
	for (int n = 0; n < _countof(target); n++)
	{
		gObjAddAttackProcMsgSendDelay(lpObj, 52, target[n], (200 + (n * 200)), lpSkill->m_index, (n + 1));
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, target[n], 1);
		}
		info.index = target[n];
		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
	MsgSendV2(lpObj, send, size);
	return 1;
}
bool CSkillManager::SkillDamageReflect(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = gServerInfo.m_ReflectDamageConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ReflectDamageConstB);
	value = ((value > gServerInfo.m_ReflectDamageMaxRate) ? gServerInfo.m_ReflectDamageMaxRate : value);
	value = (value * ((lpTarget->Type == OBJECT_USER) ? gServerInfo.m_ReflectDamageRate[lpTarget->Class] : 100)) / 100;
	int count = gServerInfo.m_ReflectDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ReflectDamageTimeConstB);
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillSwordPower(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int value1 = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstA;
	value1 = ((value1 > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value1);
	int value2 = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstA;
	value2 = ((value2 > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value2);
	int value3 = (((40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)) < 10) ? 10 : (40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)));
	int value4 = (((40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)) < 10) ? 10 : (40 - ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerConstB)));
	int count = gServerInfo.m_SwordPowerTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SwordPowerTimeConstB);
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, value4);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillSwordPowerGetDefense(int aIndex, int* defense)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}
	int value = lpObj->EffectOption.AddSwordPowerDefenseRate;
	(*defense) -= (((lpObj->Dexterity + lpObj->AddDexterity) / gServerInfo.m_SUDefenseConstA) * value) / 100;
	return 1;
}
bool CSkillManager::SkillSwordPowerGetPhysiDamage(int aIndex, int* DamageMin, int* DamageMax)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}
	int value = lpObj->EffectOption.AddSwordPowerDamageRate;
	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);
	(*DamageMin) += ((((lpObj->Strength + lpObj->AddStrength) + (lpObj->Dexterity + lpObj->AddDexterity)) / gServerInfo.m_SUPhysiDamageMinConstA) * value) / 100;
	(*DamageMax) += ((((lpObj->Strength + lpObj->AddStrength) + (lpObj->Dexterity + lpObj->AddDexterity)) / gServerInfo.m_SUPhysiDamageMaxConstA) * value) / 100;
	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	return 1;
}
bool CSkillManager::SkillSwordPowerGetMagicDamage(int aIndex, int* DamageMin, int* DamageMax)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}
	int value = lpObj->EffectOption.AddSwordPowerDamageRate;
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_ENHANCED);
	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);
	(*DamageMin) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMinConstA) * value) / 100;
	(*DamageMax) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMaxConstA) * value) / 100;
	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	return 1;
}
bool CSkillManager::SkillSwordPowerGetCurseDamage(int aIndex, int* DamageMin, int* DamageMax)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_IMPROVED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_ENHANCED) == 0 && gEffectManager.CheckEffect(lpObj, EFFECT_SWORD_POWER_MASTERED) == 0)
	{
		return 0;
	}
	int value = lpObj->EffectOption.AddSwordPowerDamageRate;
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_IMPROVED);
	value = ((value > gServerInfo.m_SwordPowerMaxRate) ? gServerInfo.m_SwordPowerMaxRate : value);
	(*DamageMin) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMinConstA) * value) / 100;
	(*DamageMax) += (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SUMagicDamageMaxConstA) * value) / 100;
	(*DamageMin) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	(*DamageMax) += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SWORD_POWER_MASTERED);
	return 1;
}
bool CSkillManager::SkillSleep(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type == OBJECT_USER && lpTarget->Authority == 32)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	if (lpTarget->Type == OBJECT_USER && this->CheckSkillTarget(lpObj, bIndex, -1, lpTarget->Type) == 0)
	{
		return 0;
	}
	int rate = gServerInfo.m_SleepConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SleepConstB);
	rate += lpObj->Inventory[1].GetBookSuccessRate();
	rate += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SLEEP_IMPROVED);
	int count = gServerInfo.m_SleepTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_SleepTimeConstB);
	count = ((count > gServerInfo.m_SleepMaxTime) ? gServerInfo.m_SleepMaxTime : count);
	if ((GetLargeRand() % 100) >= rate)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}
	if (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) != 0)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, 0, 0, 0, 0);
	gObjSetPosition(bIndex, lpTarget->X, lpTarget->Y);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillLesserDefense(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	int rate = gServerInfo.m_LesserDefenseConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseConstB);
	rate += lpObj->Inventory[1].GetBookSuccessRate();
	int value = gServerInfo.m_LesserDefenseConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseConstD);
	value = ((value > gServerInfo.m_LesserDefenseMaxRate) ? gServerInfo.m_LesserDefenseMaxRate : value);
	int count = gServerInfo.m_LesserDefenseTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDefenseTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LESSER_DEFENSE_IMPROVED);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if ((GetLargeRand() % 100) >= rate)
		{
			gAttack.MissSend(lpObj, &gObj[index], lpSkill, 1, 0);
		}
		else
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillLesserDamage(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	int rate = gServerInfo.m_LesserDamageConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageConstB);
	rate += lpObj->Inventory[1].GetBookSuccessRate();
	int value = gServerInfo.m_LesserDamageConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageConstD);
	value = ((value > gServerInfo.m_LesserDamageMaxRate) ? gServerInfo.m_LesserDamageMaxRate : value);
	int count = gServerInfo.m_LesserDamageTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_LesserDamageTimeConstB);
	count += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LESSER_DAMAGE_IMPROVED);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if ((GetLargeRand() % 100) >= rate)
		{
			gAttack.MissSend(lpObj, &gObj[index], lpSkill, 1, 0);
		}
		else
		{
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillSahamutt(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1000, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillNeil(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1000, lpSkill->m_index, 0);
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillGhostPhantom(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 1000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 2000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 3000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 4000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	gObjAddAttackProcMsgSendDelay(lpObj, 55, bIndex, 5000, lpSkill->m_index, MAKE_NUMBERDW(x, y));
	return 1;
}
bool CSkillManager::SkillGhostPhantomAreaAttack(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillRedStorm(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 250, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillFrozenStab(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 500, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillMagicCircle(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int value1 = (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DWMagicDamageMinConstA) * gServerInfo.m_MagicCircleConstA) / 100;
	int value2 = (((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DWMagicDamageMaxConstA) * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_IMPROVED)) / 100;
	int value3 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_MAGIC_CIRCLE_ENHANCED);
	int count = gServerInfo.m_MagicCircleTimeConstA;
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, value3, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillShieldRecover(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = ((lpSkill->m_level + 1) * gServerInfo.m_ShieldRecoverConstA) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_ShieldRecoverConstB);
	if ((lpTarget->Shield + value) > (lpTarget->MaxShield + lpTarget->AddShield))
	{
		lpTarget->Shield = lpTarget->MaxShield + lpTarget->AddShield;
	}
	else
	{
		lpTarget->Shield += value;
	}
	GCLifeSend(bIndex, 0xFF, (int)lpTarget->Life, lpTarget->Shield);
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state != VIEWPORT_NONE && lpObj->VpPlayer2[n].type == OBJECT_MONSTER && gObj[lpObj->VpPlayer2[n].index].CurrentAI != 0)
		{
			gObj[lpObj->VpPlayer2[n].index].Agro.IncAgro(lpObj->Index, (value / 40));
		}
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillFiveShot(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int SkillFrustrumX[4], SkillFrustrumY[4];
	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 6.0f, 7.0f, 2.0f, 0.0f);
	vec3_t Angle;
	Vector(0.0f, 6.0f, 0.0f, Angle);
	vec3_t p[4];
	Vector(0.0f, 0.0f, 40.0f, p[0]);
	Vector(0.0f, 0.0f, 20.0f, p[1]);
	Vector(0.0f, 0.0f, 340.0f, p[2]);
	Vector(0.0f, 0.0f, 320.0f, p[3]);
	float Matrix[3][4];
	vec3_t vFrustrum[5], vFrustrum2[5];
	AngleMatrix(p[0], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[0]);
	AngleMatrix(p[1], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[1]);
	Vector(0.0f, 6.0f, 0.0f, vFrustrum[2]);
	AngleMatrix(p[2], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[3]);
	AngleMatrix(p[3], Matrix);
	VectorRotate(Angle, Matrix, vFrustrum[4]);
	Vector(0.0f, 0.0f, (vec_t)((angle * 360) / 255), p[0]);
	AngleMatrix((float*)p, Matrix);
	int ArrowFrustrum[5], ArrowFrustrumX[5], ArrowFrustrumY[5];
	for (int n = 0; n < 5; n++)
	{
		VectorRotate(vFrustrum[n], Matrix, vFrustrum2[n]);
		ArrowFrustrumX[n] = (int)vFrustrum2[n][0] + lpObj->X;
		ArrowFrustrumY[n] = (int)vFrustrum2[n][1] + lpObj->Y;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		for (int i = 0; i < 5; i++)
		{
			if ((ArrowFrustrum[i] = ((ArrowFrustrumX[i] - lpObj->X) * (gObj[index].Y - lpObj->Y)) - ((ArrowFrustrumY[i] - lpObj->Y) * (gObj[index].X - lpObj->X))) > -5 && ArrowFrustrum[i] < 5)
			{
				gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, combo);
			}
		}
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillSwordSlash(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int SkillFrustrumX[4], SkillFrustrumY[4];
	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 2.0f, 4.0f, 5.0f, 0.0f);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 0, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillLightningStorm(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 400, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillBirds(int aIndex, int bIndex, CSkill* lpSkill, BYTE angle, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int SkillFrustrumX[4], SkillFrustrumY[4];
	this->GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle, lpObj->X, lpObj->Y, 1.5f, 6.0f, 1.5f, 0.0f);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (this->CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		if (OBJECT_RANGE(bIndex) != 0 && index == bIndex)
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 200, lpSkill->m_index, combo);
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 300, lpSkill->m_index, combo);
		}
		else
		{
			gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 200, lpSkill->m_index, combo);
		}
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillSelfExplosion(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 0, 0);
		if (CHECK_SKILL_ATTACK_EXTENDED_COUNT(count) == 0)
		{
			break;
		}
	}
	lpObj->Life = 0;
	gObjectManager.CharacterLifeCheck(lpObj, lpObj, 0, 1, 0, 0, 0, 0);
	return 1;
}
bool CSkillManager::SkillLargeRingBlower(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, combo);
	return 1;
}
bool CSkillManager::SkillUpperBeast(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillChainDriver(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 3, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 4, combo);
	if (gCustomMonster.IsCustomMonster(lpTarget->Class))
	{
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 150, 0);
	}
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillDarkSide(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 1, combo);
	gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 0, 0, 2, combo);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillDarkSideGetTargetIndex(int aIndex, int bIndex, CSkill* lpSkill, WORD* target)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	if (OBJECT_RANGE(bIndex) != 0 && this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[bIndex].X, gObj[bIndex].Y) != 0)
	{
		target[count++] = bIndex;
	}
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		target[count] = index;
		if ((++count) >= 5)
		{
			break;
		}
	}
	lpObj->RageFighterSkillTarget = target[0];
	return 1;
}
bool CSkillManager::SkillDragonLore(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 1, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 2, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 3, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 4, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillGreaterIgnoreDefenseRate(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int value = gServerInfo.m_GreaterIgnoreDefenseRateConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterIgnoreDefenseRateConstB);
	int count = gServerInfo.m_GreaterIgnoreDefenseRateTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterIgnoreDefenseRateTimeConstB);
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillFitness(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	int value = gServerInfo.m_FitnessConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_FitnessConstB);
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FITNESS_IMPROVED);
	int count = gServerInfo.m_FitnessTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_FitnessTimeConstB);
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];
			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}
			if (lpObj->Map != gObj[index].Map)
			{
				continue;
			}
			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillGreaterDefenseSuccessRate(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	int value1 = gServerInfo.m_GreaterDefenseSuccessRateConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseSuccessRateConstB);
	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_SUCCESS_RATE_IMPROVED);
	int value2 = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GREATER_DEFENSE_SUCCESS_RATE_ENHANCED);
	int count = gServerInfo.m_GreaterDefenseSuccessRateTimeConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_GreaterDefenseSuccessRateTimeConstB);
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	}
	else
	{
		PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
		for (int n = 0; n < MAX_PARTY_USER; n++)
		{
			int index = lpParty->Index[n];
			if (OBJECT_RANGE(index) == 0)
			{
				continue;
			}
			if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
			{
				continue;
			}
			gEffectManager.AddEffect(&gObj[index], 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
			this->GCSkillAttackSend(lpObj, lpSkill->m_index, index, 1);
		}
	}
	return 1;
}
bool CSkillManager::SkillPhoenixShot(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 1, 0, 0, 1, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 2, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 3, combo);
		gAttack.Attack(lpObj, &gObj[index], lpSkill, 0, 0, 0, 4, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillBloodStorm(int aIndex, int bIndex, CSkill* lpSkill, BYTE x, BYTE y, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, x, y) == 0)
	{
		return 0;
	}
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, x, y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 1000, lpSkill->m_index, combo);
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1500, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillCure(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gEffectManager.ClearDebuffEffect(lpTarget, 1);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillPartyHeal(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER)
	{
		return 0;
	}
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		return 0;
	}
	if (CA_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0)
	{
		return 0;
	}
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER)
	{
		return 0;
	}
	if (OBJECT_RANGE(lpTarget->PartyNumber) == 0)
	{
		return 0;
	}
	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int count = 1;
	int target[3] = { bIndex,-1,-1 };
	PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
	for (int n = 0; n < MAX_PARTY_USER; n++)
	{
		int index = lpParty->Index[n];
		if (OBJECT_RANGE(index) == 0)
		{
			continue;
		}
		if (bIndex == index)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpTarget->X, lpTarget->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		target[count++] = index;
		if (count >= _countof(target))
		{
			break;
		}
	}
	int value = gServerInfo.m_PartyHealConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_PartyHealConstB);
	value += (value * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_PARTY_HEAL_IMPROVED)) / 100;
	BYTE send[256];
	PMSG_SKILL_CHAIN_SKILL_SEND pMsg;
	pMsg.header.set(0xBF, 0x18, 0);
	int size = sizeof(pMsg);
	pMsg.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	pMsg.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	pMsg.index = aIndex;
	pMsg.count = 0;
	PMSG_SKILL_CHAIN_SKILL info;
	for (int n = 0; n < count; n++)
	{
		gObjAddMsgSendDelay(&gObj[target[n]], 16, target[n], (200 + (n * 200)), ((value * (100 - (n * 20))) / 100));
		info.index = target[n];
		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
	MsgSendV2(lpObj, send, size);
	return 1;
}
bool CSkillManager::SkillPoisonArrow(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 0, 0, 0, combo);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	if (combo != 0)
	{
		this->GCSkillAttackSend(lpObj, SKILL_COMBO, bIndex, 1);
	}
	return 1;
}
bool CSkillManager::SkillBless(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type != OBJECT_USER && OBJECT_RANGE(lpTarget->SummonIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return 0;
	}
	if (CA_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (CC_MAP_RANGE(lpObj->Map) != 0 && aIndex != bIndex)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	int value = (lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_BlessConstA;
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLESS_IMPROVED);
	int count = gServerInfo.m_BlessTimeConstA;
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
	gFruit.GCFruitResultSend(lpTarget, 17, value, 7);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillBlind(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return 0;
	}
	LPOBJ lpTarget = &gObj[bIndex];
	if (lpTarget->Type == OBJECT_USER && lpTarget->Authority == 32)
	{
		return 0;
	}
	if (this->CheckSkillRange(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
	{
		return 0;
	}
	if (lpTarget->Type == OBJECT_USER && this->CheckSkillTarget(lpObj, bIndex, -1, lpTarget->Type) == 0)
	{
		return 0;
	}
	int rate = gServerInfo.m_BlindConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_BlindConstB);
	if ((GetLargeRand() % 100) >= rate)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}
	if (gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE) != 0 || gEffectManager.CheckEffect(lpTarget, EFFECT_IRON_DEFENSE_IMPROVED) != 0)
	{
		gAttack.MissSend(lpObj, lpTarget, lpSkill, 1, 0);
		return 0;
	}
	if (lpSkill->m_index == MASTER_SKILL_ADD_BLIND_IMPROVED && (GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_BLIND_IMPROVED))
	{
		gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), 2, 0, 0, 0, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), gServerInfo.m_BlindTimeConstA, gServerInfo.m_BlindConstC, 0, 0, 0);
	}
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	return 1;
}
bool CSkillManager::SkillEarthPrison(int aIndex, int bIndex, CSkill* lpSkill, bool combo)
{
	LPOBJ lpObj = &gObj[aIndex];
	int count = 0;
	for (int n = 0; n < MAX_VIEWPORT; n++)
	{
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE)
		{
			continue;
		}
		int index = lpObj->VpPlayer2[n].index;
		if (this->CheckSkillTarget(lpObj, index, bIndex, lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		if (this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		gObjAddAttackProcMsgSendDelay(lpObj, 53, index, 1200, lpSkill->m_index, combo);
		gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 1500, lpSkill->m_index, combo);
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, index, 1);
		}
		if (CHECK_SKILL_ATTACK_COUNT(count) == 0)
		{
			break;
		}
	}
	return 1;
}
bool CSkillManager::SkillIronDefense(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int value1 = gServerInfo.m_IronDefenseConstA;
	value1 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_IRON_DEFENSE_IMPROVED);
	int value2 = gServerInfo.m_IronDefenseConstB;
	value2 += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_IRON_DEFENSE_IMPROVED);
	int count = gServerInfo.m_IronDefenseTimeConstA;
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, value1, value2, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
bool CSkillManager::SkillBloodHowling(int aIndex, int bIndex, CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || aIndex != bIndex)
	{
		return 0;
	}
	int count = gServerInfo.m_BloodHowlingTimeConstA;
	gEffectManager.AddEffect(lpObj, 0, this->GetSkillEffect(lpSkill->m_index), count, 0, 0, 0, 0);
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
	return 1;
}
void CSkillManager::ApplyMeteoriteEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_METEORITE_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 3, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyIceStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_ICE_STORM_ENHANCED))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_ICE_STORM_ENHANCED, 3, 0, 0, 0, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyTwistingSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_TWISTING_SLASH_ENHANCED))
	{
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 150, 0);
	}
}
void CSkillManager::ApplyRagefulBlowEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_RAGEFUL_BLOW_ENHANCED))
	{
		if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
		{
			CItem* lpItem = &lpTarget->Inventory[(damage = (2 + (GetLargeRand() % 5)))];
			if (lpItem->IsItem() != 0 && lpItem->IsLuckyItem() == 0 && lpItem->m_IsPeriodicItem == 0)
			{
				lpItem->m_Durability = (lpItem->m_Durability * 90) / 100;
				gItemManager.GCItemDurSend(lpTarget->Index, damage, (BYTE)lpItem->m_Durability, 0);
			}
		}
	}
}
void CSkillManager::ApplyDeathStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_MASTERED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, gServerInfo.ThoiGianChoangDamGio, 0, 0, 0, 0);
	}
	else if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DEATH_STAB_ENHANCED))
	{
		damage = ((lpObj->Strength + lpObj->AddStrength) * 10) / 100;
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_DEATH_STAB_ENHANCED, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
	}
}
void CSkillManager::ApplyFireSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	int value = gServerInfo.m_FireSlashConstA + ((lpObj->Strength + lpObj->AddStrength) / gServerInfo.m_FireSlashConstB);
	value += gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_SLASH_ENHANCED);
	value = ((value > gServerInfo.m_FireSlashMaxRate) ? gServerInfo.m_FireSlashMaxRate : value);
	int count = gServerInfo.m_FireSlashTimeConstA;
	gEffectManager.AddEffect(lpTarget, 0, this->GetSkillEffect(lpSkill->m_index), count, value, 0, 0, 0);
}
void CSkillManager::ApplyFireBurstEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIRE_BURST_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyPlasmaStormEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if (lpObj->Type == OBJECT_USER && lpTarget->Type == OBJECT_USER)
	{
		CItem* lpItem = &lpTarget->Inventory[(damage = (2 + (GetLargeRand() % 5)))];
		if (lpItem->IsItem() != 0 && lpItem->IsLuckyItem() == 0 && lpItem->m_IsPeriodicItem == 0)
		{
			lpItem->m_Durability = (lpItem->m_Durability * 50) / 100;
			gItemManager.GCItemDurSend(lpTarget->Index, damage, (BYTE)lpItem->m_Durability, 0);
		}
	}
}
void CSkillManager::ApplyFireScreamEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
}
void CSkillManager::ApplyEarthquakeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_EARTHQUAKE_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}
	else
	{
		gObjBackSpring2(lpTarget, lpObj, 3);
	}
}
void CSkillManager::ApplyDrainLifeEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	damage = ((damage * gServerInfo.m_DrainLifeConstA) / 100) + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DrainLifeConstB);
	if ((lpObj->Life + damage) > (lpObj->MaxLife + lpObj->AddLife))
	{
		lpObj->Life = lpObj->MaxLife + lpObj->AddLife;
	}
	else
	{
		lpObj->Life += damage;
	}
	GCLifeSend(lpObj->Index, 0xFF, (int)lpObj->Life, lpObj->Shield);
	if ((GetLargeRand() % 100) < 30)
	{
		damage = gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAIN_LIFE_ENHANCED);
		if (damage > 0) { gEffectManager.AddEffect(lpTarget, 0, EFFECT_DRAIN_LIFE_ENHANCED, 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage)); }
	}
}
void CSkillManager::ApplySahamuttEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	damage = (damage * gServerInfo.m_SahamuttConstA) / 100;
	damage += (damage * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SAHAMUTT_ENHANCED)) / 100;
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
}
void CSkillManager::ApplyNeilEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_NEIL_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}
	damage = (damage * gServerInfo.m_NeilConstA) / 100;
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
}
void CSkillManager::ApplyGhostPhantomEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_GHOST_PHANTOM_ENHANCED))
	{
		gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 150, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 2, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyFrozenStabEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FROZEN_STAB_MASTERED))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_FROZEN_STAB_MASTERED, 5, 0, 0, 0, 0);
	}
	else if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FROZEN_STAB_ENHANCED))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_ICE_ARROW, 3, 0, 0, 0, 0);
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyFiveShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % gServerInfo.TyLeRaChoangSkillMasterNguTien) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_FIVE_SHOT_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, gServerInfo.ThoiGianChoangSkillMasterNguTien, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplySwordSlashEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 1, 0, 0, 0, 0);
}
void CSkillManager::ApplyLargeRingBlowerEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (5 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_LARGE_RING_BLOWER_ENHANCED)))
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 5, 0, 0, 0);
	}
}
void CSkillManager::ApplyUpperBeastEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (5 + gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_UPPER_BEAST_ENHANCED)))
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 10, 0, 0, 0);
	}
}
void CSkillManager::ApplyChainDriverEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_CHAIN_DRIVER_ENHANCED))
	{
		damage = 100;
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_CHAIN_DRIVER_ENHANCED, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
	}
	else
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 10, 0, 0, 0, 0);
	}
}
void CSkillManager::ApplyVongTronLuaEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_INFERNO_IMPROVED1))
	{
		damage = 100;
		gEffectManager.AddEffect(lpTarget, 0, MASTER_SKILL_ADD_INFERNO_IMPROVED1, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
	}
}
void CSkillManager::ApplyDragonLoreEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_LORE_ENHANCED))
	{
		damage = 100;
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_DRAGON_ROAR_ENHANCED, 10, lpObj->Index, 1, SET_NUMBERHW(damage), SET_NUMBERLW(damage));
	}
}
void CSkillManager::ApplyDragonSlayerEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < (gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_DRAGON_SLAYER_ENHANCED) - lpTarget->ResistStunRate))
	{
		gEffectManager.AddEffect(lpTarget, 0, EFFECT_STERN, 2, 0, 0, 0, 0);
	}
	if (lpTarget->Type != OBJECT_USER)
	{
		return;
	}
	int rate = gServerInfo.m_DragonSlayerConstA + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DragonSlayerConstB);
	if ((GetLargeRand() % 100) >= rate)
	{
		return;
	}
	int value = gServerInfo.m_DragonSlayerConstC + ((lpObj->Energy + lpObj->AddEnergy) / gServerInfo.m_DragonSlayerConstD);
	value = ((value > gServerInfo.m_DragonSlayerMaxRate) ? gServerInfo.m_DragonSlayerMaxRate : value);
	int ShieldDamage = ((lpTarget->MaxShield + lpTarget->AddShield) * value) / 100;
	if (lpTarget->Shield < ShieldDamage)
	{
		ShieldDamage = lpTarget->Shield;
		lpTarget->Shield = 0;
	}
	else
	{
		ShieldDamage = ShieldDamage;
		lpTarget->Shield -= ShieldDamage;
	}
	gGetInfoCharacter.GCDamageSend(lpObj->Index, lpTarget->Index, 0, 0, 0, ShieldDamage);
}
void CSkillManager::ApplyPhoenixShotEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	gObjAddMsgSendDelay(lpTarget, 2, lpObj->Index, 150, 0);
}
void CSkillManager::ApplyEarthPrisonEffect(LPOBJ lpObj, LPOBJ lpTarget, CSkill* lpSkill, int damage)
{
	if ((GetLargeRand() % 100) < gServerInfo.m_EarthPrisonConstA)
	{
		gEffectManager.AddEffect(lpTarget, 0, gSkillManager.GetSkillEffect(lpSkill->m_index), 5, 0, 0, 0, 0);
	}
}
void CSkillManager::CGMultiSkillAttackRecv(PMSG_MULTI_SKILL_ATTACK_RECV* lpMsg, int aIndex, bool type)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}
	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		return;
	}
	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
	{
		return;
	}
	int skill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
	CSkill* lpSkill = 0;
	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);
		if (lpSkill == 0)
		{
			return;
		}
		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];
		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}
	if (lpSkill->m_skill != SKILL_FLAME && lpSkill->m_skill != SKILL_TWISTER && lpSkill->m_skill != SKILL_EVIL_SPIRIT && lpSkill->m_skill != SKILL_HELL_FIRE && lpSkill->m_skill != SKILL_AQUA_BEAM && lpSkill->m_skill != SKILL_BLAST && lpSkill->m_skill != SKILL_INFERNO && lpSkill->m_skill != SKILL_TRIPLE_SHOT && lpSkill->m_skill != SKILL_IMPALE && lpSkill->m_skill != SKILL_MONSTER_AREA_ATTACK && lpSkill->m_skill != SKILL_PENETRATION && lpSkill->m_skill != SKILL_FIRE_SLASH && lpSkill->m_skill != SKILL_FIRE_SCREAM)
	{
		return;
	}
	if (lpObj->Type == OBJECT_USER && (lpObj->MultiSkillIndex == 0 || lpObj->MultiSkillIndex != lpSkill->m_index || (lpObj->MultiSkillCount++) >= 5))
	{
		return;
	}
	if (lpObj->Type == OBJECT_USER && lpSkill->m_skill == SKILL_TRIPLE_SHOT)
	{
		if (lpObj->Inventory[0].IsItem() == 0 || lpObj->Inventory[1].IsItem() == 0)
		{
			return;
		}
		if (lpObj->Inventory[0].m_Index < GET_ITEM(4, 0) || lpObj->Inventory[0].m_Index >= GET_ITEM(5, 0))
		{
			return;
		}
		if (lpObj->Inventory[1].m_Index < GET_ITEM(4, 0) || lpObj->Inventory[1].m_Index >= GET_ITEM(5, 0))
		{
			return;
		}
	}
	lpMsg->count = ((lpMsg->count > 5) ? 5 : lpMsg->count);
	int combo = 0;
	if ((gServerInfo.m_EnableComboToAllSwitch == 1 || gQuest.CheckQuestListState(lpObj, 3, QUEST_FINISH) != 0) && lpObj->ComboSkill.CheckCombo(lpSkill->m_skill) != 0)
	{
		if (gServerInfo.m_CheckAutoComboHack == 0 || (GetTickCount() - lpObj->ComboTime) > ((DWORD)gServerInfo.m_CheckAutoComboHackTolerance))
		{
			combo = 1;
			lpObj->ComboTime = GetTickCount();
		}
	}
	for (int n = 0; n < lpMsg->count; n++)
	{
		PMSG_MULTI_SKILL_ATTACK* lpInfo = (PMSG_MULTI_SKILL_ATTACK*)(((BYTE*)lpMsg) + sizeof(PMSG_MULTI_SKILL_ATTACK_RECV) + (sizeof(PMSG_MULTI_SKILL_ATTACK) * n));
		int bIndex = MAKE_NUMBERW(lpInfo->index[0], lpInfo->index[1]);
		if (OBJECT_RANGE(bIndex) == 0)
		{
			continue;
		}
		LPOBJ lpTarget = &gObj[bIndex];
		if (lpObj->Type == OBJECT_USER && this->CheckSkillRadio(lpSkill->m_index, lpObj->X, lpObj->Y, lpTarget->X, lpTarget->Y) == 0)
		{
			continue;
		}
		if (type != 0)
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 1, 0, 0, combo);
		}
		else if (lpSkill->m_skill == SKILL_FIRE_SLASH)
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 1, 1, 0, 0, combo);
		}
		else
		{
			gAttack.Attack(lpObj, lpTarget, lpSkill, 0, 1, 0, 0, combo);
		}
		if (combo != 0)
		{
			this->GCSkillAttackSend(lpObj, SKILL_COMBO, lpTarget->Index, 1);
		}
	}
}
void CSkillManager::CGSkillAttackRecv(PMSG_SKILL_ATTACK_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGS(aIndex) == 0)
	{
	#ifdef RISE_SLAYER_SERVER_QA
		const int traceDisconnectedSkill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
		if (rise::slayerserver::IsSlayerSkill(traceDisconnectedSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject source-not-connected skill=%d source=%d", traceDisconnectedSkill, aIndex);
	#endif
		return;
	}
	if (lpObj->Live == 0)
	{
	#ifdef RISE_SLAYER_SERVER_QA
		const int traceDeadSkill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
		if (rise::slayerserver::IsSlayerSkill(traceDeadSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject source-not-live skill=%d source=%d type=%d connected=%d", traceDeadSkill, aIndex, lpObj->Type, lpObj->Connected);
	#endif
		return;
	}
	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	if (OBJECT_RANGE(bIndex) == 0)
	{
		return;
	}
	LPOBJ lpTarget = &gObj[bIndex];
#ifdef RISE_SLAYER_SERVER_QA
	const int traceSkill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
	if (rise::slayerserver::IsSlayerSkill(traceSkill))
	{
		LogAdd(LOG_GREEN, "[SlayerQA] CGSkillAttackRecv source=%d sourceType=%d sourceConnected=%d sourceLive=%d class=%d dbClass=%d target=%d targetType=%d targetConnected=%d targetLive=%d skill=%d",
			aIndex, lpObj->Type, lpObj->Connected, lpObj->Live, lpObj->Class,
			lpObj->DBClass, bIndex, lpTarget->Type, lpTarget->Connected,
			lpTarget->Live, traceSkill);
	}
#endif
	if (gObjIsConnectedGS(bIndex) == 0)
	{
#ifdef RISE_SLAYER_SERVER_QA
		if (rise::slayerserver::IsSlayerSkill(traceSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject target-not-connected skill=%d target=%d", traceSkill, bIndex);
#endif
		return;
	}
	if (lpTarget->Live == 0)
	{
#ifdef RISE_SLAYER_SERVER_QA
		if (rise::slayerserver::IsSlayerSkill(traceSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject target-not-live skill=%d target=%d", traceSkill, bIndex);
#endif
		return;
	}
	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0 || gMap[lpTarget->Map].CheckAttr(lpTarget->X, lpTarget->Y, 1) != 0)
	{
#ifdef RISE_SLAYER_SERVER_QA
		if (rise::slayerserver::IsSlayerSkill(traceSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject safe-zone skill=%d source=%d target=%d", traceSkill, aIndex, bIndex);
#endif
		return;
	}
	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0 || gDuel.GetDuelArenaBySpectator(bIndex) != 0)
	{
#ifdef RISE_SLAYER_SERVER_QA
		if (rise::slayerserver::IsSlayerSkill(traceSkill))
			LogAdd(LOG_RED, "[SlayerQA] reject duel-spectator skill=%d source=%d target=%d", traceSkill, aIndex, bIndex);
#endif
		return;
	}
	int skill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
	CSkill* lpSkill = 0;
	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);
		if (lpSkill == 0)
		{
#ifdef RISE_SLAYER_SERVER_QA
			if (rise::slayerserver::IsSlayerSkill(skill))
				LogAdd(LOG_RED, "[SlayerQA] reject skill-not-learned skill=%d source=%d", skill, aIndex);
#endif
			return;
		}
#ifdef RISE_SLAYER_SERVER_QA
		if (rise::slayerserver::IsSlayerSkill(skill))
			LogAdd(LOG_GREEN, "[SlayerQA] resolved skill=%d index=%d mskill=%d level=%d mana=%d bp=%d", skill, lpSkill->m_index, lpSkill->m_skill, lpSkill->m_level, lpObj->Mana, lpObj->BP);
#endif
		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
#ifdef RISE_SLAYER_SERVER_QA
			if (rise::slayerserver::IsSlayerSkill(skill))
				LogAdd(LOG_RED, "[SlayerQA] reject skill-delay skill=%d source=%d", skill, aIndex);
#endif
			return;
		}
		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
		{
#ifdef RISE_SLAYER_SERVER_QA
			if (rise::slayerserver::IsSlayerSkill(skill))
				LogAdd(LOG_RED, "[SlayerQA] reject kill-point skill=%d killCount=%d", skill, lpObj->KillCount);
#endif
			return;
		}
		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
		{
#ifdef RISE_SLAYER_SERVER_QA
			if (rise::slayerserver::IsSlayerSkill(skill))
				LogAdd(LOG_RED, "[SlayerQA] reject guild-status skill=%d guildStatus=%d", skill, lpObj->GuildStatus);
#endif
			return;
		}
		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
#ifdef RISE_SLAYER_SERVER_QA
			if (rise::slayerserver::IsSlayerSkill(skill))
				LogAdd(LOG_RED, "[SlayerQA] reject class-gate skill=%d class=%d dbClass=%d", skill, lpObj->Class, lpObj->DBClass);
#endif
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];
		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}
	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;
#ifdef RISE_SLAYER_SERVER_QA
	if (rise::slayerserver::IsSlayerSkill(skill))
		LogAdd(LOG_GREEN, "[SlayerQA] dispatch skill=%d source=%d target=%d", skill, aIndex, bIndex);
#endif
	this->UseAttackSkill(aIndex, ((skill == SKILL_NOVA_START || skill == MASTER_SKILL_ADD_NOVA_START_IMPROVED) ? skill : bIndex), lpSkill);
}
void CSkillManager::CGDurationSkillAttackRecv(PMSG_DURATION_SKILL_ATTACK_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Live == 0)
	{
		return;
	}
	if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
	{
		return;
	}
	if (gDuel.GetDuelArenaBySpectator(aIndex) != 0)
	{
		return;
	}
	int skill = MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]);
	CSkill* lpSkill = 0;
	if (lpObj->Type == OBJECT_USER)
	{
		lpSkill = this->GetSkill(lpObj, skill);
		if (lpSkill == 0)
		{
			return;
		}
		if (this->CheckSkillDelay(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
		if (this->CheckSkillRequireKillPoint(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
		if (this->CheckSkillRequireGuildStatus(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
		if (this->CheckSkillRequireClass(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
	}
	else
	{
		lpSkill = &lpObj->Skill[skill];
		if (lpSkill->IsSkill() == 0)
		{
			return;
		}
	}
	lpObj->MultiSkillIndex = 0;
	lpObj->MultiSkillCount = 0;
	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	this->UseDurationSkillAttack(lpObj->Index, bIndex, lpSkill, lpMsg->x, lpMsg->y, lpMsg->dir, lpMsg->angle);
}
void CSkillManager::CGSkillCancelRecv(PMSG_SKILL_CANCEL_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}
	gEffectManager.DelEffect(lpObj, this->GetSkillEffect(MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1])));
}
void CSkillManager::CGRageFighterSkillAttackRecv(PMSG_RAGE_FIGHTER_SKILL_ATTACK_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}
	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	CSkill* lpSkill = this->GetSkill(lpObj, MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]));
	if (lpSkill == 0)
	{
		return;
	}
	if (lpSkill->m_skill != SKILL_UPPER_BEAST && lpSkill->m_skill != SKILL_CHAIN_DRIVER && lpSkill->m_skill != SKILL_DARK_SIDE)
	{
		return;
	}
	lpObj->RageFighterSkillIndex = lpSkill->m_index;
	lpObj->RageFighterSkillCount = 0;
	if (lpSkill->m_skill == SKILL_DARK_SIDE)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, lpObj->RageFighterSkillTarget, 1);
	}
	else if (OBJECT_RANGE(bIndex) != 0)
	{
		this->GCRageFighterSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);
	}
}
void CSkillManager::CGSkillDarkSideRecv(PMSG_SKILL_DARK_SIDE_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGS(aIndex) == 0)
	{
		return;
	}
	if (lpObj->Live == 0 || lpObj->Teleport != 0)
	{
		return;
	}
	int bIndex = MAKE_NUMBERW(lpMsg->index[0], lpMsg->index[1]);
	CSkill* lpSkill = this->GetSkill(lpObj, MAKE_NUMBERW(lpMsg->skill[0], lpMsg->skill[1]));
	if (lpSkill == 0)
	{
		return;
	}
	if (lpSkill->m_skill != SKILL_DARK_SIDE)
	{
		return;
	}
	PMSG_SKILL_DARK_SIDE_SEND pMsg;
	pMsg.header.set(0x4B, sizeof(pMsg));
	pMsg.skill = lpSkill->m_index;
	pMsg.index[0] = 10000;
	pMsg.index[1] = 10000;
	pMsg.index[2] = 10000;
	pMsg.index[3] = 10000;
	pMsg.index[4] = 10000;
	this->SkillDarkSideGetTargetIndex(aIndex, ((bIndex == 0) ? -1 : bIndex), lpSkill, pMsg.index);
	DataSend(aIndex, (BYTE*)&pMsg, pMsg.header.size);
}
void CSkillManager::CGSkillTeleportAllyRecv(PMSG_SKILL_TELEPORT_ALLY_RECV* lpMsg, int aIndex)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (gObjIsConnectedGP(aIndex) == 0)
	{
		return;
	}
	if (OBJECT_RANGE(lpMsg->index) == 0)
	{
		return;
	}
	if (gObj[lpMsg->index].Connected != OBJECT_ONLINE || gObj[lpMsg->index].CloseCount > 0)
	{
		lpMsg->index = aIndex;
	}
	LPOBJ lpTarget = &gObj[lpMsg->index];
	if (OBJECT_RANGE(lpObj->PartyNumber) == 0 || OBJECT_RANGE(lpTarget->PartyNumber) == 0)
	{
		return;
	}
	if (lpObj->PartyNumber != lpTarget->PartyNumber)
	{
		return;
	}
	if (gEffectManager.CheckStunEffect(lpObj) != 0)
	{
		return;
	}
	if (lpObj->Map != lpTarget->Map)
	{
		return;
	}
	if (gObjCheckTeleportArea(lpMsg->index, lpMsg->x, lpMsg->y) == 0)
	{
		return;
	}
#if(GAMESERVER_TYPE==1)
	if (lpObj->Map == MAP_CASTLE_SIEGE && gCastleSiege.CheckTeleportMagicAxisY(lpObj->Y, lpMsg->x, lpMsg->y) == 0)
	{
		lpMsg->y = (BYTE)lpObj->Y;
	}
#endif
	CSkill* lpSkill = gSkillManager.GetSkill(lpObj, SKILL_TELEPORT_ALLY);
	if (lpSkill != 0)
	{
		if (IT_MAP_RANGE(lpObj->Map) != 0 && gIllusionTemple.GetState(GET_IT_LEVEL(lpObj->Map)) != IT_STATE_START)
		{
			lpMsg->x = (BYTE)lpObj->X;
			lpMsg->y = (BYTE)lpObj->Y;
		}
		if (DG_MAP_RANGE(lpObj->Map) != 0 && gDoubleGoer.GetState() != DG_STATE_START)
		{
			lpMsg->x = (BYTE)lpObj->X;
			lpMsg->y = (BYTE)lpObj->Y;
		}
		if (gSkillManager.CheckSkillMana(lpObj, lpSkill->m_index) == 0 || gSkillManager.CheckSkillBP(lpObj, lpSkill->m_index) == 0)
		{
			return;
		}
		gSkillManager.GCSkillAttackSend(lpObj, lpSkill->m_index, lpMsg->index, 1);
		gObjTeleportMagicUse(lpMsg->index, lpMsg->x, lpMsg->y);
		lpObj->Mana -= (gSkillManager.GetSkillMana(lpSkill->m_index) * lpObj->MPConsumptionRate) / 100;
		lpObj->BP -= (gSkillManager.GetSkillBP(lpSkill->m_index) * lpObj->BPConsumptionRate) / 100;
		GCManaSend(lpObj->Index, 0xFF, (int)lpObj->Mana, lpObj->BP);
	}
}
void CSkillManager::GCSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type)
{
	PMSG_SKILL_ATTACK_SEND pMsg;
	pMsg.header.set(0x19, sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.target[0] = SET_NUMBERHB(aIndex) | (type * 0x80);
	pMsg.target[1] = SET_NUMBERLB(aIndex);
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}
void CSkillManager::GCSkillCancelSend(LPOBJ lpObj, int skill)
{
	PMSG_SKILL_CANCEL_SEND pMsg;
	pMsg.header.set(0x1B, sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}
void CSkillManager::GCDurationSkillAttackSend(LPOBJ lpObj, int skill, BYTE x, BYTE y, BYTE dir)
{
	PMSG_DURATION_SKILL_ATTACK_SEND pMsg;
	pMsg.header.set(0x1E, sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.x = x;
	pMsg.y = y;
	pMsg.dir = dir;
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}
void CSkillManager::GCRageFighterSkillAttackSend(LPOBJ lpObj, int skill, int aIndex, BYTE type)
{
	PMSG_RAGE_FIGHTER_SKILL_ATTACK_SEND pMsg;
	pMsg.header.set(0x4A, sizeof(pMsg));
	pMsg.skill[0] = SET_NUMBERHB(skill);
	pMsg.skill[1] = SET_NUMBERLB(skill);
	pMsg.index[0] = SET_NUMBERHB(lpObj->Index);
	pMsg.index[1] = SET_NUMBERLB(lpObj->Index);
	pMsg.target[0] = SET_NUMBERHB(aIndex) | (type * 0x80);
	pMsg.target[1] = SET_NUMBERLB(aIndex);
	if (lpObj->Type == OBJECT_USER)
	{
		DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);
	}
	MsgSendV2(lpObj, (BYTE*)&pMsg, pMsg.header.size);
}
void CSkillManager::GCSkillAddSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type)
{
	BYTE send[256];
	PMSG_SKILL_LIST_SEND pMsg;
	pMsg.header.set(0xF3, 0x11, 0);
	int size = sizeof(pMsg);
	pMsg.count = 0xFE;
	pMsg.type = type;
	PMSG_SKILL_LIST info;
	info.slot = slot;
	info.skill = skill;
	info.level = (level << 3) | ((skill / 255) & 7);
	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}
void CSkillManager::GCSkillDelSend(int aIndex, BYTE slot, int skill, BYTE level, BYTE type)
{
	BYTE send[256];
	PMSG_SKILL_LIST_SEND pMsg;
	pMsg.header.set(0xF3, 0x11, 0);
	int size = sizeof(pMsg);
	pMsg.count = 0xFF;
	pMsg.type = type;
	PMSG_SKILL_LIST info;
	info.slot = slot;
	info.skill = skill;
	info.level = (level << 3) | ((skill / 255) & 7);
	memcpy(&send[size], &info, sizeof(info));
	size += sizeof(info);
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(aIndex, send, size);
}
void CSkillManager::GCSkillListSend(LPOBJ lpObj, BYTE type)
{
	BYTE send[1024];
	PMSG_SKILL_LIST_SEND pMsg;
	pMsg.header.set(0xF3, 0x11, 0);
	int size = sizeof(pMsg);
	pMsg.count = 0;
	pMsg.type = type;
	PMSG_SKILL_LIST info;
	for (int n = 0; n < MAX_SKILL_LIST; n++)
	{
		if (lpObj->Skill[n].IsSkill() == 0)
		{
			continue;
		}
		info.slot = n;
		info.skill = lpObj->Skill[n].m_index;
		info.level = (lpObj->Skill[n].m_level << 3) | ((lpObj->Skill[n].m_index / 255) & 7);
		memcpy(&send[size], &info, sizeof(info));
		size += sizeof(info);
		pMsg.count++;
	}
	pMsg.header.size = size;
	memcpy(send, &pMsg, sizeof(pMsg));
	DataSend(lpObj->Index, send, size);
}

bool CSkillManager::SkillSlayerSwordInertia(int aIndex, int bIndex,
	CSkill* lpSkill)
{
	// S21 creates three boomerang visuals but accepts a target only once per
	// cast.  Keep damage server-authoritative and broadcast one native packet.
	if (this->BasicSkillAttack(aIndex, bIndex, lpSkill, false) == 0)
		return false;
	this->GCSkillAttackSend(&gObj[aIndex], lpSkill->m_index, bIndex, 1);
	return true;
}

bool CSkillManager::SkillSlayerBatFlock(int aIndex, int bIndex,
	CSkill* lpSkill)
{
	if (OBJECT_RANGE(aIndex) == 0 || OBJECT_RANGE(bIndex) == 0 || !lpSkill)
		return false;
	LPOBJ caster = &gObj[aIndex];
	if (caster->Type != OBJECT_USER || caster->Live == 0 ||
		caster->State != OBJECT_PLAYING ||
		!rise::slayerserver::IsSlayerDbClass(caster->DBClass))
		return false;
	if (gMap[caster->Map].CheckAttr(caster->X, caster->Y, 1) != 0 ||
		gDuel.GetDuelArenaBySpectator(aIndex) != 0)
		return false;

	// S21's separate 0x125 visual event carries at most 10 distinct target
	// keys. Its GS collision shape is not available in the client dump. Use
	// the imported Bat Flock range and native 5.2 viewport/target checks as
	// a provisional server-authoritative multi-enemy selection adapter.
	auto Eligible = [&](int index, int type) -> bool
	{
		return OBJECT_RANGE(index) != 0 && index != aIndex &&
			gObjIsConnectedGS(index) != 0 &&
			gObj[index].Live != 0 && gObj[index].State == OBJECT_PLAYING &&
			gObj[index].Map == caster->Map &&
			gMap[caster->Map].CheckAttr(gObj[index].X,
				gObj[index].Y, 1) == 0 &&
			gDuel.GetDuelArenaBySpectator(index) == 0 &&
			this->CheckSkillTarget(caster, index, -1, type) != 0 &&
			this->CheckSkillRange(lpSkill->m_index, caster->X, caster->Y,
				gObj[index].X, gObj[index].Y) != 0;
	};
	if (!Eligible(bIndex, gObj[bIndex].Type))
		return false;

	int candidates[rise::slayer::kBatFanoutMaxTargets] = { bIndex };
	int candidateCount = 1;
	for (int n = 0; n < MAX_VIEWPORT &&
		candidateCount < rise::slayer::kBatFanoutMaxTargets; ++n)
	{
		if (caster->VpPlayer2[n].state == VIEWPORT_NONE)
			continue;
		const int index = caster->VpPlayer2[n].index;
		if (!Eligible(index, caster->VpPlayer2[n].type))
			continue;
		bool duplicate = false;
		for (int t = 0; t < candidateCount; ++t)
			duplicate |= candidates[t] == index;
		if (!duplicate)
			candidates[candidateCount++] = index;
	}

	const int dotDamage = rise::slayerserver::BatFlockDotDamage(
		caster->Energy + caster->AddEnergy);
	int affected[rise::slayer::kBatFanoutMaxTargets] = {};
	int affectedCount = 0;
	for (int n = 0; n < candidateCount; ++n)
	{
		const int index = candidates[n];
		if (!Eligible(index, gObj[index].Type))
			continue;
		if (!this->BasicSkillAttack(aIndex, index, lpSkill, false))
		{
			if (index == bIndex)
				return false;
			continue;
		}
		this->BasicSkillAttack(aIndex, index, lpSkill, false);
		gEffectManager.AddEffect(&gObj[index], 0, EFFECT_SLAYER_BAT_FLOCK,
			5, aIndex, 1, SET_NUMBERHW(dotDamage), SET_NUMBERLW(dotDamage));
		affected[affectedCount++] = index;
	}
	if (affectedCount == 0)
		return false;
	this->GCSkillAttackSend(caster, lpSkill->m_index, bIndex, 1);
	// Damage and list-bearing visuals share the exact affected set; no local
	// client timer or independently guessed visual target list is involved.
	rise::slayer::BatFanoutWire fanout = {};
	fanout.type = 0xC1;
	fanout.size = static_cast<BYTE>(sizeof(fanout));
	fanout.head = rise::slayer::kBatFanoutHead;
	fanout.sub = rise::slayer::kBatFanoutSub;
	fanout.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	fanout.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	fanout.caster[0] = SET_NUMBERHB(aIndex);
	fanout.caster[1] = SET_NUMBERLB(aIndex);
	fanout.count = static_cast<BYTE>(affectedCount);
	for (int n = 0; n < affectedCount; ++n)
	{
		fanout.target[n][0] = SET_NUMBERHB(affected[n]);
		fanout.target[n][1] = SET_NUMBERLB(affected[n]);
	}
	DataSend(aIndex, reinterpret_cast<BYTE*>(&fanout), fanout.size);
	MsgSendV2(caster, reinterpret_cast<BYTE*>(&fanout), fanout.size);
	return true;
}

bool CSkillManager::SkillSlayerPierceAttack(int aIndex, int bIndex,
	CSkill* lpSkill)
{
	if (OBJECT_RANGE(aIndex) == 0 || OBJECT_RANGE(bIndex) == 0 || !lpSkill)
		return false;
	LPOBJ caster = &gObj[aIndex];
	if (caster->Type != OBJECT_USER || caster->Live == 0 ||
		caster->State != OBJECT_PLAYING ||
		!rise::slayerserver::IsSlayerDbClass(caster->DBClass))
		return false;
	if (gMap[caster->Map].CheckAttr(caster->X, caster->Y, 1) != 0 ||
		gDuel.GetDuelArenaBySpectator(aIndex) != 0)
		return false;

	// S21 SkillAOETargetting.xml includes 294, and the separate 0x126
	// receive path caps its list at ten keys. The S21 GS collider is not
	// available; this is a native 5.2 range/viewport selection adapter.
	auto Eligible = [&](int index, int type) -> bool
	{
		return OBJECT_RANGE(index) != 0 && index != aIndex &&
			gObjIsConnectedGS(index) != 0 &&
			gObj[index].Live != 0 && gObj[index].State == OBJECT_PLAYING &&
			gObj[index].Map == caster->Map &&
			gMap[caster->Map].CheckAttr(gObj[index].X,
				gObj[index].Y, 1) == 0 &&
			gDuel.GetDuelArenaBySpectator(index) == 0 &&
			this->CheckSkillTarget(caster, index, -1, type) != 0 &&
			this->CheckSkillRange(lpSkill->m_index, caster->X, caster->Y,
				gObj[index].X, gObj[index].Y) != 0;
	};
	if (!Eligible(bIndex, gObj[bIndex].Type))
		return false;
	int candidates[rise::slayer::kPierceFanoutMaxTargets] = { bIndex };
	int candidateCount = 1;
	for (int n = 0; n < MAX_VIEWPORT &&
		candidateCount < rise::slayer::kPierceFanoutMaxTargets; ++n)
	{
		if (caster->VpPlayer2[n].state == VIEWPORT_NONE)
			continue;
		const int index = caster->VpPlayer2[n].index;
		if (!Eligible(index, caster->VpPlayer2[n].type))
			continue;
		bool duplicate = false;
		for (int t = 0; t < candidateCount; ++t)
			duplicate |= candidates[t] == index;
		if (!duplicate)
			candidates[candidateCount++] = index;
	}

	// S21 0x689 sends one outbound target request from the local caster per
	// resolved lane. Keep the accepted cast and its victim list on GS; do
	// not apply every strike at t=0 before any authored lane has launched.
	PendingSlayerPierce pending = {};
	pending.connectedAt = caster->ConnectTickCount;
	pending.openedAt = GetTickCount();
	pending.map = caster->Map;
	pending.castX = caster->X;
	pending.castY = caster->Y;
	pending.count = candidateCount;
	for (int n = 0; n < candidateCount; ++n)
		pending.target[n] = candidates[n];
	{
		std::lock_guard<std::mutex> guard(gPendingSlayerPierceMutex);
		std::deque<PendingSlayerPierce>& casts = gPendingSlayerPierce[aIndex];
		PruneSlayerPierceCasts(casts, caster->ConnectTickCount,
			pending.openedAt);
		// Do not overwrite an earlier in-flight visual lane. Refuse a new
		// cast once the bounded 5.2 adapter queue is full.
		if (casts.size() >= kSlayerPierceMaxPendingPerCaster)
			return false;
		bool serialFree = false;
		for (int attempt = 0; attempt < 256; ++attempt)
		{
			pending.serial = ++gSlayerPierceSerial;
			serialFree = true;
			for (const PendingSlayerPierce& prior : casts)
				if (prior.serial == pending.serial)
					serialFree = false;
			if (serialFree)
				break;
		}
		if (!serialFree)
			return false;
		casts.push_back(pending);
	}
	this->GCSkillAttackSend(caster, lpSkill->m_index, bIndex, 1);
	// Supplemental lanes are bounded by this server-selected, serialed list.
	rise::slayer::PierceFanoutWire fanout = {};
	fanout.type = 0xC1;
	fanout.size = static_cast<BYTE>(sizeof(fanout));
	fanout.head = rise::slayer::kPierceFanoutHead;
	fanout.sub = rise::slayer::kPierceFanoutSub;
	fanout.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	fanout.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	fanout.caster[0] = SET_NUMBERHB(aIndex);
	fanout.caster[1] = SET_NUMBERLB(aIndex);
	fanout.serial = pending.serial;
	fanout.count = static_cast<BYTE>(candidateCount);
	for (int n = 0; n < candidateCount; ++n)
	{
		fanout.target[n][0] = SET_NUMBERHB(candidates[n]);
		fanout.target[n][1] = SET_NUMBERLB(candidates[n]);
	}
	DataSend(aIndex, reinterpret_cast<BYTE*>(&fanout), fanout.size);
	MsgSendV2(&gObj[aIndex], reinterpret_cast<BYTE*>(&fanout), fanout.size);
	return true;
}

void CSkillManager::CGSlayerPierceLaneRecv(BYTE* lpMsg, int size,
	int aIndex)
{
	if (!lpMsg || size != sizeof(rise::slayer::PierceLaneRequestWire) ||
		OBJECT_RANGE(aIndex) == 0 || gObjIsConnectedGS(aIndex) == 0)
		return;
	const rise::slayer::PierceLaneRequestWire& lane =
		*reinterpret_cast<const rise::slayer::PierceLaneRequestWire*>(lpMsg);
	const int skillId = MAKE_NUMBERW(lane.skill[0], lane.skill[1]);
	const int targetIndex = MAKE_NUMBERW(lane.target[0], lane.target[1]);
	if (lane.type != 0xC1 || lane.size != size ||
		lane.head != rise::slayer::kPierceFanoutHead ||
		lane.sub != rise::slayer::kPierceLaneRequestSub ||
		skillId != rise::slayerserver::kPierceAttack ||
		lane.direction < 1 || lane.direction > 50 ||
		OBJECT_RANGE(targetIndex) == 0 || targetIndex == aIndex ||
		gObjIsConnectedGS(targetIndex) == 0)
		return;
	LPOBJ caster = &gObj[aIndex];
	LPOBJ target = &gObj[targetIndex];
	if (caster->Type != OBJECT_USER || caster->Live == 0 ||
		caster->State != OBJECT_PLAYING ||
		!rise::slayerserver::IsSlayerDbClass(caster->DBClass) ||
		target->Live == 0 || target->State != OBJECT_PLAYING ||
		caster->Map != target->Map ||
		gMap[caster->Map].CheckAttr(caster->X, caster->Y, 1) != 0 ||
		gMap[target->Map].CheckAttr(target->X, target->Y, 1) != 0 ||
		gDuel.GetDuelArenaBySpectator(aIndex) != 0 ||
		gDuel.GetDuelArenaBySpectator(targetIndex) != 0)
		return;
	CSkill* skill = this->GetSkill(caster, skillId);
	if (!skill || !this->CheckSkillRequireClass(caster, skillId) ||
		this->CheckSkillTarget(caster, targetIndex, -1, target->Type) == 0)
		return;
	PendingSlayerPierce pending = {};
	int ordinal = -1;
	{
		std::lock_guard<std::mutex> guard(gPendingSlayerPierceMutex);
		auto it = gPendingSlayerPierce.find(aIndex);
		if (it == gPendingSlayerPierce.end())
			return;
		PruneSlayerPierceCasts(it->second, caster->ConnectTickCount,
			GetTickCount());
		if (it->second.empty())
		{
			gPendingSlayerPierce.erase(it);
			return;
		}
		const PendingSlayerPierce* matched = 0;
		for (const PendingSlayerPierce& cast : it->second)
			if (cast.serial == lane.serial)
			{
				matched = &cast;
				break;
			}
		if (!matched)
			return;
		pending = *matched;
		if (pending.serial != lane.serial ||
			pending.connectedAt != caster->ConnectTickCount ||
			pending.map != caster->Map ||
			GetTickCount() - pending.openedAt > kSlayerPierceLaneWindowMs)
			return;
		for (int n = 0; n < pending.count; ++n)
			if (pending.target[n] == targetIndex && !pending.consumed[n])
			{
				ordinal = n;
				break;
			}
	}
	if (ordinal < 0 ||
		abs(target->X - static_cast<int>(lane.tileX)) > 1 ||
		abs(target->Y - static_cast<int>(lane.tileY)) > 1 ||
		this->CheckSkillRange(skillId, pending.castX, pending.castY,
			target->X, target->Y) == 0)
		return;
	{
		std::lock_guard<std::mutex> guard(gPendingSlayerPierceMutex);
		auto it = gPendingSlayerPierce.find(aIndex);
		if (it == gPendingSlayerPierce.end())
			return;
		PendingSlayerPierce* matched = 0;
		for (PendingSlayerPierce& cast : it->second)
			if (cast.serial == lane.serial)
			{
				matched = &cast;
				break;
			}
		if (!matched ||
			matched->openedAt != pending.openedAt ||
			matched->castX != pending.castX ||
			matched->castY != pending.castY ||
			matched->connectedAt != caster->ConnectTickCount ||
			matched->map != caster->Map ||
			GetTickCount() - matched->openedAt > kSlayerPierceLaneWindowMs ||
			matched->target[ordinal] != targetIndex ||
			matched->consumed[ordinal])
			return;
		matched->consumed[ordinal] = true;
		PruneSlayerPierceCasts(it->second, caster->ConnectTickCount,
			GetTickCount());
		if (it->second.empty())
			gPendingSlayerPierce.erase(it);
	}
	// A packet cannot create targets, refresh a cast or strike the same lane
	// twice. GS observes the Bat mark at the moment the lane lands.
	const bool batMarked = gEffectManager.CheckEffect(target,
		EFFECT_SLAYER_BAT_FLOCK) != 0;
	const int strikes = batMarked ? 4 : 2;
	for (int n = 0; n < strikes && target->Live != 0; ++n)
	{
		// BasicSkillAttack rechecks the *current* caster tile, even though
		// this lane was authorized against its saved cast origin above. Keep
		// the same 5.2 damage call without invalidating delayed S21 lanes.
		gAttack.Attack(caster, target, skill, 1, 0, 0, 0, false);
	}
}

bool CSkillManager::SkillSlayerDetection(int aIndex, int bIndex,
	CSkill* lpSkill)
{
	if (OBJECT_RANGE(aIndex) == 0 || OBJECT_RANGE(bIndex) == 0 ||
		bIndex != aIndex)
		return false;
	// BuffEffectManager row 316: Detection remains active for one minute.
	// Keep the duration on GameServer so the client cannot extend or recreate
	// the result from a local timer.
	if (!gEffectManager.AddEffect(&gObj[aIndex], 0,
		rise::slayerserver::kDetectionEffect,
		rise::slayerserver::DetectionDurationSeconds(), 0, 0, 0, 0))
		return false;
	this->GCSkillAttackSend(&gObj[aIndex], lpSkill->m_index, aIndex, 1);
	// Only the buff owner receives the local minimap reveal start signal;
	// neighboring players still receive the authored cast effect via 0x19.
	rise::slayer::DetectionWire reveal = {};
	reveal.type = 0xC1;
	reveal.size = static_cast<BYTE>(sizeof(reveal));
	reveal.head = rise::slayer::kDetectionHead;
	reveal.sub = rise::slayer::kDetectionSub;
	reveal.skill[0] = SET_NUMBERHB(lpSkill->m_index);
	reveal.skill[1] = SET_NUMBERLB(lpSkill->m_index);
	reveal.caster[0] = SET_NUMBERHB(aIndex);
	reveal.caster[1] = SET_NUMBERLB(aIndex);
	reveal.map[0] = SET_NUMBERHB(gObj[aIndex].Map);
	reveal.map[1] = SET_NUMBERLB(gObj[aIndex].Map);
	const int duration = rise::slayerserver::DetectionDurationSeconds();
	reveal.durationSeconds[0] = SET_NUMBERHB(duration);
	reveal.durationSeconds[1] = SET_NUMBERLB(duration);
	DataSend(aIndex, reinterpret_cast<BYTE*>(&reveal), reveal.size);
	return true;
}

bool CSkillManager::SkillSlayerDemolish(int aIndex, int bIndex,
	CSkill* lpSkill)
{
	LPOBJ lpObj = &gObj[aIndex];
	if (lpObj->Type != OBJECT_USER || lpObj->Live == 0 ||
		OBJECT_RANGE(bIndex) == 0 || bIndex != aIndex)
	{
		return 0;
	}

	// MasterSkillCalc_3rd.lua (SlayerDemolish_MasterLevel1/2_Calc):
	//   ((Strength / 8) + (Dexterity / 28) + 120) * 0.03
	//   + SkillTreeValue, duration = 60 seconds.
	// The legacy 5.2 tree has no Slayer mastery-value slot yet, so the
	// authoritative imported base uses SkillTreeValue=0 until that ABI is
	// migrated.  Do not silently borrow a different class' mastery node.
	const double raw = ((lpObj->Strength + lpObj->AddStrength) / 8.0) +
		((lpObj->Dexterity + lpObj->AddDexterity) / 28.0) + 120.0;
	const int value = max(0, static_cast<int>(raw * 0.03));
	const int duration = rise::slayerserver::DemolishDurationSeconds();

	auto ApplyTo = [&](LPOBJ target) -> bool
	{
		// AddEffect can reject a weaker refresh or a full effect pool. Do not
		// announce a cast unless the caster's authoritative buff is accepted.
		// AddEffect already sends the individual 5.2 effect-state packet.
		if (!gEffectManager.AddEffect(target, 0, EFFECT_SLAYER_DEMOLISH,
			duration, value, 0, 0, 0))
			return false;
		return true;
	};

	if (OBJECT_RANGE(lpObj->PartyNumber) == 0)
	{
		if (!ApplyTo(lpObj))
			return false;
		this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);
		return true;
	}
	// Party tables are not required to contain the casting slot.  Apply to the
	// caster explicitly, then fan out to nearby party members without a
	// duplicate self packet.
	if (!ApplyTo(lpObj))
		return false;
	// A party Demolish is one cast graph at the caster, not one identical
	// caster-root graph for every party recipient. Recipient buff states are
	// delivered by AddEffect above/below, independently of the cast packet.
	this->GCSkillAttackSend(lpObj, lpSkill->m_index, aIndex, 1);

	// SkillSettings.ini sets PartySkillRange=9 for Slayer party buffs.
	// Use that S21 value directly because the legacy catalog has no Radio
	// entry for the overlay row (CheckSkillRadio would reject Radio=0).
	PARTY_INFO* lpParty = &gParty.m_PartyInfo[lpObj->PartyNumber];
	for (int n = 0; n < MAX_PARTY_USER; ++n)
	{
		const int index = lpParty->Index[n];
		if (OBJECT_RANGE(index) == 0 || index == lpObj->Index || gObj[index].Live == 0 ||
			gObj[index].State != OBJECT_PLAYING ||
			gObj[index].Map != lpObj->Map)
		{
			continue;
		}
		const int dx = gObj[index].X - lpObj->X;
		const int dy = gObj[index].Y - lpObj->Y;
		if ((dx * dx + dy * dy) > (9 * 9))
		{
			continue;
		}
		ApplyTo(&gObj[index]);
	}
	return 1;
}
