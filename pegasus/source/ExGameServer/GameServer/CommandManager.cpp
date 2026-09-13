#include "stdafx.h"
#include "CommandManager.h"
#include "BloodCastle.h"
#include "BonusManager.h"
#include "CashShop.h"
#include "ChaosCastle.h"
#include "Command.h"
#include "CustomAttack.h"
#include "CustomPick.h"
#include "CustomQuest.h"
#include "CustomTop.h"
#include "CustomStore.h"
#include "DevilSquare.h"
#include "DSProtocol.h"
#include "EventHideAndSeek.h"
#include "EventKillAll.h"
#include "EventRunAndCatch.h"
#include "Filter.h"
#include "FilterRename.h"
#include "GameMain.h"
#include "GameMaster.h"
#include "Guild.h"
#include "GuildClass.h"
#include "InvasionManager.h"
#include "IllusionTemple.h"
#include "ItemManager.h"
#include "ItemBagManager.h"
#include "JewelMix.h"
#include "JSProtocol.h"
#include "Log.h"
#include "MapServerManager.h"
#include "MasterSkillTree.h"
#include "Monster.h"
#include "Move.h"
#include "Notice.h"
#include "ObjectManager.h"
#include "Protocol.h"
#include "Quest.h"
#include "QuestReward.h"
#include "ResetTable.h"
#include "ServerInfo.h"
#include "Util.h"
#include "ReiDoMU.h"
#include "Warehouse.h"
#include "Viewport.h"
#include "DefaultClassInfo.h"
#include "Duel.h"
#include "EffectManager.h"
#include <cctype>
#include "ItemStack.h"
#include "readscript.h"
#include "PartySetPass.h"
#include "RISE/ResetLimiter.h"
#include "RISE/RISE_AddBuff.h"
#include "RISE/RISE_StatsInfo.h"
#include "RISE/RISE_CmdSocket.h"
#include "RISE/ChangeClass.h"
#include "RISE/CustomRankUser.h"
#include "RISE/ZzzToolKit.h"
#include "RISE/CustomJewelBank.h"
#include "RISE/RISE_MessLang.h"
#include "RISE/RISE_AutoHp.h"
#if defined(PEGASUS_ISOLATED_TEST)
#include "../../Shared/PegasusSwordBlowProtocol.h"
#include "../../Shared/PegasusHighSkillCatalogData.h"
#include "../../Shared/PegasusRuntimeQa15.h"
#include "RISE/WindSoulServerContract.h"

namespace
{
constexpr const char* kPegasusQa15BuildStamp = "reaction-sample-v4";
// Capability/build declaration, not a cryptographic executable identity.
constexpr const char* kPegasusQaServerIdentity =
    "QA15 SERVER bird-heading=target-v1 compiled=" __DATE__ " " __TIME__;
constexpr int kPegasusQaReactionExcludedMonsterClass = 275;
constexpr int kPegasusQaSpawnMonsterClass = 0;
constexpr int kPegasusQaSpawnMonsterCount = 8;

bool IsIsolatedSkillTestGM(LPOBJ lpObj)
{
	return (lpObj->Authority & 32) != 0 ||
		gGameMaster.CheckGameMasterLevel(lpObj, 1) != 0;
}

// 0: no free slot/set failed, 1: already present, 2: newly added.
int AddIsolatedTestSkill(LPOBJ lpObj, int skill)
{
	for (int n = 0; n < MAX_SKILL_LIST; ++n)
	{
		if (lpObj->Skill[n].IsSkill() != 0 &&
			(lpObj->Skill[n].m_index == skill || lpObj->Skill[n].m_skill == skill))
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, (BYTE)n, skill,
				(BYTE)lpObj->Skill[n].m_level, 0);
			return 1;
		}
	}
	for (int n = 0; n < MAX_SKILL_LIST; ++n)
	{
		if (lpObj->Skill[n].IsSkill() == 0 && lpObj->Skill[n].Set(skill, 0) != 0)
		{
			gSkillManager.GCSkillAddSend(lpObj->Index, (BYTE)n, skill, 0, 0);
			return 2;
		}
	}
	return 0;
}

// The native client deliberately suppresses SetPlayerShock for monster class
// 275.  QA15 validates target reaction independently from damage, so selecting
// that class would produce a deterministic false failure.  Keep this selector
// isolated to QA instead of changing the native combat rule.
bool GetPegasusRuntimeQaTarget(LPOBJ lpObj, int skill, int* targetIndex)
{
	int nearestDistance = 100;
	for (int n = 0; n < MAX_VIEWPORT; ++n)
	{
		const int index = lpObj->VpPlayer2[n].index;
		if (lpObj->VpPlayer2[n].state == VIEWPORT_NONE ||
			OBJECT_RANGE(index) == 0 ||
			lpObj->VpPlayer2[n].type != OBJECT_MONSTER ||
			gObj[index].Class == kPegasusQaReactionExcludedMonsterClass)
		{
			continue;
		}
		if (gSkillManager.CheckSkillTarget(lpObj, index, -1,
			lpObj->VpPlayer2[n].type) == 0)
		{
			continue;
		}
		const int distance = gObjCalcDistance(lpObj, &gObj[index]);
		if (distance >= nearestDistance)
		{
			continue;
		}
		if (gSkillManager.CheckSkillRange(skill, lpObj->X, lpObj->Y,
			gObj[index].X, gObj[index].Y) == 0 &&
			gSkillManager.CheckSkillRadio(skill, lpObj->X, lpObj->Y,
			gObj[index].X, gObj[index].Y) == 0)
		{
			continue;
		}
		*targetIndex = index;
		nearestDistance = distance;
	}
	return nearestDistance != 100;
}

struct PegasusRuntimeQaState
{
	bool active;
	BYTE step;
	BYTE castOrdinal;
	DWORD nextTick;
};

PegasusRuntimeQaState gPegasusRuntimeQa[MAX_OBJECT] = {};

int ClearPegasusRuntimeQaBuffs(LPOBJ lpObj, std::size_t nextStep)
{
	int removed = 0;
	for (std::size_t n = 0; n < rise::pegasus::kRuntimeQaBuffEffectCount; ++n)
	{
		const int effect = rise::pegasus::kRuntimeQaBuffEffects[n];
		if (rise::pegasus::RuntimeQaRetainsBuffForStep(nextStep, effect))
		{
			LogAdd(LOG_BLACK,
				"[PegasusQA15] step=%u effect=%d result=BUFF_RETAINED",
				static_cast<unsigned>(nextStep + 1), effect);
			continue;
		}
		if (gEffectManager.CheckEffect(lpObj, effect) == 0)
		{
			LogAdd(LOG_BLACK,
				"[PegasusQA15] step=%u effect=%d result=BUFF_ABSENT",
				static_cast<unsigned>(nextStep + 1), effect);
			continue;
		}
		const bool deleted = gEffectManager.DelEffect(lpObj, effect);
		LogAdd(deleted ? LOG_BLACK : LOG_RED,
			"[PegasusQA15] step=%u effect=%d result=%s",
			static_cast<unsigned>(nextStep + 1), effect,
			deleted ? "BUFF_REMOVE_SENT" : "BUFF_REMOVE_FAILED");
		if (deleted)
			++removed;
	}
	return removed;
}

void StopPegasusRuntimeQa(int index)
{
	if (OBJECT_RANGE(index) != 0)
		gPegasusRuntimeQa[index] = PegasusRuntimeQaState();
}

void ProcessPegasusRuntimeQa(LPOBJ lpObj)
{
	PegasusRuntimeQaState& state = gPegasusRuntimeQa[lpObj->Index];
	if (!state.active)
		return;
	if (!IsIsolatedSkillTestGM(lpObj) || lpObj->Live == 0)
	{
		ClearPegasusRuntimeQaBuffs(lpObj, rise::pegasus::kRuntimeQaSkillCount);
		StopPegasusRuntimeQa(lpObj->Index);
		return;
	}
	const DWORD now = GetTickCount();
	if (static_cast<int>(now - state.nextTick) < 0)
		return;
	if (state.step >= rise::pegasus::kRuntimeQaSkillCount)
	{
		const int removed = ClearPegasusRuntimeQaBuffs(
			lpObj, rise::pegasus::kRuntimeQaSkillCount);
		state.active = false;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)"QA15 sequence complete. Review video and Pegasus QA logs.");
		LogAdd(LOG_BLACK,
			"[PegasusQA15] COMPLETE account=%s character=%s buffs_removed=%d",
			lpObj->Account, lpObj->Name, removed);
		return;
	}

	const std::size_t step = state.step;
	const int skill = rise::pegasus::RuntimeQaSkillId(step);
	const char* name = rise::pegasus::RuntimeQaSkillName(step);
	const bool selfBuff = rise::pegasus::RuntimeQaSkillIsSelfBuff(step);
	const int removedBuffs = ClearPegasusRuntimeQaBuffs(lpObj, step);
	if (removedBuffs != 0)
	{
		LogAdd(LOG_BLACK,
			"[PegasusQA15] step=%u result=BUFF_CLEANUP removed=%d",
			static_cast<unsigned>(step + 1), removedBuffs);
	}
	const int addResult = AddIsolatedTestSkill(lpObj, skill);
	int target = lpObj->Index;
	if (!selfBuff && !GetPegasusRuntimeQaTarget(lpObj, skill, &target))
	{
		char notice[160];
		sprintf_s(notice, "[QA15 %02u/15] FAIL %s (%d): no live monster in range.",
			static_cast<unsigned>(step + 1), name, skill);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, notice);
		LogAdd(LOG_RED, "[PegasusQA15] step=%u skill=%d name=%s result=NO_TARGET",
			static_cast<unsigned>(step + 1), skill, name);
	}
	else if (addResult == 0)
	{
		char notice[160];
		sprintf_s(notice, "[QA15 %02u/15] FAIL %s (%d): skill list full.",
			static_cast<unsigned>(step + 1), name, skill);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, notice);
		LogAdd(LOG_RED, "[PegasusQA15] step=%u skill=%d name=%s result=NO_SKILL_SLOT",
			static_cast<unsigned>(step + 1), skill, name);
	}
	else
	{
		// The sequence validates casts, not resource starvation accumulated
		// from earlier steps. Refill native resources immediately before each
		// real receive-path submission.
		lpObj->Mana = lpObj->MaxMana + lpObj->AddMana;
		lpObj->BP = lpObj->MaxBP + lpObj->AddBP;
		// Keep the selected isolated QA monster alive across the complete
		// sequence. Otherwise a high-damage first cast can invalidate every
		// later visual step with NO_TARGET.
		if (!selfBuff && OBJECT_RANGE(target) != 0 &&
			gObj[target].Type == OBJECT_MONSTER)
		{
			gObj[target].MaxLife = 1000000000.0f;
			gObj[target].Life = gObj[target].MaxLife;
		}
		char notice[160];
		const unsigned castCount = rise::pegasus::RuntimeQaCastCount(step);
		sprintf_s(notice, "[QA15 %02u/15] CAST %s (%d) %u/%u.",
			static_cast<unsigned>(step + 1), name, skill,
			static_cast<unsigned>(state.castOrdinal + 1), castCount);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, notice);
		LogAdd(LOG_BLACK,
			"[PegasusQA15] step=%u skill=%d name=%s target=%d target_class=%d cast=%u/%u result=SUBMIT",
			static_cast<unsigned>(step + 1), skill, name, target,
			selfBuff ? -1 : gObj[target].Class,
			static_cast<unsigned>(state.castOrdinal + 1), castCount);
		gCustomAttack.SendSkillAttack(lpObj, target, skill);
		if (state.castOrdinal + 1 < castCount)
		{
			++state.castOrdinal;
			state.nextTick = now + rise::pegasus::kRuntimeQaRepeatDelayMs;
			return;
		}
	}

	state.castOrdinal = 0;
	++state.step;
	state.nextTick = now + rise::pegasus::kRuntimeQaStepDelayMs;
}
}
#endif
CCommandManager gCommandManager;
void CCommandManager::Init()
{
	for (int n = 0; n < MAX_COMMAND; n++)
	{
		this->m_CommandInfo[n].code = -1;
		this->Add(gCommand.GetCommand(n), n + 1);
	}
}
void CCommandManager::MainProc()
{
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) == 0)
		{
#if defined(PEGASUS_ISOLATED_TEST)
			StopPegasusRuntimeQa(n);
#endif
			continue;
		}
		if (gObj[n].AutoAddPointCount > 0)
		{
			this->CommandAddPointAutoProc(&gObj[n]);
		}
		if (gObj[n].AutoResetEnable != 0)
		{
			this->CommandResetAutoProc(&gObj[n]);
		}
#if defined(PEGASUS_ISOLATED_TEST)
		ProcessPegasusRuntimeQa(&gObj[n]);
#endif
	}
}
void CCommandManager::Add(char* label, int code)
{
	for (int n = 0; n < MAX_COMMAND; n++)
	{
		if (this->m_CommandInfo[n].code != -1)
		{
			continue;
		}
		this->m_CommandInfo[n].code = code;
		strcpy_s(this->m_CommandInfo[n].label, label);
		break;
	}
}
long CCommandManager::GetNumber(char* arg, int pos)
{
	int count = 0, p = 0;
	char buffer[60] = { 0 };
	int len = strlen(arg);
	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);
	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}
	return atoi(buffer);
}
void CCommandManager::GetString(char* arg, char* out, int size, int pos)
{
	int count = 0, p = 0;
	char buffer[60] = { 0 };
	int len = strlen(arg);
	len = ((len >= sizeof(buffer)) ? (sizeof(buffer) - 1) : len);
	for (int n = 0; n < len; n++)
	{
		if (arg[n] == 0x20)
		{
			count++;
		}
		else if (count == pos)
		{
			buffer[p] = arg[n];
			p++;
		}
	}
	memcpy(out, buffer, (size - 1));
}
long CCommandManager::GetCommandCode(char* label)
{
	for (int n = 0; n < MAX_COMMAND; n++)
	{
		if (this->m_CommandInfo[n].code == -1)
		{
			continue;
		}
		if (_stricmp(label, this->m_CommandInfo[n].label) == 0)
		{
			return this->m_CommandInfo[n].code;
		}
	}
	return 0;
}
bool CCommandManager::ManagementCore(LPOBJ lpObj, char* message, int Npc)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	char command[32] = { 0 };
	memset(command, 0, sizeof(command));
	this->GetString(message, command, sizeof(command), 0);
#if defined(PEGASUS_ISOLATED_TEST)
	if (_stricmp(command, "/qa15stop") == 0)
	{
		const int removed = ClearPegasusRuntimeQaBuffs(
			lpObj, rise::pegasus::kRuntimeQaSkillCount);
		StopPegasusRuntimeQa(lpObj->Index);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)"QA15 sequence stopped.");
		LogAdd(LOG_BLACK,
			"[PegasusQA15] STOP account=%s character=%s buffs_removed=%d",
			lpObj->Account, lpObj->Name, removed);
		return 1;
	}
	if (_stricmp(command, "/qa15") == 0)
	{
		if (!IsIsolatedSkillTestGM(lpObj))
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"QA15 requires GameMaster level 1.");
			return 0;
		}
		// Provision exactly one skill immediately before its sample in
		// ProcessPegasusRuntimeQa. Sending fifteen add-skill packets in one chat
		// handler was redundant and made a failed start indistinguishable from a
		// client/server packet burst failure.
		LogAdd(LOG_BLACK,
			"[PegasusQA15] START_REQUEST build=%s account=%s character=%s index=%d",
			kPegasusQa15BuildStamp, lpObj->Account, lpObj->Name, lpObj->Index);
		if (gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) != 0)
		{
			LogAdd(LOG_RED,
				"[PegasusQA15] PREFLIGHT result=FAIL reason=SAFE_ZONE map=%d x=%d y=%d",
				lpObj->Map, lpObj->X, lpObj->Y);
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"QA15 requires a position outside the safe zone.");
			return 0;
		}
		int qaTarget = -1;
		if (!GetPegasusRuntimeQaTarget(lpObj,
			rise::pegasus::RuntimeQaSkillId(0), &qaTarget))
		{
			LogAdd(LOG_BLACK,
				"[PegasusQA15] TARGET_CHECK result=NONE action=SPAWN monster=%d count=%d excluded=%d",
				kPegasusQaSpawnMonsterClass, kPegasusQaSpawnMonsterCount,
				kPegasusQaReactionExcludedMonsterClass);
			// One command must be enough for the owner recording. Spawn a small
			// isolated test group only when no valid target is already present.
			char spotArguments[] = "0 8";
			if (this->CommandSpot(lpObj, spotArguments) == 0)
			{
				LogAdd(LOG_RED,
					"[PegasusQA15] SPAWN_RESULT result=FAIL monster=%d count=%d",
					kPegasusQaSpawnMonsterClass, kPegasusQaSpawnMonsterCount);
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
					(char*)"QA15 could not prepare reaction-capable targets.");
				return 0;
			}
			LogAdd(LOG_BLACK,
				"[PegasusQA15] SPAWN_RESULT result=PASS monster=%d count=%d",
				kPegasusQaSpawnMonsterClass, kPegasusQaSpawnMonsterCount);
		}
		else
		{
			LogAdd(LOG_BLACK,
				"[PegasusQA15] TARGET_CHECK result=PASS target=%d target_class=%d",
				qaTarget, gObj[qaTarget].Class);
		}
		PegasusRuntimeQaState& state = gPegasusRuntimeQa[lpObj->Index];
		const int removed = ClearPegasusRuntimeQaBuffs(
			lpObj, rise::pegasus::kRuntimeQaSkillCount);
		state.active = true;
		state.step = 0;
		state.castOrdinal = 0;
		state.nextTick = GetTickCount() + rise::pegasus::kRuntimeQaInitialDelayMs;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
            (char*)"%s", kPegasusQaServerIdentity);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)"QA15 reaction-sample-v4: 15 skills; native reaction sampling enabled.");
		LogAdd(LOG_BLACK,
			"[PegasusQA15] START account=%s character=%s buffs_removed=%d",
			lpObj->Account, lpObj->Name, removed);
		return 1;
	}
	if (_stricmp(command, "/testwindsoul") == 0)
	{
		if (!IsIsolatedSkillTestGM(lpObj))
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"Wind Soul test command requires GameMaster level 1.");
			return 0;
		}
		const int result = AddIsolatedTestSkill(lpObj, rise_windsoul::kSkillId);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)(result == 2 ? "Wind Soul (737) added for isolated QA." :
			result == 1 ? "Wind Soul (737) is already available." :
			"Wind Soul could not be added: skill list is full."));
		return result != 0;
	}
	if (_stricmp(command, "/testpegasus") == 0 ||
		_stricmp(command, "/testhighskills") == 0)
	{
		if (!IsIsolatedSkillTestGM(lpObj))
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"High-skill test command requires GameMaster level 1.");
			return 0;
		}
		int added = 0, existing = 0, failed = 0;
		for (const rise::pegasus::HighSkillSeed& seed :
			rise::pegasus::kHighSkillSeeds)
		{
			const int result = AddIsolatedTestSkill(lpObj, seed.skillId);
			added += result == 2;
			existing += result == 1;
			failed += result == 0;
		}
		if (_stricmp(command, "/testhighskills") == 0)
		{
			const int result = AddIsolatedTestSkill(lpObj,
				rise_windsoul::kSkillId);
			added += result == 2;
			existing += result == 1;
			failed += result == 0;
		}
		char notice[128];
		sprintf_s(notice, "High-skill QA: added %d, existing %d, failed %d.",
			added, existing, failed);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, notice);
		return failed == 0;
	}
	// Isolated QA convenience only. This bypasses item 15:110 provisioning so
	// the owner can exercise the complete 1501 packet/runtime path immediately.
	// It is deliberately absent from every normal GameServer build.
	if (_stricmp(command, "/testswordblowoff") == 0)
	{
		// The native character loader marks a connected GM with Authority bit 32
		// but also appends an in-memory level-0 row. The level lookup additionally
		// requires an exact current IP, so a valid GM can be rejected after an IP
		// change. This isolated QA command accepts the authoritative connected-GM
		// bit as well as a configured level-1 match.
		if ((lpObj->Authority & 32) == 0 &&
			gGameMaster.CheckGameMasterLevel(lpObj, 1) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"Sword Blow test command requires GameMaster level 1.");
			return 0;
		}
		const int skill = rise::pegasus::kSwordBlowSkill;
		for (int n = 0; n < MAX_SKILL_LIST; ++n)
		{
			if (lpObj->Skill[n].IsSkill() != 0 &&
				(lpObj->Skill[n].m_index == skill || lpObj->Skill[n].m_skill == skill))
			{
				const int level = lpObj->Skill[n].m_level;
				lpObj->Skill[n].Clear();
				gSkillManager.GCSkillDelSend(lpObj->Index, (BYTE)n, skill,
					(BYTE)level, 0);
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
					(char*)"Sword Blow (1501) removed; relog before GS rollback.");
				return 1;
			}
		}
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)"Sword Blow (1501) is not in this character skill list.");
		return 1;
	}
	if (_stricmp(command, "/testswordblow") == 0)
	{
		if ((lpObj->Authority & 32) == 0 &&
			gGameMaster.CheckGameMasterLevel(lpObj, 1) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"Sword Blow test command requires GameMaster level 1.");
			return 0;
		}

		const int skill = rise::pegasus::kSwordBlowSkill;
		for (int n = 0; n < MAX_SKILL_LIST; ++n)
		{
			if (lpObj->Skill[n].IsSkill() != 0 &&
				(lpObj->Skill[n].m_index == skill || lpObj->Skill[n].m_skill == skill))
			{
				gSkillManager.GCSkillAddSend(lpObj->Index, (BYTE)n, skill,
					(BYTE)lpObj->Skill[n].m_level, 0);
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
					(char*)"Sword Blow (1501) is already available.");
				return 1;
			}
		}

		for (int n = 0; n < MAX_SKILL_LIST; ++n)
		{
			if (lpObj->Skill[n].IsSkill() == 0 && lpObj->Skill[n].Set(skill, 0) != 0)
			{
				gSkillManager.GCSkillAddSend(lpObj->Index, (BYTE)n, skill, 0, 0);
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
					(char*)"Sword Blow (1501) added for this isolated QA session.");
				return 1;
			}
		}

		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			(char*)"Sword Blow could not be added: skill list is full.");
		return 0;
	}
	if (_stricmp(command, "/testswordblowfly") == 0)
	{
		if (!IsIsolatedSkillTestGM(lpObj))
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
				(char*)"Sword Blow Fly test command requires GameMaster level 1.");
			return 0;
		}
		const int result = AddIsolatedTestSkill(lpObj,
			rise::pegasus::kSwordBlowFlySkill);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
			result == 2 ? (char*)"Sword Blow Fly (1502) added for isolated QA." :
			result == 1 ? (char*)"Sword Blow Fly (1502) is already available." :
			(char*)"Sword Blow Fly could not be added: skill list is full.");
		return result != 0;
	}
	if (_stricmp(command, "/testswordblowflyoff") == 0)
	{
		if (!IsIsolatedSkillTestGM(lpObj))
			return 0;
		for (int n = 0; n < MAX_SKILL_LIST; ++n)
		{
			if (lpObj->Skill[n].IsSkill() != 0 &&
				(lpObj->Skill[n].m_index == rise::pegasus::kSwordBlowFlySkill ||
				 lpObj->Skill[n].m_skill == rise::pegasus::kSwordBlowFlySkill))
			{
				const int level = lpObj->Skill[n].m_level;
				lpObj->Skill[n].Clear();
				gSkillManager.GCSkillDelSend(lpObj->Index, (BYTE)n,
					rise::pegasus::kSwordBlowFlySkill, (BYTE)level, 0);
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0,
					(char*)"Sword Blow Fly (1502) removed.");
				return 1;
			}
		}
		return 1;
	}
#endif
	COMMAND_LIST CommandData;
	if (gCommand.GetInfoByName(command, &CommandData) == 0)
	{
		return 0;
	}
	int code = this->GetCommandCode(command);
	char* argument = &message[strlen(command)];
	if (argument[0] == 0x20)
	{
		argument++;
	}
	if (CommandData.Enable[lpObj->AccountLevel] == 0)
	{
		return 0;
	}
	if (gGameMaster.CheckGameMasterLevel(lpObj, CommandData.GameMaster) == 0)
	{
		if (lpObj->Interface.type == INTERFACE_TRADE)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
			return 0;
		}
	}
	if (lpObj->Money < (DWORD)CommandData.Money[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(83), CommandData.Money[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(83), CommandData.Money[lpObj->AccountLevel]);
		}
		return 0;
	}
	if ((DWORD)CommandData.Money[lpObj->AccountLevel] > 0)
	{
		if (lpObj->Interface.use == INTERFACE_TRADE)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(635));
			if (Npc >= 0)
			{
				GCChatTargetNewSend(lpObj, Npc, pMessage->Text(635));
			}
			return 0;
		}
	}
	if (lpObj->Level < (CommandData.MinLevel[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(84), CommandData.MinLevel[lpObj->AccountLevel]);
		if (Npc > 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(84));
		}
		return 0;
	}
	if (CommandData.MaxLevel[lpObj->AccountLevel] != 0 && lpObj->Level > (CommandData.MaxLevel[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(565), CommandData.MaxLevel[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(565), CommandData.MaxLevel[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (lpObj->Reset < (CommandData.MinReset[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(85), CommandData.MinReset[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(85), CommandData.MinReset[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (CommandData.MaxReset[lpObj->AccountLevel] != 0 && lpObj->Reset > (CommandData.MaxReset[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(255), CommandData.MaxReset[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(255), CommandData.MaxReset[lpObj->AccountLevel]);
		}
		return 0;
	}
	DWORD tick = (GetTickCount() - lpObj->CommandDelay[code]) / 1000;
	if (tick < ((DWORD)CommandData.Delay))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(86), (CommandData.Delay - tick));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(86), (CommandData.Delay - tick));
		}
		return 0;
	}
	if (CommandData.GameMaster != 0 && gGameMaster.CheckGameMasterLevel(lpObj, CommandData.GameMaster) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(87));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(87));
		}
		return 0;
	}
	if (CommandData.Coin1 > 0 || CommandData.Coin2 > 0 || CommandData.Coin3 > 0)
	{
		if (code != COMMAND_CHANGECLASS && lpObj->CommandNotice[code] == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(637), CommandData.Coin1, CommandData.Coin2, CommandData.Coin3);
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(638));
			if (Npc >= 0)
			{
				GCChatTargetNewSend(lpObj, Npc, pMessage->Text(637), CommandData.Coin1, CommandData.Coin2, CommandData.Coin3);
			}
			lpObj->CommandNotice[code] = 1;
			return 0;
		}
	}
	if (lpObj->ThisCoin[0] < CommandData.Coin1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(566), CommandData.Coin1);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(566), CommandData.Coin1);
		}
		return 0;
	}
	if (lpObj->ThisCoin[1] < CommandData.Coin2)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(567), CommandData.Coin2);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(567), CommandData.Coin2);
		}
		return 0;
	}
	if (lpObj->ThisCoin[2] < CommandData.Coin3)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(568), CommandData.Coin3);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(568), CommandData.Coin3);
		}
		return 0;
	}
	int Result = 0;
	switch (code)
	{
		case COMMAND_MOVE:				Result = CommandMove(lpObj, argument);									break;
		case COMMAND_POST:				Result = CommandPost(lpObj, argument);									break;
		case COMMAND_ADD_POINT1:		Result = CommandAddPoint(lpObj, argument, 0);							break;
		case COMMAND_ADD_POINT2:		Result = CommandAddPoint(lpObj, argument, 1);							break;
		case COMMAND_ADD_POINT3:		Result = CommandAddPoint(lpObj, argument, 2);							break;
		case COMMAND_ADD_POINT4:		Result = CommandAddPoint(lpObj, argument, 3);							break;
		case COMMAND_ADD_POINT5:		Result = CommandAddPoint(lpObj, argument, 4);							break;
		case COMMAND_PK_CLEAR:			Result = CommandPKClear(lpObj, argument, Npc);							break;
		case COMMAND_MONEY:				Result = CommandMoney(lpObj, argument);									break;
		case COMMAND_CHANGE:			Result = CommandChange(lpObj, argument, Npc);							break;
		case COMMAND_WARE:				Result = CommandWare(lpObj, argument);									break;
		case COMMAND_RESET:				Result = CommandReset(lpObj, argument, Npc);							break;
		case COMMAND_GM_MOVE:			Result = CommandGMMove(lpObj, argument);								break;
		case COMMAND_GM_POST:			Result = CommandGMPost(lpObj, argument);								break;
		case COMMAND_TRACK:				Result = CommandTrack(lpObj, argument);									break;
		case COMMAND_TRACE:				Result = CommandTrace(lpObj, argument);									break;
		case COMMAND_DISCONNECT:		Result = CommandDisconnect(lpObj, argument);							break;
		case COMMAND_FIREWORKS:			Result = CommandFireworks(lpObj, argument);								break;
		case COMMAND_SKIN:				Result = CommandSkin(lpObj, argument);									break;
		case COMMAND_SET_MONEY:			Result = CommandSetMoney(lpObj, argument);								break;
		case COMMAND_NOTICE:			Result = CommandNotice(lpObj, argument);								break;
		case COMMAND_MASTER_RESET:		Result = CommandMasterReset(lpObj, argument, Npc);						break;
		case COMMAND_GUILD_WAR:			Result = CommandGuildWar(lpObj, argument);								break;
		case COMMAND_BATTLE_SOCCER:		Result = CommandBattleSoccer(lpObj, argument);							break;
		case COMMAND_REQUEST:			Result = CommandRequest(lpObj, argument);								break;
		case COMMAND_CUSTOM_STORE:		Result = gCustomStore.CommandCustomStore(lpObj, argument);				break;
		case COMMAND_CUSTOM_OFFSTORE:	Result = gCustomStore.CommandCustomStoreOffline(lpObj, argument);		break;
		case COMMAND_HIDE:				Result = CommandHide(lpObj, argument);									break;
		case COMMAND_CUSTOM_ATTACK:		Result = gCustomAttack.CommandCustomAttack(lpObj, argument);			break;
		case COMMAND_CUSTOM_OFFATTACK:	Result = gCustomAttack.CommandCustomAttackOffline(lpObj, argument);		break;
		case COMMAND_CLEAR_INVENTORY:	Result = CommandClearInventory(lpObj, argument);						break;
		case COMMAND_SETVIP:			Result = CommandSetVIP(lpObj, argument);								break;
		case COMMAND_MARRY:				Result = CommandMarry(lpObj, argument);									break;
		case COMMAND_OPENWAREHOUSE:		Result = CommandOpenWarehouse(lpObj);									break;
		case COMMAND_CHANGECLASS:		Result = g_ChangeClass->CommandChangeClass(lpObj, argument, Npc);	break;
		case COMMAND_REWARD:			Result = CommandReward(lpObj, argument);								break;
		case COMMAND_REWARDALL:			Result = CommandRewardAll(lpObj, argument);								break;
		case COMMAND_RELOAD:			Result = CommandReload(lpObj, argument);								break;
		case COMMAND_RENAME:			Result = CommandRename(lpObj, argument);								break;
		case COMMAND_INF:				Result = CommandInfo(lpObj, argument);									break;
		case COMMAND_BLOCCHAR:			Result = CommandBlocChar(lpObj, argument);								break;
		case COMMAND_BLOCACC:			Result = CommandBlocAcc(lpObj, argument);								break;
		case COMMAND_MAKE:				Result = g_SocketMaker->SocketCommandMake(lpObj, argument);				break;
		case COMMAND_MAKESET:			Result = g_SocketMaker->SocketCommandMakeSet(lpObj, argument);			break;
		case COMMAND_DROP:				Result = CommandDrop(lpObj, argument);									break;
		case COMMAND_TOP:				Result = CommandTop(lpObj, argument);									break;
		case COMMAND_READD:				Result = CommandReAdd(lpObj, argument, Npc);							break;
		case COMMAND_PICK:				Result = gCustomPick.CommandPick(lpObj, argument);						break;
		case COMMAND_HELPER:			Result = CommandHelper(lpObj, argument, Npc);							break;
		case COMMAND_REMASTER:			Result = CommandReMaster(lpObj, argument, Npc);							break;
		case COMMAND_STARTBC:			Result = CommandStartBC(lpObj, argument);								break;
		case COMMAND_STARTDS:			Result = CommandStartDS(lpObj, argument);								break;
		case COMMAND_STARTCC:			Result = CommandStartCC(lpObj, argument);								break;
		case COMMAND_STARTIT:			Result = CommandStartIT(lpObj, argument);								break;
		case COMMAND_DISABLEPVP:		Result = CommandDisablePvp(lpObj, argument, Npc);						break;
		case COMMAND_LOCK:				Result = CommandLock(lpObj, argument);									break;
		case COMMAND_UNLOCK:			Result = CommandUnLock(lpObj, argument);								break;
		case COMMAND_MOVEALL:			Result = CommandMoveAll(lpObj, argument);								break;
		case COMMAND_MOVEGUILD:			Result = CommandMoveGuild(lpObj, argument);								break;
		case COMMAND_KILLALL:			Result = gEventKillAll.CommandEventKillAll(lpObj, argument);			break;
		case COMMAND_SPOT:				Result = CommandSpot(lpObj, argument);									break;
		case COMMAND_SETCOIN:			Result = CommandSetCoin(lpObj, argument);								break;
		case COMMAND_PACK:				Result = gJewelMix.CommandJewelMix(lpObj, argument);					break;
		case COMMAND_UNPACK:			Result = gJewelMix.CommandJewelUnMix(lpObj, argument);					break;
		case COMMAND_STARTKING:			Result = CommandStartKing(lpObj, argument);								break;
		case COMMAND_STARTINVASION:		Result = CommandStartInvasion(lpObj, argument);							break;
		case COMMAND_QUYETDAU:			Result = CommandQuyetDau(lpObj, argument);								break;
		case COMMAND_QUYETDAUOK:		Result = CommandQuyetDauOK(lpObj, argument);							break;
		case COMMAND_SETLEVEL:			Result = CommandSetLevel(lpObj, argument);								break;
		case COMMAND_SETRESET:			Result = CommandSetReset(lpObj, argument);								break;
		case COMMAND_MAKEINV:			Result = CommandMakeExtInventory(lpObj, argument);						break;
		case COMMAND_GIFT:				Result = CommandGift(lpObj, argument);									break;
		case COMMAND_ADDBANK:			break;
		case COMMAND_AUTOHP:			Result = gAUTOHP.CommandHp(lpObj);										break;
		case COMMAND_SETPASS:			Result = gCommandManager.CommandSetPass(lpObj, argument);				break;
		case COMMAND_ADDBUFF:			Result = g_AddBuffer->CommandAddBuff(lpObj);								break;
		case COMMAND_DELBUFF:			Result = g_AddBuffer->CommandDelBuff(lpObj);								break;
		case COMMAND_REI:
		{
			gReiDoMU.CommandReiDoMU(lpObj, argument);
			Result = 0;
		}
		break;
		case COMMAND_EVENTGET:
		{
			gEventRunAndCatch.CommandEventRunAndCatch(lpObj, argument);
			Result = 0;
		}
		break;
		case COMMAND_EVENTGETJOIN:
		{
			gEventRunAndCatch.CommandEventRunAndCatchJoin(lpObj, argument);
			gEventKillAll.CommandEventKillAllJoin(lpObj, argument);
			Result = 0;
		}
		break;
		case COMMAND_EVENTESC:
		{
			gEventHideAndSeek.CommandEventHideAndSeek(lpObj, argument);
			Result = 0;
		}
		break;
		case COMMAND_QUEST:
		{
			gCustomQuest.QuestCommand(lpObj, argument, Npc);
			Result = 0;
		}
		break;
		default:
		return 0;
	}
	if (Result)
	{
		this->RemoveRequisites(lpObj, CommandData.Index);
		lpObj->CommandDelay[code] = GetTickCount();
	}
	return 1;
}
void CCommandManager::RemoveRequisites(LPOBJ lpObj, int IdCommand)
{
	COMMAND_LIST CommandData;
	if (gCommand.GetInfo(IdCommand, &CommandData) == 1)
	{
		if (CommandData.Money[lpObj->AccountLevel] > 0)
		{
			lpObj->Money -= CommandData.Money[lpObj->AccountLevel];
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(636), CommandData.Money[lpObj->AccountLevel]);
			GCMoneySend(lpObj->Index, lpObj->Money);
		}
		if (CommandData.Coin1 > 0 || CommandData.Coin2 > 0 || CommandData.Coin3 > 0)
		{
			GDSetCoinSend(lpObj->Index, -(CommandData.Coin1), -(CommandData.Coin2), -(CommandData.Coin3), 0, 0, "RemoveRequisites");
		}
	}
}
bool CCommandManager::CommandMove(LPOBJ lpObj, char* arg)
{
	MOVE_INFO MoveInfo;
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	if (gMove.GetInfoByName(arg, &MoveInfo) != 0)
	{
		gMove.Move(lpObj, MoveInfo.Index);
		return 1;
	}
	return 0;
}
bool CCommandManager::CommandPost(LPOBJ lpObj, char* arg)
{
	gFilter.CheckSyntax(arg);
	if (gServerInfo.m_CommandPostType >= 0)
	{
		short tmpservercode = gServerInfo.m_ServerCode;
		while (tmpservercode >= 20) { tmpservercode -= 20; }
		tmpservercode += 1;
		char szTemp[256];
		wsprintf(szTemp, "Sub-%d %s", tmpservercode, arg);
		GDGlobalPostSend(gMapServerManager.GetMapServerGroup(), 2, lpObj->Name, szTemp);
	}
	gLog.Output(LOG_COMMAND, "[CommandPost][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, arg);
	return 1;
}
bool CCommandManager::CommandAddPoint(LPOBJ lpObj, char* arg, int type)
{
	if (gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel] != 0 && lpObj->Class == CLASS_DL && type == 4)
	{
		return 0;
	}
	if (lpObj->Class != CLASS_DL && type == 4)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(89));
		return 0;
	}
	char mode[5] = { 0 };
	this->GetString(arg, mode, sizeof(mode), 0);
	if (strcmp(mode, "auto") == 0)
	{
		this->CommandAddPointAuto(lpObj, arg, type);
		return 1;
	}
	int amount = this->GetNumber(arg, 0);
	if (amount <= 0 || lpObj->LevelUpPoint < amount)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(92));
		return 0;
	}
	if (gObjectManager.CharacterLevelUpPointAdd(lpObj, type, amount) == 0)
	{
		return 0;
	}
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(91), amount, lpObj->LevelUpPoint);
	gLog.Output(LOG_COMMAND, "[CommandAddPoint][%s][%s] - (Type: %d, Amount: %d)", lpObj->Account, lpObj->Name, type, amount);
	return 1;
}
void CCommandManager::CommandAddPointAuto(LPOBJ lpObj, char* arg, int type)
{
	if (gServerInfo.m_CommandAddPointAutoEnable[lpObj->AccountLevel] == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(90));
		return;
	}
	int amount = this->GetNumber(arg, 1);
	if (amount > 0)
	{
		lpObj->AutoAddPointCount = 0;
		lpObj->AutoAddPointStats[type] = amount;
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[0] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[1] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[2] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[3] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[4] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(184), amount);
		gLog.Output(LOG_COMMAND, "[CommandAddPointAuto][%s][%s] - (Type: %d, Amount: %d)", lpObj->Account, lpObj->Name, type, amount);
	}
	else
	{
		lpObj->AutoAddPointCount = 0;
		lpObj->AutoAddPointStats[type] = 0;
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[0] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[1] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[2] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[3] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[4] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(185));
	}
}
bool CCommandManager::CommandPKClear(LPOBJ lpObj, char* arg, int Npc)
{
	if (lpObj->PKLevel <= 3)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(93));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(93));
		}
		return 0;
	}
	lpObj->PKLevel = 3;
	lpObj->PKCount = 0;
	GCPKLevelSend(lpObj->Index, lpObj->PKLevel);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(199));
	if (Npc >= 0)
	{
		GCChatTargetNewSend(lpObj, Npc, pMessage->Text(199));
	}
	gLog.Output(LOG_COMMAND, "[CommandPKClear][%s][%s] - (PKLevel: %d)", lpObj->Account, lpObj->Name, lpObj->PKLevel);
	return 1;
}
bool CCommandManager::CommandMoney(LPOBJ lpObj, char* arg)
{
	int money = this->GetNumber(arg, 0);
	if (money < 0)
	{
		return 0;
	}
	if (money == 0)
	{
		lpObj->Money = 0;
	}
	else if (gObjCheckMaxMoney(lpObj->Index, money) == 0)
	{
		lpObj->Money = MAX_MONEY;
	}
	else
	{
		lpObj->Money += money;
	}
	GCMoneySend(lpObj->Index, lpObj->Money);
	gLog.Output(LOG_COMMAND, "[CommandMoney][%s][%s] - (Money: %d)", lpObj->Account, lpObj->Name, money);
	return 1;
}
bool CCommandManager::CommandChange(LPOBJ lpObj, char* arg, int Npc)
{
	int ChangeUp = lpObj->ChangeUp;
	if (lpObj->Class == CLASS_MG || lpObj->Class == CLASS_DL || lpObj->Class == CLASS_RF)
	{
		ChangeUp++;
	}
	if (ChangeUp >= gServerInfo.m_CommandChangeLimit[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(198));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(198));
		}
		return 0;
	}
	for (int n = 0; n <= ((ChangeUp == 0) ? 3 : 6); n++)
	{
		if (gQuest.CheckQuestListState(lpObj, n, QUEST_FINISH) == 0)
		{
			if (n != 3 || lpObj->Class == CLASS_DK)
			{
				gQuest.AddQuestList(lpObj, n, QUEST_ACCEPT);
				gQuestReward.InsertQuestReward(lpObj, n);
				gQuest.AddQuestList(lpObj, n, QUEST_FINISH);
			}
		}
	}
	lpObj->SendQuestInfo = 0;
	gQuest.GCQuestInfoSend(lpObj->Index);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(197));
	if (Npc >= 0)
	{
		GCChatTargetNewSend(lpObj, Npc, pMessage->Text(197));
	}
	g_UserRank->ProtocolUserRank(lpObj->Index, lpObj->Index);
	GDResetInfoSaveSend(lpObj->Index, 0, 0, 0);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	gLog.Output(LOG_COMMAND, "[CommandChange][%s][%s] - (ChangeUp: %d)", lpObj->Account, lpObj->Name, ChangeUp);
	return 1;
}
bool CCommandManager::CommandWare(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.use != 0 || lpObj->State == 32 || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->LoadWarehouse != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(196));
		return 0;
	}
	int number = this->GetNumber(arg, 0);
	if (number < 0 || number >= gServerInfo.m_CommandWareNumber[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(195));
		return 0;
	}
	lpObj->WarehouseMoney = 0;
	memset(lpObj->WarehouseMap, 0xFF, WAREHOUSE_SIZE);
	for (int n = 0; n < WAREHOUSE_SIZE; n++)
	{
		lpObj->Warehouse[n].Clear();
	}
	lpObj->Interface.use = 0;
	lpObj->Interface.type = INTERFACE_NONE;
	lpObj->Interface.state = 0;
	lpObj->LoadWarehouse = 0;
	lpObj->WarehouseNumber = number;
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(194), number);
	gLog.Output(LOG_COMMAND, "[CommandWare][%s][%s] - (Number: %d)", lpObj->Account, lpObj->Name, number);
	return 1;
}
bool CCommandManager::CommandReset(LPOBJ lpObj, char* arg, int Npc)
{
	char mode[5] = { 0 };
	this->GetString(arg, mode, sizeof(mode), 0);
	if (lpObj->Interface.type == INTERFACE_TRADE)
	{
		return 0;
	}
	if (strcmp(mode, "auto") == 0)
	{
		this->CommandResetAuto(lpObj, arg, Npc);
		return 1;
	}
	if (lpObj->PKLevel >= 5)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(221));
		return 0;
	}
	if (lpObj->Interface.use != 0 || lpObj->State == 32 || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(190));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(190));
		}
		return 0;
	}
	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(191), gResetTable.GetResetLevel(lpObj));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(191), gResetTable.GetResetLevel(lpObj));
		}
		return 0;
	}
	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(192), gResetTable.GetResetMoney(lpObj));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(192), gResetTable.GetResetMoney(lpObj));
		}
		return 0;
	}
	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(173), gServerInfo.m_CommandResetLimit[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(173), gServerInfo.m_CommandResetLimit[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (gServerInfo.m_CommandResetCheckItem[lpObj->AccountLevel] != 0)
	{
		const int equipmentSlots[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 237, 238, 239, 240, 241, 242 };
		for (int n : equipmentSlots)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(174));
				if (Npc >= 0)
				{
					GCChatTargetNewSend(lpObj, Npc, pMessage->Text(174));
				}
				return 0;
			}
		}
	}
	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return 0;
	}
	SDHP_COMMAND_RESET_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	return 0;
}
void CCommandManager::CommandResetAuto(LPOBJ lpObj, char* arg, int Npc)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return;
	}
	if (gServerInfo.m_CommandResetAutoEnable[lpObj->AccountLevel] == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(193));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(193));
		}
		return;
	}
	if (lpObj->AutoResetEnable == 0)
	{
		lpObj->AutoResetEnable = (gServerInfo.m_CommandResetAutoEnable[lpObj->AccountLevel] == 0) ? 0 : 1;
		lpObj->AutoResetStats[0] = this->GetNumber(arg, 1);
		lpObj->AutoResetStats[1] = this->GetNumber(arg, 2);
		lpObj->AutoResetStats[2] = this->GetNumber(arg, 3);
		lpObj->AutoResetStats[3] = this->GetNumber(arg, 4);
		lpObj->AutoResetStats[4] = ((lpObj->Class == CLASS_DL) ? this->GetNumber(arg, 5) : 0);
		gCustomAttack.DGCustomAttackResumeSaveSend(lpObj->Index);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(186));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(186));
		}
	}
	else
	{
		lpObj->AutoResetEnable = 0;
		lpObj->AutoResetStats[0] = 0;
		lpObj->AutoResetStats[1] = 0;
		lpObj->AutoResetStats[2] = 0;
		lpObj->AutoResetStats[3] = 0;
		lpObj->AutoResetStats[4] = 0;
		gCustomAttack.DGCustomAttackResumeSaveSend(lpObj->Index);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(187));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(187));
		}
	}
	gObjCheckAutoRest = lpObj->AutoResetEnable;
}
bool CCommandManager::CommandGMMove(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	int map = this->GetNumber(arg, 1);
	int x = this->GetNumber(arg, 2);
	int y = this->GetNumber(arg, 3);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	gObjTeleport(lpTarget->Index, map, x, y);
	gLog.Output(LOG_COMMAND, "[CommandGMMove][%s][%s] - (Name: %s, Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, name, map, x, y);
	return 1;
}
bool CCommandManager::CommandGMPost(LPOBJ lpObj, char* arg)
{
	gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, "%s", arg);
	gLog.Output(LOG_COMMAND, "[CommandGMPost][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, arg);
	return 1;
}
bool CCommandManager::CommandTrack(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	gObjTeleport(lpObj->Index, lpTarget->Map, lpTarget->X, lpTarget->Y);
	gLog.Output(LOG_COMMAND, "[CommandTrack][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
	return 1;
}
bool CCommandManager::CommandTrace(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	gObjTeleport(lpTarget->Index, lpObj->Map, lpObj->X, lpObj->Y);
	gLog.Output(LOG_COMMAND, "[CommandTrace][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
	return 1;
}
bool CCommandManager::CommandDisconnect(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	gObjUserKill(lpTarget->Index);
	gLog.Output(LOG_COMMAND, "[CommandDisconnect][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
	return 1;
}
bool CCommandManager::CommandFireworks(LPOBJ lpObj, char* arg)
{
	for (int n = 0; n < 15; n++)
	{
		GCFireworksSend(lpObj, (lpObj->X + (((GetLargeRand() % 5) * 2) - 4)), (lpObj->Y + (((GetLargeRand() % 5) * 2) - 4)));
	}
	gLog.Output(LOG_COMMAND, "[CommandFireworks][%s][%s] - (Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, lpObj->Map, lpObj->X, lpObj->Y);
	return 1;
}
bool CCommandManager::CommandMake(LPOBJ lpObj, char* arg)
{
	int qtd = 0, section, type, level, skill, luck, option, exc, set, HarmonyOption, OptionEX, socket;
	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF,0xFF,0xFF,0xFF,0xFF };
	section = this->GetNumber(arg, 0);
	if (section < 0)
	{
		qtd = section;
		section = this->GetNumber(arg, 1);
		type = this->GetNumber(arg, 2);
		level = this->GetNumber(arg, 3);
		skill = this->GetNumber(arg, 4);
		luck = this->GetNumber(arg, 5);
		option = this->GetNumber(arg, 6);
		exc = this->GetNumber(arg, 7);
		set = this->GetNumber(arg, 8);
		HarmonyOption = this->GetNumber(arg, 9);
		OptionEX = this->GetNumber(arg, 10);
		socket = this->GetNumber(arg, 11);
	}
	else
	{
		type = this->GetNumber(arg, 1);
		level = this->GetNumber(arg, 2);
		skill = this->GetNumber(arg, 3);
		luck = this->GetNumber(arg, 4);
		option = this->GetNumber(arg, 5);
		exc = this->GetNumber(arg, 6);
		set = this->GetNumber(arg, 7);
		HarmonyOption = this->GetNumber(arg, 8);
		OptionEX = this->GetNumber(arg, 9);
		socket = this->GetNumber(arg, 10);
	}
	for (int n = 0; n < socket && socket <= MAX_SOCKET_OPTION; n++)
	{
		ItemSocketOption[n] = 0xFE;
	}
	(qtd < 0) ? qtd -= qtd * 2 : qtd;
	int index = GET_ITEM(section, type);
	if (qtd > 0)
	{
		lpObj->IsMakeCommand = true;
		lpObj->MakeCount = qtd;
		lpObj->MakeLoggedCount = 0;
		for (int n = 0; n < qtd; n++)
		{
			GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, index, level, 0, skill, luck, option, -1, exc, set, HarmonyOption, OptionEX, ItemSocketOption, 0xFF, 0);
		}
	}
	else
	{
		lpObj->IsMakeCommand = true;
		lpObj->MakeCount = 1;
		lpObj->MakeLoggedCount = 0;
		GDCreateItemSend(lpObj->Index, 0xEB, 0, 0, index, level, 0, skill, luck, option, -1, exc, set, HarmonyOption, OptionEX, ItemSocketOption, 0xFF, 0);
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(177));
	return 1;
}
bool CCommandManager::CommandMakeExtInventory(LPOBJ lpObj, char* arg)
{
	int Slot = this->GetNumber(arg, 0);
	int InventoryCoin = gServerInfo.m_SetInventoryCoin;
	int WareHouseCoin = gServerInfo.m_SetWareHouseCoin;
	if (Slot < 0 || Slot > 4)
	{
		return false;
	}
	if (Slot >= 0 && Slot <= 3)
	{
		if (lpObj->ExtInventory > Slot)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(57), Slot);
			return false;
		}
		if (lpObj->ThisCoin[0] < InventoryCoin)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(56), InventoryCoin);
			return false;
		}
		GDSetCoinSend(lpObj->Index, -InventoryCoin, 0, 0, 0, 0, "Set InventoryExt");
		lpObj->ExtInventory++;
		if (lpObj->ExtInventory > 4)
		{
			lpObj->ExtInventory = 4;
		}
		else if (lpObj->ExtInventory < 0)
		{
			lpObj->ExtInventory = 0;
		}
		GDResetInfoSaveSend(lpObj->Index, 0, 0, 0);
		gObjectManager.CharacterCalcAttribute(lpObj->Index);
		gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
		GDCharacterInfoSaveSend(lpObj->Index);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(55), lpObj->ExtInventory);
		return true;
	}
	else if (Slot == 4)
	{
		if (lpObj->ExtWarehouse == 0 && lpObj->ExtWarehouse < 1)
		{
			if (lpObj->ThisCoin[0] < WareHouseCoin)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(56), WareHouseCoin);
				return false;
			}
			GDSetCoinSend(lpObj->Index, -WareHouseCoin, 0, 0, 0, 0, "Set WarehouseExt");
			lpObj->ExtWarehouse = 1;
			GDResetInfoSaveSend(lpObj->Index, 0, 0, 0);
			gObjectManager.CharacterCalcAttribute(lpObj->Index);
			gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
			GDCharacterInfoSaveSend(lpObj->Index);
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(458));
		}
		else
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(459));
			return false;
		}
		return true;
	}
	return false;
}

bool CCommandManager::CommandMakeSet(LPOBJ lpObj, char* arg)
{
	constexpr int kMaxSocket = MAX_SOCKET_OPTION;
	constexpr int itemSlotStart = 7;
	constexpr int itemSlotEnd = 11;
	int ValueData[10];
	for (int i = 0; i <= 9; i++)
	{
		ValueData[i] = this->GetNumber(arg, i);
	}
	BYTE itemSocketOption[kMaxSocket] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	for (int i = 0; i < ValueData[9] && i < kMaxSocket; ++i)
	{
		itemSocketOption[i] = 0xFE;
	}
	for (int i = itemSlotStart; i <= itemSlotEnd; ++i)
	{
		GDCreateItemSend(
			lpObj->Index, 0xEB, 0, 0, GET_ITEM(i, ValueData[0]), ValueData[1], 0,
			ValueData[2], ValueData[3], ValueData[4], -1, ValueData[5], ValueData[6],
			ValueData[7], ValueData[8], itemSocketOption, 0xFF, 0
		);
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(177));
	gLog.Output(LOG_COMMAND,
		"[CommandMakeSet][%s][%s] - (Type: %d, Level: %d, Skill: %d, Luck: %d, Option: %d, Exc: %d, Set: %d, GC: %d, 380: %d, Sockets: %d)",
		lpObj->Account, lpObj->Name,
		ValueData[0], ValueData[1], ValueData[2], ValueData[3], ValueData[4],
		ValueData[5], ValueData[6], ValueData[7], ValueData[8], ValueData[9]);
	return true;
}
bool CCommandManager::CommandDrop(LPOBJ lpObj, char* arg)
{
	int section = this->GetNumber(arg, 0);
	int type = this->GetNumber(arg, 1);
	int level = this->GetNumber(arg, 2);
	int skill = this->GetNumber(arg, 3);
	int luck = this->GetNumber(arg, 4);
	int option = this->GetNumber(arg, 5);
	int exc = this->GetNumber(arg, 6);
	int set = this->GetNumber(arg, 7);
	int HarmonyOption = this->GetNumber(arg, 8);
	int OptionEX = this->GetNumber(arg, 9);
	int socket = this->GetNumber(arg, 10);
	BYTE ItemSocketOption[MAX_SOCKET_OPTION] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	for (int n = 0; n < socket && socket <= MAX_SOCKET_OPTION; n++)
	{
		ItemSocketOption[n] = 0xFE;
	}
	GDCreateItemSend(lpObj->Index, lpObj->Map, (BYTE)lpObj->X, (BYTE)lpObj->Y, GET_ITEM(section, type), level, 0, skill, luck, option, -1, exc, set, 0, 0, ItemSocketOption, 0xFF, 0);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(177));
	gLog.Output(LOG_COMMAND, "[CommandDrop] Account:[%s] Name:[%s] - (Section: %d, Type: %d, Level: %d, Skill: %d, Luck: %d, Option: %d, Exc: %d, Set: %d, Sockets: %d)", lpObj->Account, lpObj->Name, section, type, level, skill, luck, option, exc, set, socket);
	return 1;
}
bool CCommandManager::CommandSkin(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	lpTarget->Change = this->GetNumber(arg, 1);
	gObjViewportListProtocolCreate(lpTarget);
	gObjectManager.CharacterUpdateMapEffect(lpTarget);
	gLog.Output(LOG_COMMAND, "[CommandSkin][%s][%s] - (Name: %s, Change: %d)", lpObj->Account, lpObj->Name, name, lpTarget->Change);
	return 1;
}
bool CCommandManager::CommandSetMoney(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	int money = this->GetNumber(arg, 1);
	if (money < 0)
	{
		return 0;
	}
	if (money == 0)
	{
		lpTarget->Money = 0;
	}
	else if (gObjCheckMaxMoney(lpTarget->Index, money) == 0)
	{
		lpTarget->Money = MAX_MONEY;
	}
	else
	{
		lpTarget->Money += money;
	}
	GCMoneySend(lpTarget->Index, lpTarget->Money);
	gLog.Output(LOG_COMMAND, "[CommandSetMoney][%s][%s] - (Name: %s, Money: %d)", lpObj->Account, lpObj->Name, name, money);
	return 1;
}
bool CCommandManager::CommandSetVIP(LPOBJ lpObj, char* arg)
{
	char account[11] = { 0 };
	this->GetString(arg, account, sizeof(account), 0);
	int tipo = this->GetNumber(arg, 1);
	int dias = this->GetNumber(arg, 2);
	if (dias < 0 || tipo < 0)
	{
		return 0;
	}
	GJAccountLevelSaveSend(lpObj->Index, tipo, (dias * 86400), account);
	GJAccountLevelSend(lpObj->Index, account);
	gLog.Output(LOG_COMMAND, "[CommandSetVIP][%s][%s] - (Account: %s, Tipo: %d, Dias: %d)", lpObj->Account, lpObj->Name, account, tipo, dias);
	return 1;
}
bool CCommandManager::CommandNotice(LPOBJ lpObj, char* arg)
{
	GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, arg);
	gLog.Output(LOG_COMMAND, "[CommandNotice][%s][%s] - (Message: %s)", lpObj->Account, lpObj->Name, arg);
	return 1;
}
bool CCommandManager::CommandMasterReset(LPOBJ lpObj, char* arg, int Npc)
{
	if (lpObj->Interface.use != 0 || lpObj->State == 32 || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(188));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(188));
		}
		return 0;
	}
	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(189), gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(189), gServerInfo.m_CommandMasterResetLevel[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (lpObj->Reset < gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(181), gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(181), gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (lpObj->Money < ((DWORD)gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(182), gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(182), gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (lpObj->MasterReset >= gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel])
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(183), gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel]);
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(183), gServerInfo.m_CommandMasterResetLimit[lpObj->AccountLevel]);
		}
		return 0;
	}
	if (gServerInfo.m_CommandMasterResetCheckItem[lpObj->AccountLevel] != 0)
	{
		const int equipmentSlots[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 237, 238, 239, 240, 241, 242 };
		for (int n : equipmentSlots)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(180));
				if (Npc >= 0)
				{
					GCChatTargetNewSend(lpObj, Npc, pMessage->Text(180));
				}
				return 0;
			}
		}
	}
	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return 0;
	}
	SDHP_COMMAND_MASTER_RESET_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x01, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	return 0;
}
bool CCommandManager::CommandGuildWar(LPOBJ lpObj, char* arg)
{
	char GuildName[9] = { 0 };
	this->GetString(arg, GuildName, sizeof(GuildName), 0);
	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 0);
	}
	return 1;
}
bool CCommandManager::CommandBattleSoccer(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	char GuildName[9] = { 0 };
	this->GetString(arg, GuildName, sizeof(GuildName), 0);
	if (strlen(GuildName) >= 1)
	{
		GCGuildWarRequestResult(GuildName, lpObj->Index, 1);
	}
	return 1;
}
bool CCommandManager::CommandRequest(LPOBJ lpObj, char* arg)
{
	char mode[5] = { 0 };
	this->GetString(arg, mode, sizeof(mode), 0);
	if (strcmp(mode, "on") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option &= ~4;
		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(178));
	}
	else if (strcmp(mode, "off") == 0)
	{
		lpObj->Option &= ~1;
		lpObj->Option &= ~2;
		lpObj->Option &= ~4;
		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(176));
	}
	else if (strcmp(mode, "auto") == 0)
	{
		lpObj->Option |= 1;
		lpObj->Option |= 2;
		lpObj->Option |= 4;
		memset(lpObj->AutoPartyPassword, 0, sizeof(lpObj->AutoPartyPassword));
		this->GetString(arg, lpObj->AutoPartyPassword, sizeof(lpObj->AutoPartyPassword), 1);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(208));
	}
	return 1;
}
bool CCommandManager::CommandHide(LPOBJ lpObj, char* arg)
{
	if (gEffectManager.CheckEffect(lpObj, EFFECT_INVISIBILITY) == 0)
	{
		gEffectManager.AddEffect(lpObj, 0, EFFECT_INVISIBILITY, 0, 0, 0, 0, 0);
		gObjViewportListProtocolDestroy(lpObj);
		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 1);
	}
	else
	{
		gEffectManager.DelEffect(lpObj, EFFECT_INVISIBILITY);
		gObjViewportListProtocolCreate(lpObj);
		gLog.Output(LOG_COMMAND, "[CommandHide][%s][%s] - (State: %d)", lpObj->Account, lpObj->Name, 0);
	}
	return 1;
}
void CCommandManager::CommandAddPointAutoProc(LPOBJ lpObj)
{
	if (lpObj->LevelUpPoint == 0 || lpObj->AutoAddPointCount == 0)
	{
		return;
	}
	int AddStatCount = lpObj->AutoAddPointCount;
	int* stat[5] = { &lpObj->Strength,&lpObj->Dexterity,&lpObj->Vitality,&lpObj->Energy,&lpObj->Leadership };
	for (int n = 0; n < 5; n++)
	{
		if (lpObj->AutoAddPointStats[n] > 0)
		{
			if (AddStatCount == 0)
			{
				break;
			}
			int AddStat = lpObj->LevelUpPoint / AddStatCount;
			AddStat = (((lpObj->LevelUpPoint % AddStatCount) > 0) ? (AddStat + 1) : AddStat);
			AddStat = ((AddStat > lpObj->AutoAddPointStats[n]) ? lpObj->AutoAddPointStats[n] : AddStat);
			if (stat[n] == &lpObj->Leadership) {
				AddStat = ((((*stat[n]) + AddStat) > gServerInfo.m_MaxStatPointCMD[lpObj->AccountLevel]) ? (gServerInfo.m_MaxStatPointCMD[lpObj->AccountLevel] - (*stat[n])) : AddStat);
			}
			else {
				AddStat = ((((*stat[n]) + AddStat) > gServerInfo.m_MaxStatPoint[lpObj->AccountLevel]) ? (gServerInfo.m_MaxStatPoint[lpObj->AccountLevel] - (*stat[n])) : AddStat);
			}
			AddStatCount--;
			(*stat[n]) += AddStat;
			lpObj->LevelUpPoint -= AddStat;
			if (stat[n] == &lpObj->Leadership) {
				lpObj->AutoAddPointStats[n] -= (((*stat[n]) >= gServerInfo.m_MaxStatPointCMD[lpObj->AccountLevel]) ? lpObj->AutoAddPointStats[n] : AddStat);
			}
			else {
				lpObj->AutoAddPointStats[n] -= (((*stat[n]) >= gServerInfo.m_MaxStatPoint[lpObj->AccountLevel]) ? lpObj->AutoAddPointStats[n] : AddStat);
			}
			lpObj->AutoAddPointCount = 0;
			lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[0] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
			lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[1] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
			lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[2] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
			lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[3] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
			lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[4] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
		}
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
}
void CCommandManager::CommandResetAutoProc(LPOBJ lpObj)
{
	if (lpObj->Interface.type == INTERFACE_TRADE || lpObj->Interface.use != 0 || lpObj->State == 32 || lpObj->DieRegen != 0 || lpObj->Teleport != 0 || lpObj->PShopOpen != 0 || lpObj->SkillSummonPartyTime != 0)
	{
		return;
	}
	if (((gMasterSkillTree.CheckMasterLevel(lpObj) == 0) ? lpObj->Level : (lpObj->Level + lpObj->MasterLevel)) < gResetTable.GetResetLevel(lpObj))
	{
		return;
	}
	if (lpObj->Money < ((DWORD)gResetTable.GetResetMoney(lpObj)))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(765));
		return;
	}
	if (lpObj->Reset >= gServerInfo.m_CommandResetLimit[lpObj->AccountLevel])
	{
		return;
	}
	if (gServerInfo.m_CommandResetCheckItem[lpObj->AccountLevel] != 0)
	{
		const int equipmentSlots[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 237, 238, 239, 240, 241, 242 };
		for (int n : equipmentSlots)
		{
			if (lpObj->Inventory[n].IsItem() != 0)
			{
				return;
			}
		}
	}
	if (((lpObj->CommandManagerTransaction[0] == 0) ? (lpObj->CommandManagerTransaction[0]++) : lpObj->CommandManagerTransaction[0]) != 0)
	{
		return;
	}
	if (gServerInfo.m_CommandResetAutoEnable[lpObj->AccountLevel] == 0)
	{
		return;
	}
	SDHP_COMMAND_RESET_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x00, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
}
void CCommandManager::DGCommandResetRecv(SDHP_COMMAND_RESET_RECV* lpMsg)
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}
	LPOBJ lpObj = &gObj[lpMsg->index];
	lpObj->CommandManagerTransaction[0] = 0;
	if (lpObj->Reset >= gResetLitmit.GioiHanRS)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(1), gResetLitmit.GioiHanRS);
		return;
	}
	if (lpMsg->ResetDay >= ((DWORD)gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(209), gServerInfo.m_CommandResetLimitDay[lpObj->AccountLevel]);
		return;
	}
	if (lpMsg->ResetWek >= ((DWORD)gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(210), gServerInfo.m_CommandResetLimitWek[lpObj->AccountLevel]);
		return;
	}
	if (lpMsg->ResetMon >= ((DWORD)gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(211), gServerInfo.m_CommandResetLimitMon[lpObj->AccountLevel]);
		return;
	}
	lpObj->Money -= gResetTable.GetResetMoney(lpObj);
	GCMoneySend(lpObj->Index, lpObj->Money);
	lpObj->Level = ((gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandResetStartLevel[lpObj->AccountLevel]);
	lpObj->Experience = gLevelExperience[lpObj->Level - 1];
	lpObj->Reset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->ResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->ResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->ResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_RESET_AMOUNT, gServerInfo.m_CommandResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpObj->AutoAddPointCount = 0;
	lpObj->AutoAddPointStats[0] = 0;
	lpObj->AutoAddPointStats[1] = 0;
	lpObj->AutoAddPointStats[2] = 0;
	lpObj->AutoAddPointStats[3] = 0;
	lpObj->AutoAddPointStats[4] = 0;
	if (gServerInfo.m_CommandResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = ((lpObj->DBClass / 16) * 16);
		lpObj->ChangeUp = lpObj->DBClass % 16;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		BYTE Class = (lpObj->ChangeUp * 16);
		Class -= (Class / 32);
		Class += (lpObj->Class * 32);
		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);
		memset(lpObj->Quest, 0xFF, sizeof(lpObj->Quest));
		gQuest.GCQuestInfoSend(lpObj->Index);
	}
	if (gServerInfo.m_CommandResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;
		lpObj->MasterPoint = 0;
		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}
		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);
		gSkillManager.GCSkillListSend(lpObj, 0);
		gObjectManager.CharacterCalcAttribute(lpObj->Index);
		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}
	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);
		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;
		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];
		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));
		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);
		point += lpObj->FruitAddPoint;
		lpObj->LevelUpPoint = point;
		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		if (gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel] != 0)
		{
			lpObj->Leadership = lpObj->Leadership + gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel];
		}
		else
		{
			lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
		}
	}
	if (gServerInfo.m_CommandResetType == 2)
	{
		int point = gResetTable.GetResetPointKeep(lpObj);
		lpObj->LevelUpPoint += point;
		if (gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel] != 0)
		{
			lpObj->Leadership = lpObj->Leadership + gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel];
		}
	}
	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;
		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;
		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;
		lpObj->LevelUpPoint += point;
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	GDResetInfoSaveSend(lpObj->Index, lpMsg->ResetDay, lpMsg->ResetWek, lpMsg->ResetMon);
	gNotice.GCNoticeSendToAll(eMessageRs, 0, 0, 20, 0, 0, pMessage->Text(43), lpObj->Name, IsToolKit.QN(lpObj->Reset));
	if (gServerInfo.m_CommandResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_DW:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_DK:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_MG:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_DL:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		}
	}
	lpObj->AutoAddPointStats[0] = lpObj->AutoResetStats[0];
	lpObj->AutoAddPointStats[1] = lpObj->AutoResetStats[1];
	lpObj->AutoAddPointStats[2] = lpObj->AutoResetStats[2];
	lpObj->AutoAddPointStats[3] = lpObj->AutoResetStats[3];
	lpObj->AutoAddPointStats[4] = lpObj->AutoResetStats[4];
	lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[0] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
	lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[1] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
	lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[2] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
	lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[3] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
	lpObj->AutoAddPointCount = ((lpObj->AutoAddPointStats[4] > 0) ? (lpObj->AutoAddPointCount + 1) : lpObj->AutoAddPointCount);
	this->CommandAddPointAutoProc(lpObj);
	GCEffectInfoSend(lpObj->Index, 16);
	gLog.Output(LOG_COMMAND, "[CommandReset][%s][%s] - (Reset: %d)", lpObj->Account, lpObj->Name, lpObj->Reset);
}
void CCommandManager::DGCommandMasterResetRecv(SDHP_COMMAND_MASTER_RESET_RECV* lpMsg)
{
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandMasterResetRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}
	LPOBJ lpObj = &gObj[lpMsg->index];
	lpObj->CommandManagerTransaction[0] = 0;
	if (lpMsg->MasterResetDay >= ((DWORD)gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(212), gServerInfo.m_CommandMasterResetLimitDay[lpObj->AccountLevel]);
		return;
	}
	if (lpMsg->MasterResetWek >= ((DWORD)gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(213), gServerInfo.m_CommandMasterResetLimitWek[lpObj->AccountLevel]);
		return;
	}
	if (lpMsg->MasterResetMon >= ((DWORD)gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(214), gServerInfo.m_CommandMasterResetLimitMon[lpObj->AccountLevel]);
		return;
	}
	lpObj->Money -= gServerInfo.m_CommandMasterResetMoney[lpObj->AccountLevel];
	GCMoneySend(lpObj->Index, lpObj->Money);
	lpObj->Level = ((gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel] == -1) ? (lpObj->Level - gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartLevel[lpObj->AccountLevel]);
	lpObj->Experience = gLevelExperience[lpObj->Level - 1];
	if (gServerInfo.m_CommandMasterResetRemoveReset[lpObj->AccountLevel] > 0)
	{
		lpObj->Reset = (lpObj->Reset - gServerInfo.m_CommandMasterResetRemoveReset[lpObj->AccountLevel]);
	}
	else
	{
		lpObj->Reset = ((gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel] == -1) ? (lpObj->Reset - gServerInfo.m_CommandMasterResetReset[lpObj->AccountLevel]) : gServerInfo.m_CommandMasterResetStartReset[lpObj->AccountLevel]);
	}
	lpObj->MasterReset += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->MasterResetDay += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->MasterResetWek += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	lpMsg->MasterResetMon += gBonusManager.GetBonusValue(lpObj, BONUS_INDEX_MASTER_RESET_AMOUNT, gServerInfo.m_CommandMasterResetCount[lpObj->AccountLevel], -1, -1, -1, -1);
	if (gServerInfo.m_CommandMasterResetQuest[lpObj->AccountLevel] != 0)
	{
		lpObj->DBClass = ((lpObj->DBClass / 16) * 16);
		lpObj->ChangeUp = lpObj->DBClass % 16;
		gObjectManager.CharacterMakePreviewCharSet(lpObj->Index);
		BYTE Class = (lpObj->ChangeUp * 16);
		Class -= (Class / 32);
		Class += (lpObj->Class * 32);
		gQuest.GCQuestRewardSend(lpObj->Index, 201, Class);
		memset(lpObj->Quest, 0xFF, sizeof(lpObj->Quest));
		gQuest.GCQuestInfoSend(lpObj->Index);
	}
	if (gServerInfo.m_CommandMasterResetSkill[lpObj->AccountLevel] != 0)
	{
		lpObj->MasterLevel = 0;
		lpObj->MasterPoint = 0;
		for (int n = 0; n < MAX_SKILL_LIST; n++)
		{
			lpObj->Skill[n].Clear();
		}
		for (int n = 0; n < MAX_MASTER_SKILL_LIST; n++)
		{
			lpObj->MasterSkill[n].Clear();
		}
		gMasterSkillTree.GCMasterSkillListSend(lpObj->Index);
		gSkillManager.GCSkillListSend(lpObj, 0);
		gObjectManager.CharacterCalcAttribute(lpObj->Index);
		gMasterSkillTree.GCMasterInfoSend(lpObj);
	}
	if (gServerInfo.m_CommandMasterResetType == 1)
	{
		int point = 0;
		point = gServerInfo.m_CommandMasterResetPoint[lpObj->AccountLevel] * lpObj->MasterReset;
		point = (point * gServerInfo.m_CommandMasterResetPointRate[lpObj->Class]) / 100;
		point += (lpObj->Level - 1) * gServerInfo.m_LevelUpPoint[lpObj->Class];
		point += ((gQuest.CheckQuestListState(lpObj, 2, QUEST_FINISH) == 0) ? 0 : ((lpObj->Level > 220) ? ((lpObj->Level - 220) * gServerInfo.m_PlusStatPoint) : 0));
		point += gQuest.GetQuestRewardLevelUpPoint(lpObj);
		point += lpObj->FruitAddPoint;
		lpObj->LevelUpPoint = point;
		lpObj->Strength = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Strength;
		lpObj->Dexterity = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Dexterity;
		lpObj->Vitality = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Vitality;
		lpObj->Energy = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Energy;
		lpObj->Leadership = gDefaultClassInfo.m_DefaultClassInfo[lpObj->Class].Leadership;
	}
	if (gServerInfo.m_CommandResetType == 1)
	{
		int point = gResetTable.GetResetPoint(lpObj);
		point = (point * gServerInfo.m_CommandResetPointRate[lpObj->Class]) / 100;
		lpObj->LevelUpPoint += point;
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	GDMasterResetInfoSaveSend(lpObj->Index, lpMsg->MasterResetDay, lpMsg->MasterResetWek, lpMsg->MasterResetMon);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(179), lpObj->MasterReset);
	if (gServerInfo.m_CommandMasterResetMove[lpObj->AccountLevel] != 0)
	{
		switch (lpObj->Class)
		{
		case CLASS_DW:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_DK:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_FE:
			gObjMoveGate(lpObj->Index, 27);
			break;
		case CLASS_MG:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_DL:
			gObjMoveGate(lpObj->Index, 17);
			break;
		case CLASS_SU:
			gObjMoveGate(lpObj->Index, 267);
			break;
		case CLASS_RF:
			gObjMoveGate(lpObj->Index, 17);
			break;
		}
	}
	gLog.Output(LOG_COMMAND, "[CommandMasterReset][%s][%s] - (MasterReset: %d)", lpObj->Account, lpObj->Name, lpObj->MasterReset);
}
bool CCommandManager::CommandClearInventory(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	if (lpObj->Lock > 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 2, 0, pMessage->Text(358));
		return 0;
	}
	int MaxValue = gItemManager.GetInventoryMaxValue(lpObj);
	for (int i = INVENTORY_WEAR_SIZE; i < MaxValue; i++)
	{
		CItem& item = lpObj->Inventory[i];
		if (item.IsItem() == 0)
		{
			continue;
		}
		gLog.Output(
			LOG_CMD_XOADO,
			"[ClearInventory][%s/%s] Type: %d, Item: %s, Level: %d, %d, %d, %d, %d, %d, %03d, %03d, %03d, %03d, %03d, %03d, %03d, Slot: %d, Serial: %08X",
			lpObj->Account,
			lpObj->Name,
			item.m_Index,
			gItemManager.GetItemName(item.m_Index),
			item.m_Level,
			item.m_Option1,
			item.m_Option2,
			item.m_Option3,
			item.m_NewOption,
			item.m_SetOption,
			item.m_JewelOfHarmonyOption,
			item.m_ItemOptionEx,
			item.m_SocketOption[0],
			item.m_SocketOption[1],
			item.m_SocketOption[2],
			item.m_SocketOption[3],
			item.m_SocketOption[4],
			i,
			item.m_Serial
		);
		gItemManager.InventoryDelItem(lpObj->Index, i);
		gItemManager.GCItemDeleteSend(lpObj->Index, i, 1);
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(313));
	return 1;
}
bool CCommandManager::CommandMarry(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	char mode[11] = { 0 };
	this->GetString(arg, mode, sizeof(mode), 0);
	if (mode[0] == NULL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(336));
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(363), gServerInfo.m_CommandMarryLevel, gServerInfo.m_CommandMarryCost);
		return 0;
	}
	if (strcmp(mode, "to") == 0 && gServerInfo.m_CommandMarryOnlyGM == 0)
	{
		char MarryName[11] = { 0 };
		this->GetString(arg, MarryName, sizeof(MarryName), 1);
		LPOBJ lpTarget = gObjFind(MarryName);
		if (lpTarget == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(367), MarryName);
			return 0;
		}
		if (strcmp(MarryName, lpObj->Name) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(318));
			return 0;
		}
		SDHP_COMMAND_MARRY_SEND pMsg;
		pMsg.header.set(0x0F, 0x02, sizeof(pMsg));
		pMsg.index = lpObj->Index;
		memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
		memcpy(pMsg.marryname, MarryName, sizeof(pMsg.marryname));
		memcpy(pMsg.mode, "marry", sizeof(pMsg.mode));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	}
	else if (strcmp(mode, "to") == 0 && gServerInfo.m_CommandMarryOnlyGM == 1)
	{
		if (gGameMaster.CheckGameMasterLevel(lpObj, 1) == 0)
		{
			return 0;
		}
		char MarryName1[11] = { 0 };
		char MarryName2[11] = { 0 };
		this->GetString(arg, MarryName1, sizeof(MarryName1), 1);
		this->GetString(arg, MarryName2, sizeof(MarryName2), 2);
		LPOBJ lpTarget1 = gObjFind(MarryName1);
		LPOBJ lpTarget2 = gObjFind(MarryName2);
		if (lpTarget1 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(367), MarryName1);
			return 0;
		}
		if (lpTarget2 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(367), MarryName2);
			return 0;
		}
		if (strcmp(MarryName1, MarryName2) == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(318));
			return 0;
		}
		SDHP_COMMAND_MARRY_SEND pMsg;
		pMsg.header.set(0x0F, 0x02, sizeof(pMsg));
		pMsg.index = lpTarget1->Index;
		memcpy(pMsg.name, MarryName1, sizeof(pMsg.name));
		memcpy(pMsg.marryname, MarryName2, sizeof(pMsg.marryname));
		memcpy(pMsg.mode, "marry", sizeof(pMsg.mode));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	}
	else if (strcmp(mode, "divorce") == 0)
	{
		SDHP_COMMAND_MARRY_SEND pMsg;
		pMsg.header.set(0x0F, 0x02, sizeof(pMsg));
		pMsg.index = lpObj->Index;
		memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
		memcpy(pMsg.marryname, lpObj->Name, sizeof(pMsg.marryname));
		memcpy(pMsg.mode, "divorce", sizeof(pMsg.mode));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	}
	else if (strcmp(mode, "track") == 0)
	{
		if (lpObj->Money < (DWORD)gServerInfo.m_CommandMarryCost)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(360), gServerInfo.m_CommandMarryCost);
			return 0;
		}
		SDHP_COMMAND_MARRY_SEND pMsg;
		pMsg.header.set(0x0F, 0x02, sizeof(pMsg));
		pMsg.index = lpObj->Index;
		memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
		memcpy(pMsg.marryname, lpObj->Name, sizeof(pMsg.marryname));
		memcpy(pMsg.mode, "track", sizeof(pMsg.mode));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	}
	else if (strcmp(mode, "trace") == 0)
	{
		if (lpObj->Money < (DWORD)gServerInfo.m_CommandMarryCost)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(360), gServerInfo.m_CommandMarryCost);
			return 0;
		}
		SDHP_COMMAND_MARRY_SEND pMsg;
		pMsg.header.set(0x0F, 0x02, sizeof(pMsg));
		pMsg.index = lpObj->Index;
		memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
		memcpy(pMsg.marryname, lpObj->Name, sizeof(pMsg.marryname));
		memcpy(pMsg.mode, "trace", sizeof(pMsg.mode));
		gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(336));
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(363), gServerInfo.m_CommandMarryLevel, gServerInfo.m_CommandMarryCost);
		return 0;
	}
	return 0;
}
void CCommandManager::DGCommandMarryRecv(SDHP_COMMAND_MARRY_RECV* lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->index];
	LPOBJ lpTarget = gObjFind(lpMsg->marryname);
	if (strcmp(lpMsg->mode, "marry") == 0)
	{
		if (lpMsg->countyou > 0) {
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(334));
			return;
		}
		if (lpMsg->counthim > 0) {
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(335));
			return;
		}
		int minx = 12;
		int maxx = 15;
		int miny = 23;
		int maxy = 28;
		if (lpObj->Map == 2 && lpObj->X >= minx && lpObj->X <= maxx && lpObj->Y >= miny && lpObj->Y <= maxy)
		{
		}
		else
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(361));
			return;
		}
		if (lpTarget->Map == 2 && lpTarget->X >= minx && lpTarget->X <= maxx && lpTarget->Y >= miny && lpTarget->Y <= maxy)
		{
		}
		else
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(362));
			return;
		}
		int first_classtype = 0;
		if (
			(lpObj->DBClass == DB_CLASS_DW) || (lpObj->DBClass == DB_CLASS_SM) || (lpObj->DBClass == DB_CLASS_GM) || (lpObj->DBClass == DB_CLASS_DK) ||
			(lpObj->DBClass == DB_CLASS_BK) || (lpObj->DBClass == DB_CLASS_BM) || (lpObj->DBClass == DB_CLASS_MG) || (lpObj->DBClass == DB_CLASS_DM) ||
			(lpObj->DBClass == DB_CLASS_DL) || (lpObj->DBClass == DB_CLASS_LE) || (lpObj->DBClass == DB_CLASS_RF) || (lpObj->DBClass == DB_CLASS_FM)) {
			first_classtype = 0;
		}
		else if (
			(lpObj->DBClass == DB_CLASS_FE) || (lpObj->DBClass == DB_CLASS_ME) || (lpObj->DBClass == DB_CLASS_HE) || (lpObj->DBClass == DB_CLASS_SU) ||
			(lpObj->DBClass == DB_CLASS_BS) || (lpObj->DBClass == DB_CLASS_DS)) {
			first_classtype = 1;
		}
		int second_classtype = 0;
		if (
			(lpTarget->DBClass == DB_CLASS_DW) || (lpTarget->DBClass == DB_CLASS_SM) || (lpTarget->DBClass == DB_CLASS_GM) || (lpTarget->DBClass == DB_CLASS_DK) ||
			(lpTarget->DBClass == DB_CLASS_BK) || (lpTarget->DBClass == DB_CLASS_BM) || (lpTarget->DBClass == DB_CLASS_MG) || (lpTarget->DBClass == DB_CLASS_DM) ||
			(lpTarget->DBClass == DB_CLASS_DL) || (lpTarget->DBClass == DB_CLASS_LE) || (lpTarget->DBClass == DB_CLASS_RF) || (lpTarget->DBClass == DB_CLASS_FM)) {
			second_classtype = 0;
		}
		else if (
			(lpTarget->DBClass == DB_CLASS_FE) || (lpTarget->DBClass == DB_CLASS_ME) || (lpTarget->DBClass == DB_CLASS_HE) || (lpTarget->DBClass == DB_CLASS_SU) ||
			(lpTarget->DBClass == DB_CLASS_BS) || (lpTarget->DBClass == DB_CLASS_DS)) {
			second_classtype = 1;
		}
		char fulltext_new[256];
		wsprintf(fulltext_new, pMessage->Text(345), lpMsg->name, lpMsg->marryname);
		if ((first_classtype == 0) && (second_classtype == 0)) {
			if ((GetTickCount() - lpObj->MarryTimeMsj) > (DWORD)(gServerInfo.m_CommandMarryNoticeDelay * 1000))
			{
				GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, pMessage->Text(346));
				GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, fulltext_new);
			}
			lpObj->MarryTimeMsj = GetTickCount();
			return;
		}
		if ((first_classtype == 1) && (second_classtype == 1)) {
			if ((GetTickCount() - lpObj->MarryTimeMsj) > (DWORD)(gServerInfo.m_CommandMarryNoticeDelay * 1000))
			{
				GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, pMessage->Text(346));
				GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, fulltext_new);
			}
			lpObj->MarryTimeMsj = GetTickCount();
			return;
		}
		GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, pMessage->Text(344));
		GDMarryInfoSaveSend(lpObj->Index, lpMsg->marryname, lpMsg->mode);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(312), lpMsg->marryname);
		char fulltext[256];
		wsprintf(fulltext, pMessage->Text(342), lpObj->Name, lpMsg->marryname);
		GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, fulltext);
		for (int n = 0; n < 15; n++)
		{
			GCFireworksSend(lpObj, (lpObj->X + (((GetLargeRand() % 5) * 2) - 4)), (lpObj->Y + (((GetLargeRand() % 5) * 2) - 4)));
		}
		this->RemoveRequisites(lpObj, 30);
	}
	else if (strcmp(lpMsg->mode, "divorcerise") == 0)
	{
		if (lpMsg->countyou == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(339));
			return;
		}
		time_t timer;
		struct tm y2k = { 0 };
		UINT seconds;
		y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
		y2k.tm_year = 116; y2k.tm_mon = 0; y2k.tm_mday = 1;
		time(&timer);
		seconds = static_cast<UINT>(difftime(timer, mktime(&y2k)));
		UINT value = seconds - lpMsg->marriedon;
		if (value < (UINT)gServerInfo.m_CommandMarryMinTime)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(337));
			return;
		}
		LPOBJ lpTarget1 = gObjFind(lpMsg->NameGet1);
		LPOBJ lpTarget2 = gObjFind(lpMsg->NameGet2);
		if (lpTarget1 == 0 && lpTarget2 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(364));
			return;
		}
		char fulltext[256];
		GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, pMessage->Text(344));
		if (lpTarget1 == 0)
		{
			wsprintf(fulltext, pMessage->Text(343), lpObj->Name, lpMsg->NameGet2);
			GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, fulltext);
		}
		if (lpTarget2 == 0)
		{
			wsprintf(fulltext, pMessage->Text(343), lpObj->Name, lpMsg->NameGet1);
			GDGlobalNoticeSend(gMapServerManager.GetMapServerGroup(), 0, 0, 0, 0, 0, 0, fulltext);
		}
		GDMarryInfoSaveSend(lpObj->Index, lpMsg->marryname, lpMsg->mode);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(338));
	}
	else if (strcmp(lpMsg->mode, "track") == 0)
	{
		if (lpMsg->countyou == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(339));
			return;
		}
		LPOBJ lpTarget1 = gObjFind(lpMsg->NameGet1);
		LPOBJ lpTarget2 = gObjFind(lpMsg->NameGet2);
		if (lpTarget1 == 0 && lpTarget2 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(365));
			return;
		}
		if (lpObj->Level >= gServerInfo.m_CommandMarryLevel)
		{
			if (lpTarget1 == 0)
			{
				if ((lpTarget2->Map == MAP_ICARUS || lpTarget2->Map == MAP_KANTURU3) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 37)))
				{
					gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
					return;
				}
				if (lpTarget2->Map == MAP_BLOOD_CASTLE1
					|| lpTarget2->Map == MAP_BLOOD_CASTLE2
					|| lpTarget2->Map == MAP_BLOOD_CASTLE3
					|| lpTarget2->Map == MAP_BLOOD_CASTLE4
					|| lpTarget2->Map == MAP_BLOOD_CASTLE5
					|| lpTarget2->Map == MAP_BLOOD_CASTLE6
					|| lpTarget2->Map == MAP_BLOOD_CASTLE7
					|| lpTarget2->Map == MAP_CHAOS_CASTLE1
					|| lpTarget2->Map == MAP_CHAOS_CASTLE2
					|| lpTarget2->Map == MAP_CHAOS_CASTLE3
					|| lpTarget2->Map == MAP_CHAOS_CASTLE4
					|| lpTarget2->Map == MAP_CHAOS_CASTLE5
					|| lpTarget2->Map == MAP_CHAOS_CASTLE6
					|| lpTarget2->Map == MAP_KALIMA1
					|| lpTarget2->Map == MAP_KALIMA2
					|| lpTarget2->Map == MAP_KALIMA3
					|| lpTarget2->Map == MAP_KALIMA4
					|| lpTarget2->Map == MAP_KALIMA5
					|| lpTarget2->Map == MAP_KALIMA6
					|| lpTarget2->Map == MAP_KALIMA7
					|| lpTarget2->Map == MAP_DEVIL_SQUARE1
					|| lpTarget2->Map == MAP_DEVIL_SQUARE2)
				{
					gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
					return;
				}
				gObjTeleport(lpObj->Index, lpTarget2->Map, lpTarget2->X, lpTarget2->Y);
				gObjTeleport(lpTarget2->Index, lpTarget2->Map, lpTarget2->X, lpTarget2->Y);
				gObjViewportListProtocolDestroy(lpObj);
				gObjClearViewport(lpObj);
			}
			if (lpTarget2 == 0)
			{
				if ((lpTarget1->Map == MAP_ICARUS || lpTarget1->Map == MAP_KANTURU3) && (lpObj->Inventory[7].IsItem() == 0 && lpObj->Inventory[8].m_Index != GET_ITEM(13, 3) && lpObj->Inventory[8].m_Index != GET_ITEM(13, 37)))
				{
					gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
					return;
				}
				if (lpTarget1->Map == MAP_BLOOD_CASTLE1
					|| lpTarget1->Map == MAP_BLOOD_CASTLE2
					|| lpTarget1->Map == MAP_BLOOD_CASTLE3
					|| lpTarget1->Map == MAP_BLOOD_CASTLE4
					|| lpTarget1->Map == MAP_BLOOD_CASTLE5
					|| lpTarget1->Map == MAP_BLOOD_CASTLE6
					|| lpTarget1->Map == MAP_BLOOD_CASTLE7
					|| lpTarget1->Map == MAP_CHAOS_CASTLE1
					|| lpTarget1->Map == MAP_CHAOS_CASTLE2
					|| lpTarget1->Map == MAP_CHAOS_CASTLE3
					|| lpTarget1->Map == MAP_CHAOS_CASTLE4
					|| lpTarget1->Map == MAP_CHAOS_CASTLE5
					|| lpTarget1->Map == MAP_CHAOS_CASTLE6
					|| lpTarget1->Map == MAP_KALIMA1
					|| lpTarget1->Map == MAP_KALIMA2
					|| lpTarget1->Map == MAP_KALIMA3
					|| lpTarget1->Map == MAP_KALIMA4
					|| lpTarget1->Map == MAP_KALIMA5
					|| lpTarget1->Map == MAP_KALIMA6
					|| lpTarget1->Map == MAP_KALIMA7
					|| lpTarget1->Map == MAP_DEVIL_SQUARE1
					|| lpTarget1->Map == MAP_DEVIL_SQUARE2)
				{
					gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
					return;
				}
				gObjTeleport(lpObj->Index, lpTarget1->Map, lpTarget1->X, lpTarget1->Y);
				gObjTeleport(lpTarget1->Index, lpTarget1->Map, lpTarget1->X, lpTarget1->Y);
				gObjViewportListProtocolDestroy(lpObj);
				gObjClearViewport(lpObj);
			}
		}
		else
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(347), gServerInfo.m_CommandMarryLevel);
			return;
		}
		GDMarryInfoSaveSend(lpObj->Index, lpMsg->marryname, lpMsg->mode);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(340), lpMsg->marryname);
		lpObj->Money -= gServerInfo.m_CommandMarryCost;
		GCMoneySend(lpObj->Index, lpObj->Money);
	}
	else if (strcmp(lpMsg->mode, "trace") == 0)
	{
		if (lpMsg->countyou == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(339));
			return;
		}
		LPOBJ lpTarget1 = gObjFind(lpMsg->NameGet1);
		LPOBJ lpTarget2 = gObjFind(lpMsg->NameGet2);
		if (lpTarget1 == 0 && lpTarget2 == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(365));
			return;
		}
		if (lpObj->Map == MAP_BLOOD_CASTLE1
			|| lpObj->Map == MAP_BLOOD_CASTLE2
			|| lpObj->Map == MAP_BLOOD_CASTLE3
			|| lpObj->Map == MAP_BLOOD_CASTLE4
			|| lpObj->Map == MAP_BLOOD_CASTLE5
			|| lpObj->Map == MAP_BLOOD_CASTLE6
			|| lpObj->Map == MAP_BLOOD_CASTLE7
			|| lpObj->Map == MAP_CHAOS_CASTLE1
			|| lpObj->Map == MAP_CHAOS_CASTLE2
			|| lpObj->Map == MAP_CHAOS_CASTLE3
			|| lpObj->Map == MAP_CHAOS_CASTLE4
			|| lpObj->Map == MAP_CHAOS_CASTLE5
			|| lpObj->Map == MAP_CHAOS_CASTLE6
			|| lpObj->Map == MAP_KALIMA1
			|| lpObj->Map == MAP_KALIMA2
			|| lpObj->Map == MAP_KALIMA3
			|| lpObj->Map == MAP_KALIMA4
			|| lpObj->Map == MAP_KALIMA5
			|| lpObj->Map == MAP_KALIMA6
			|| lpObj->Map == MAP_KALIMA7
			|| lpObj->Map == MAP_DEVIL_SQUARE1
			|| lpObj->Map == MAP_DEVIL_SQUARE2)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
			return;
		}
		if (lpTarget1 == 0)
		{
			if ((lpObj->Map == MAP_ICARUS || lpObj->Map == MAP_KANTURU3) && (lpTarget2->Inventory[7].IsItem() == 0 && lpTarget2->Inventory[8].m_Index != GET_ITEM(13, 3) && lpTarget2->Inventory[8].m_Index != GET_ITEM(13, 37)))
			{
				gNotice.GCNoticeSend(lpTarget2->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
				return;
			}
			if (lpTarget2->Level >= gServerInfo.m_CommandMarryLevel)
			{
				gObjTeleport(lpTarget2->Index, lpObj->Map, lpObj->X, lpObj->Y);
				gObjTeleport(lpObj->Index, lpObj->Map, lpObj->X, lpObj->Y);
			}
			else
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(348), gServerInfo.m_CommandMarryLevel);
				return;
			}
		}
		if (lpTarget2 == 0)
		{
			if ((lpObj->Map == MAP_ICARUS || lpObj->Map == MAP_KANTURU3) && (lpTarget1->Inventory[7].IsItem() == 0 && lpTarget1->Inventory[8].m_Index != GET_ITEM(13, 3) && lpTarget1->Inventory[8].m_Index != GET_ITEM(13, 37)))
			{
				gNotice.GCNoticeSend(lpTarget1->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(251));
				return;
			}
			if (lpTarget1->Level >= gServerInfo.m_CommandMarryLevel)
			{
				gObjTeleport(lpTarget1->Index, lpObj->Map, lpObj->X, lpObj->Y);
				gObjClearViewport(lpObj);
			}
			else
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(348), gServerInfo.m_CommandMarryLevel);
				return;
			}
		}
		GDMarryInfoSaveSend(lpObj->Index, lpMsg->marryname, lpMsg->mode);
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(341), lpMsg->marryname);
		lpObj->Money -= gServerInfo.m_CommandMarryCost;
		GCMoneySend(lpObj->Index, lpObj->Money);
	}
	gLog.Output(LOG_COMMAND, "[CommandMarry][%s][%d][%d][%d][%s][%s][%s]", lpObj->Account, lpMsg->countyou, lpMsg->counthim, lpMsg->marriedon, lpMsg->marryname, lpMsg->NameGet1, lpMsg->NameGet2);
}
bool CCommandManager::CommandOpenWarehouse(LPOBJ lpObj)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX || lpObj->Interface.type == INTERFACE_TRADE || lpObj->Interface.type == INTERFACE_PERSONAL_SHOP)
	{
		return 0;
	}
	if (gItemManager.ChaosBoxHasItem(lpObj) || gItemManager.TradeHasItem(lpObj))
	{
		return 0;
	}
	if (gServerInfo.m_CommandOpenWareOnlySafeZone == 1 && gMap[lpObj->Map].CheckAttr(lpObj->X, lpObj->Y, 1) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(351));
		return 0;
	}
	lpObj->Interface.use = 1;
	lpObj->Interface.type = INTERFACE_WAREHOUSE;
	lpObj->Interface.state = 0;
	gWarehouse.GDWarehouseItemSend(lpObj->Index, lpObj->Account);
	return 1;
}
bool CCommandManager::CommandReward(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	if (name[0] == NULL) {
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(405));
		return 0;
	}
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(404));
		return 0;
	}
	if (strcmp(lpTarget->Name, lpObj->Name) == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(409));
		return 0;
	}
	int type = this->GetNumber(arg, 1);
	int value = this->GetNumber(arg, 2);
	if (type <= 0 || value <= 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(405));
		return 0;
	}
	if (type > gServerInfo.m_CommandRewardMaxType)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(406));
		return 0;
	}
	if (value > gServerInfo.m_CommandRewardMaxValue)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(407));
		return 0;
	}
	SDHP_COMMAND_REWARD_SEND pMsg;
	pMsg.header.set(0x0F, 0x03, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.name, lpTarget->Name, sizeof(pMsg.name));
	memcpy(pMsg.account, lpTarget->Account, sizeof(pMsg.account));
	memcpy(pMsg.nameGM, lpObj->Name, sizeof(pMsg.nameGM));
	memcpy(pMsg.accountGM, lpObj->Account, sizeof(pMsg.accountGM));
	pMsg.Type = type;
	pMsg.Value = value;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	if (type == 1) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType1Text, lpObj->Name, lpTarget->Name, value);
	if (type == 2) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType2Text, lpObj->Name, lpTarget->Name, value);
	if (type == 3) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType3Text, lpObj->Name, lpTarget->Name, value);
	if (type == 4) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType4Text, lpObj->Name, lpTarget->Name, value);
	if (type == 5) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType5Text, lpObj->Name, lpTarget->Name, value);
	if (type == 6) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType6Text, lpObj->Name, lpTarget->Name, value);
	if (type == 7) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType7Text, lpObj->Name, lpTarget->Name, value);
	if (type == 8) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType8Text, lpObj->Name, lpTarget->Name, value);
	if (type == 9) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType9Text, lpObj->Name, lpTarget->Name, value);
	if (type == 10) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardType10Text, lpObj->Name, lpTarget->Name, value);
	gCashShop.CGCashShopPointRecv(lpTarget->Index);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(408));
	gLog.Output(LOG_COMMAND, "[CommandReward][%s][%s] - (Player: %s, Type: %d, Value: %d)", lpObj->Account, lpObj->Name, lpTarget->Name, type, value);
	return 1;
}
bool CCommandManager::CommandRewardAll(LPOBJ lpObj, char* arg)
{
	int type = this->GetNumber(arg, 0);
	int value = this->GetNumber(arg, 1);
	if (type <= 0 || value <= 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(412));
		return 0;
	}
	if (type > gServerInfo.m_CommandRewardAllMaxType)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(406));
		return 0;
	}
	if (value > gServerInfo.m_CommandRewardAllMaxValue)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(407));
		return 0;
	}
	SDHP_COMMAND_REWARDALL_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x04, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.nameGM, lpObj->Name, sizeof(pMsg.nameGM));
	memcpy(pMsg.accountGM, lpObj->Account, sizeof(pMsg.accountGM));
	pMsg.Type = type;
	pMsg.Value = value;
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	if (type == 1) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType1Text, lpObj->Name, value);
	if (type == 2) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType2Text, lpObj->Name, value);
	if (type == 3) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType3Text, lpObj->Name, value);
	if (type == 4) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType4Text, lpObj->Name, value);
	if (type == 5) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType5Text, lpObj->Name, value);
	if (type == 6) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType6Text, lpObj->Name, value);
	if (type == 7) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType7Text, lpObj->Name, value);
	if (type == 8) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType8Text, lpObj->Name, value);
	if (type == 9) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType9Text, lpObj->Name, value);
	if (type == 10) gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, gServerInfo.m_CommandRewardAllType10Text, lpObj->Name, value);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(408));
	gLog.Output(LOG_COMMAND, "[CommandRewardAll][%s][%s] - (Type: %d, Value: %d)", lpObj->Account, lpObj->Name, type, value);
	return 1;
}
bool CCommandManager::CommandReload(LPOBJ lpObj, char* arg)
{
	char name[20] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	if (strcmp(name, "cashshop") == 0)
	{
		gServerInfo.ReadCashShopInfo();
	}
	else if (strcmp(name, "chaosmix") == 0)
	{
		gServerInfo.ReadChaosMixInfo();
	}
	else if (strcmp(name, "character") == 0)
	{
		gServerInfo.ReadCharacterInfo();
	}
	else if (strcmp(name, "command") == 0)
	{
		gServerInfo.ReadCommandInfo();
	}
	else if (strcmp(name, "common") == 0)
	{
		gServerInfo.ReadCommonInfo();
	}
	else if (strcmp(name, "custom") == 0)
	{
		gServerInfo.ReadCustomInfo();
	}
	else if (strcmp(name, "event") == 0)
	{
		gServerInfo.ReadEventInfo();
	}
	else if (strcmp(name, "eventitembag") == 0)
	{
		gServerInfo.ReadEventItemBagInfo();
	}
	else if (strcmp(name, "hack") == 0)
	{
		gServerInfo.ReadHackInfo();
	}
	else if (strcmp(name, "item") == 0)
	{
		gServerInfo.ReadItemInfo();
	}
	else if (strcmp(name, "monster") == 0)
	{
		gServerInfo.ReloadMonsterInfo();
	}
	else if (strcmp(name, "move") == 0)
	{
		gServerInfo.ReadMoveInfo();
	}
	else if (strcmp(name, "quest") == 0)
	{
		gServerInfo.ReadQuestInfo();
	}
	else if (strcmp(name, "shop") == 0)
	{
		gServerInfo.ReadShopInfo();
	}
	else if (strcmp(name, "skill") == 0)
	{
		gServerInfo.ReadSkillInfo();
	}
	else if (strcmp(name, "util") == 0)
	{
		gServerInfo.ReadUtilInfo();
	}
	else if (strcmp(name, "bots") == 0)
	{
		gServerInfo.ReloadBotInfo();
	}
	else if (strcmp(name, "rise") == 0)
	{
		gServerInfo.ReloadCfgRISE();
	}
	else if (strcmp(name, "all") == 0)
	{
		gServerInfo.ReloadAll();
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(410));
		return 0;
	}
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(411));
	return 1;
}
bool IsAllowedCharacter(char c)
{
	return (std::isalpha(c) != 0) || (std::isdigit(c) != 0);
}

bool CCommandManager::CommandRename(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(444));
		return 0;
	}

	if (lpObj->GuildNumber > 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(431));
		return 0;
	}

	if (gServerInfo.m_CommandRenameTicket == 1 && lpObj->RenameEnable == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(414));
		return 0;
	}

	DWORD tick = (GetTickCount() - lpObj->RenameTime) / 1000;
	if (tick < ((DWORD)gServerInfo.m_CommandRenameDelay[lpObj->AccountLevel]))
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(415), (gServerInfo.m_CommandRenameDelay[lpObj->AccountLevel] - tick));
		return 0;
	}

	if (strlen(arg) > 10 || strlen(arg) < 3)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(413));
		return 0;
	}

	for (size_t i = 0; i < strlen(arg); ++i)
	{
		if (!IsAllowedCharacter(arg[i]))
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(617));
			return false;
		}
	}

	char RenameName[11] = { 0 };
	this->GetString(arg, RenameName, sizeof(RenameName), 0);

	if (RenameName[0] == NULL)
	{
		return 0;
	}
	if (strcmp(RenameName, lpObj->Name) == 0)
	{
		return 0;
	}

	lpObj->RenameActive = 1;

	SDHP_COMMAND_RENAME_SEND pMsg;
	pMsg.header.set(0x0F, 0x05, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	memcpy(pMsg.name, lpObj->Name, sizeof(pMsg.name));
	memcpy(pMsg.newname, RenameName, sizeof(pMsg.newname));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	gLog.Output(LOG_COMMAND, "[CommandRename][%s][%s] - (NewName: %s)", lpObj->Account, lpObj->Name, RenameName);

	return 0;
}
void CCommandManager::DGCommandRenameRecv(SDHP_COMMAND_RENAME_RECV* lpMsg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 2)
	if (gObjIsAccountValid(lpMsg->index, lpMsg->account) == 0)
	{
		LogAdd(LOG_RED, "[DGCommandRenameRecv] Invalid Account [%d](%s)", lpMsg->index, lpMsg->account);
		CloseClient(lpMsg->index);
		return;
	}
	LPOBJ lpObj = &gObj[lpMsg->index];
	if (lpMsg->result == 0)
	{
		this->RemoveRequisites(lpObj, 38);
		lpObj->RenameTime = GetTickCount();
		memcpy(lpObj->Name, lpMsg->newname, sizeof(lpObj->Name));
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, pMessage->Text(430), lpMsg->name, lpMsg->newname);
		lpObj->RenameActive = 0;
		lpObj->CloseCount = 1;
		lpObj->CloseType = 1;
		LogAdd(LOG_GREEN, "[CommandRename][%s][%s] - Sucess (OldName: %s)", lpObj->Account, lpObj->Name, lpMsg->name);
		return;
	}
	if (lpMsg->result == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(428));
		LogAdd(LOG_BLACK, "[CommandRename][%s][%s] - Name exists (NewName: %s)", lpObj->Account, lpObj->Name, lpMsg->newname);
		return;
	}
	if (lpMsg->result == 2)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(429));
		LogAdd(LOG_RED, "[CommandRename][%s][%s] - Error (NewName: %s)", lpObj->Account, lpObj->Name, lpMsg->newname);
		return;
	}
#endif
}
bool CCommandManager::CommandInfo(LPOBJ lpObj, char* arg)
{
	if (lpObj->Interface.type == INTERFACE_CHAOS_BOX
		|| lpObj->Interface.type == INTERFACE_TRADE
		|| lpObj->Interface.type == INTERFACE_PARTY
		|| lpObj->Interface.type == INTERFACE_WAREHOUSE
		|| lpObj->Interface.type == INTERFACE_PERSONAL_SHOP
		|| lpObj->Interface.type == INTERFACE_CASH_SHOP
		|| lpObj->Interface.type == INTERFACE_TRAINER
		|| lpObj->Interface.use != 0
		|| lpObj->State == 32
		|| lpObj->DieRegen != 0
		|| lpObj->Teleport != 0
		|| lpObj->PShopOpen != 0
		|| lpObj->ChaosLock != 0
		|| lpObj->SkillSummonPartyTime != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(46));
		return 0;
	}
#if (GAMESERVER_CLIENTE_UPDATE >= 2)
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(175));
		return 0;
	}
	if (gGameMaster.CheckGameMasterLevel(lpObj, 1) == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(766), lpTarget->Account);
	}
	GJAccountLevelSend(lpObj->Index, lpTarget->Account);
	gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(767), lpTarget->Level);
	gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(768), lpTarget->Reset);
	gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(769), lpTarget->MasterReset);
#endif
	return 1;
}
bool CCommandManager::CommandBlocChar(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	int days = this->GetNumber(arg, 1);
	if (days <= 0)
	{
		return 0;
	}
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget > 0)
	{
		gObjUserKill(lpTarget->Index);
	}
	SDHP_COMMAND_BLOC_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x07, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	pMsg.days = days;
	memcpy(pMsg.namebloc, name, sizeof(pMsg.namebloc));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	if (gServerInfo.m_CommandBlocCharShowMessage == 1)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, pMessage->Text(641), lpObj->Name, name, days);
	}
	gLog.Output(LOG_COMMAND, "[CommandBlocChar][%s][%s] - (Name: %s)", lpObj->Account, lpObj->Name, name);
	return 1;
}
bool CCommandManager::CommandBlocCharRecv(SDHP_COMMAND_BLOC_RECV* lpMsg)
{
	if (lpMsg->result != 1)
	{
		gNotice.GCNoticeSend(lpMsg->index, 1, 0, 0, 0, 0, 0, pMessage->Text(446));
	}
	else
	{
		gNotice.GCNoticeSend(lpMsg->index, 1, 0, 0, 0, 0, 0, pMessage->Text(454));
	}
	return 1;
}
bool CCommandManager::CommandBlocAcc(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	int days = this->GetNumber(arg, 1);
	if (days <= 0)
	{
		return 0;
	}
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget > 0)
	{
		gObjUserKill(lpTarget->Index);
	}
	SDHP_COMMAND_BLOC_SEND pMsg{};
	pMsg.header.set(0x0F, 0x06, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	pMsg.days = days;
	memcpy(pMsg.namebloc, name, sizeof(pMsg.namebloc));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	if (gServerInfo.m_CommandBlocAccShowMessage == 1)
	{
		gNotice.GCNoticeSendToAll(0, 0, 0, 0, 0, 0, pMessage->Text(642), lpObj->Name, name, days);
	}
	gLog.Output(LOG_COMMAND, "[CommandBlocAcc][%s][%s] - (Acc: %s)", lpObj->Account, lpObj->Name, name);
	return 1;
}
bool CCommandManager::CommandBlocAccRecv(SDHP_COMMAND_BLOC_RECV* lpMsg)
{
	if (lpMsg->result != 1)
	{
		gNotice.GCNoticeSend(lpMsg->index, 1, 0, 0, 0, 0, 0, pMessage->Text(446));
	}
	else
	{
		gNotice.GCNoticeSend(lpMsg->index, 1, 0, 0, 0, 0, 0, pMessage->Text(447));
	}
	return 1;
}
bool CCommandManager::CommandGift(LPOBJ lpObj, char* arg)
{
	SDHP_GIFT_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x08, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	memcpy(pMsg.account, lpObj->Account, sizeof(pMsg.account));
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	return 0;
}
void CCommandManager::DGCommandGiftRecv(SDHP_GIFT_RECV* lpMsg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 3)
	LPOBJ lpObj = &gObj[lpMsg->index];
	if (lpMsg->result > 0)
	{
		if (lpMsg->result > gServerInfo.m_CommandGiftLimit)
		{
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(503));
		}
		else
		{
			gItemBagManager.DropItemBySpecialValue(ITEM_BAG_GIFT, lpObj, lpObj->Map, lpObj->X, lpObj->Y);
			GCFireworksSend(lpObj, lpObj->X, lpObj->Y);
			LogAdd(LOG_EVENT, "[CustomGift][%s][%s] GiftNumber: %d", lpObj->Account, lpObj->Name, lpMsg->result);
			gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(502));
			this->RemoveRequisites(lpObj, 49);
		}
	}
	else
	{
		LogAdd(LOG_RED, "[CustomGift][%s][%s] Gift Error", lpObj->Account, lpObj->Name);
	}
#endif
}
bool CCommandManager::CommandTop(LPOBJ lpObj, char* arg)
{
	SDHP_TOP_SEND pMsg;
	memset(&pMsg, 0, sizeof(pMsg));
	pMsg.header.set(0x0F, 0x09, sizeof(pMsg));
	pMsg.index = lpObj->Index;
	char tipo[10] = { 0 };
	char classe[3] = { 0 };
	this->GetString(arg, tipo, sizeof(tipo), 0);
	this->GetString(arg, classe, sizeof(classe), 1);
	if (strcmp(classe, "dw") == 0)
	{
		pMsg.classe = 0;
	}
	else if (strcmp(classe, "dk") == 0)
	{
		pMsg.classe = 16;
	}
	else if (strcmp(classe, "fe") == 0)
	{
		pMsg.classe = 32;
	}
	else if (strcmp(classe, "mg") == 0)
	{
		pMsg.classe = 48;
	}
	else if (strcmp(classe, "dl") == 0)
	{
		pMsg.classe = 64;
	}
	else if (strcmp(classe, "su") == 0)
	{
		pMsg.classe = 80;
	}
	else if (strcmp(classe, "rf") == 0)
	{
		pMsg.classe = 96;
	}
	else
	{
		pMsg.classe = 999;
	}
	CUSTOMTOP_INFO CustomTopInfo;
	if (gCustomTop.GetInfoByName(tipo, &CustomTopInfo) != 0)
	{
		pMsg.type = CustomTopInfo.Index;
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(533));
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(534));
		return 0;
	}
	gDataServerConnection.DataSend((BYTE*)&pMsg, pMsg.header.size);
	return 1;
}
void CCommandManager::DGCommandTopRecv(SDHP_TOP_RECV* lpMsg)
{
	LPOBJ lpObj = &gObj[lpMsg->index];
	CUSTOMTOP_INFO CustomTopInfo;
	if (gCustomTop.GetInfo(lpMsg->type, &CustomTopInfo) != 0)
	{
		if (lpMsg->classe == 0)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(770), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 16)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(771), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 32)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(772), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 48)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(773), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 64)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(774), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 80)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(775), CustomTopInfo.Title);
		}
		else if (lpMsg->classe == 96)
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(776), CustomTopInfo.Title);
		}
		else
		{
			gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, "%s", CustomTopInfo.Title);
		}
	}
	else
	{
		return;
	}

	if (lpMsg->count <= 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(535));
		return;
	}
	for (int n = 0; n < lpMsg->count; n++)
	{
		SDHP_TOP_INFO* lpInfo = (SDHP_TOP_INFO*)(((BYTE*)lpMsg) + sizeof(SDHP_TOP_RECV) + (sizeof(SDHP_TOP_INFO) * n));
		gNotice.GCNoticeSend(lpObj->Index, 0, 0, 0, 0, 0, 0, pMessage->Text(536), n + 1, lpInfo->name, lpInfo->value);
	}
}
bool CCommandManager::CommandReAdd(LPOBJ lpObj, char* arg, int Npc)
{
	char name[4] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	if (lpObj->Interface.use != 0 || lpObj->Teleport != 0 || lpObj->DieRegen != 0 || lpObj->PShopOpen != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(444));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(444));
		}
		return 0;
	}
	int Resto = 0;
	if (strcmp(name, "") == 0)
	{
		if (gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel] != 0)
		{
			for (int n = 0; n < 4; n++) {
				Resto += gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, n);
			}
			lpObj->LevelUpPoint += (lpObj->Strength + lpObj->Dexterity + lpObj->Vitality + lpObj->Energy - Resto);
			lpObj->Strength = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 0);
			lpObj->Dexterity = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 1);
			lpObj->Vitality = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 2);
			lpObj->Energy = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 3);
		}
		else
		{
			for (int n = 0; n < 5; n++) {
				Resto += gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, n);
			}
			lpObj->LevelUpPoint += (lpObj->Strength + lpObj->Dexterity + lpObj->Vitality + lpObj->Energy + lpObj->Leadership - Resto);
			lpObj->Strength = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 0);
			lpObj->Dexterity = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 1);
			lpObj->Vitality = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 2);
			lpObj->Energy = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 3);
			lpObj->Leadership = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 4);
		}
	}
	else
	{
		if (strcmp(name, "str") == 0)
		{
			lpObj->LevelUpPoint += (lpObj->Strength) - gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 0);
			lpObj->Strength = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 0);
		}
		else if (strcmp(name, "agi") == 0)
		{
			lpObj->LevelUpPoint += (lpObj->Dexterity) - gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 1);
			lpObj->Dexterity = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 1);
		}
		else if (strcmp(name, "vit") == 0)
		{
			lpObj->LevelUpPoint += (lpObj->Vitality) - gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 2);
			lpObj->Vitality = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 2);
		}
		else if (strcmp(name, "ene") == 0)
		{
			lpObj->LevelUpPoint += (lpObj->Energy) - gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 3);
			lpObj->Energy = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 3);
		}
		else if (strcmp(name, "cmd") == 0)
		{
			if (gServerInfo.m_CommandResetKeepDLCommandPoint[lpObj->AccountLevel] == 1)
			{
				gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(576));
				return 0;
			}
			else
			{
				lpObj->LevelUpPoint += (lpObj->Leadership) - gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 4);
				lpObj->Leadership = gDefaultClassInfo.GetCharacterDefaultStat(lpObj->Class, 4);
			}
		}
	}
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(537));
	gEffectManager.ClearAllEffect(lpObj);
	if (gServerInfo.ResetPointCharOut)
	{
		lpObj->CloseCount = 1;
		lpObj->CloseType = 1;
	}
	gLog.Output(LOG_COMMAND, "[CommandReAdd][%s] [%s] - OK", lpObj->Account, lpObj->Name);
	return 1;
}
bool CCommandManager::CommandHelper(LPOBJ lpObj, char* arg, int Npc)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 6)
	if (lpObj->Map == MAP_ICARUS || CC_MAP_RANGE(lpObj->Map) != 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(444));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(444));
		}
		return 0;
	}
	if (OBJECT_RANGE(lpObj->SummonIndex) != 0)
	{
		gObjSummonKill(lpObj->Index);
		GCSummonLifeSend(lpObj->Index, 0, 1);
		return 0;
	}
	int index;
	for (int i = 1; i <= gServerInfo.m_CommandHelpMonsterAmount; i++)
	{
		index = gObjAddSummon();
		if (OBJECT_RANGE(index) == 0)
		{
			return 0;
		}
		lpObj->SummonIndex = index;
		LPOBJ lpSummon = &gObj[index];
		lpSummon->X = lpObj->X - 1;
		lpSummon->Y = lpObj->Y + i;
		lpSummon->MTX = lpSummon->X;
		lpSummon->MTY = lpSummon->Y;
		lpSummon->Dir = 2;
		lpSummon->Map = lpObj->Map;
		gObjSetMonster(index, gServerInfo.m_CommandHelpMonsterIndex);
		lpSummon->Life += (float)((__int64)lpSummon->Life * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
		lpSummon->MaxLife += (float)((__int64)lpSummon->MaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
		lpSummon->ScriptMaxLife += (float)((__int64)lpSummon->ScriptMaxLife * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_LIFE)) / 100;
		lpSummon->Defense += ((__int64)lpSummon->Defense * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DEFENSE)) / 100;
		lpSummon->PhysiDamageMin += ((__int64)lpSummon->PhysiDamageMin * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;
		lpSummon->PhysiDamageMax += ((__int64)lpSummon->PhysiDamageMax * gMasterSkillTree.GetMasterSkillValue(lpObj, MASTER_SKILL_ADD_SUMMON_DAMAGE)) / 100;
		lpSummon->SummonIndex = lpObj->Index;
		lpSummon->Attribute = 100;
		lpSummon->TargetNumber = -1;
		lpSummon->ActionState.Attack = 0;
		lpSummon->ActionState.Emotion = 0;
		lpSummon->ActionState.EmotionCount = 0;
		lpSummon->PathCount = 0;
		lpSummon->MoveRange = 15;
		GCSummonLifeSend(lpSummon->SummonIndex, (int)lpSummon->Life, (int)lpSummon->MaxLife);
	}
	gLog.Output(LOG_COMMAND, "[CommandHelper][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandReMaster(LPOBJ lpObj, char* arg, int Npc)
{
	if (gObjRebuildMasterSkillTree(lpObj) == 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(555));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(555));
		}
		gLog.Output(LOG_COMMAND, "[CommandReMaster][%s][%s] - Used", lpObj->Account, lpObj->Name);
		return 1;
	}
	else
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(556));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(556));
		}
		return 0;
	}
	return 1;
}
bool CCommandManager::CommandStartBC(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 6)
	gBloodCastle.StartBC();
	gLog.Output(LOG_COMMAND, "[CommandStartBC][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandStartDS(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 6)
	gDevilSquare.StartDS();
	gLog.Output(LOG_COMMAND, "[CommandStartDS][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandStartCC(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 6)
	gChaosCastle.StartCC();
	gLog.Output(LOG_COMMAND, "[CommandStartCC][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandStartIT(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 6)
	gIllusionTemple.StartIT();
	gLog.Output(LOG_COMMAND, "[CommandStartIT][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
#if EVENT_DROP
bool CCommandManager::CommandStartDrop(LPOBJ lpObj, char* arg)
{
	gCustomEventDrop.StartDrop();
	gLog.Output(LOG_COMMAND, "[CommandStartDrop][%s][%s] - Used", lpObj->Account, lpObj->Name);
	return 1;
}
#endif
bool CCommandManager::CommandStartKing(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 9)
	gReiDoMU.StartKing();
	gLog.Output(LOG_COMMAND, "[CommandStartKingOfMu][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandStartInvasion(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 13)
	int index = this->GetNumber(arg, 0);
	if (index <= 0)
	{
		return 0;
	}
	gInvasionManager.StartInvasion(index);
	gLog.Output(LOG_COMMAND, "[CommandStartInvasion][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
	return 1;
}
bool CCommandManager::CommandQuyetDau(LPOBJ lpObj, char* arg)
{
	return 1;
}
bool CCommandManager::CommandQuyetDauOK(LPOBJ lpObj, char* arg)
{
	return 1;
}
bool CCommandManager::CommandDisablePvp(LPOBJ lpObj, char* arg, int Npc)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 7)
	if (lpObj->DisablePvp == 1)
	{
		lpObj->DisablePvp = 0;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(672));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(672));
		}
		gCustomAttack.DGCustomAttackResumeSaveSend(lpObj->Index);
		return 0;
	}
	else
	{
		lpObj->DisablePvp = 1;
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(671));
		if (Npc >= 0)
		{
			GCChatTargetNewSend(lpObj, Npc, pMessage->Text(671));
		}
		gLog.Output(LOG_COMMAND, "[CommandDisablePvP][%s][%s] - Used", lpObj->Account, lpObj->Name);
		gCustomAttack.DGCustomAttackResumeSaveSend(lpObj->Index);
		return 1;
	}
#endif
	return 0;
}
bool CCommandManager::CommandLock(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 7)
	if (lpObj->Lock > 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(675));
		return 0;
	}
	int password = this->GetNumber(arg, 0);
	if (password <= 0)
	{
		return 0;
	}
	if (password > 99999999)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(674));
		return 0;
	}
	lpObj->Lock = password;
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(673));
	gLog.Output(LOG_COMMAND, "[CommandLock][%s][%s] - Password: %d", lpObj->Account, lpObj->Name, password);
	GJAccountLockSaveSend(lpObj->Index, password);
	return 1;
#endif
	return 0;
}
bool CCommandManager::CommandUnLock(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 7)
	if (lpObj->Lock <= 0)
	{
		return 0;
	}
	int password = this->GetNumber(arg, 0);
	if (password <= 0)
	{
		return 0;
	}
	if (password > 99999999)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(674));
		return 0;
	}
	if (password != lpObj->Lock)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(677));
		return 0;
	}
	lpObj->Lock = 0;
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(676));
	gLog.Output(LOG_COMMAND, "[CommandUnLock][%s][%s] - Password: %d", lpObj->Account, lpObj->Name, password);
	GJAccountLockSaveSend(lpObj->Index, 0);
	return 1;
#endif
	return 0;
}
bool CCommandManager::CommandMoveAll(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 8)
	int map = this->GetNumber(arg, 0);
	int x = this->GetNumber(arg, 1);
	int y = this->GetNumber(arg, 2);
	for (int n = OBJECT_START_USER; n < MAX_OBJECT; n++)
	{
		if (gObjIsConnectedGP(n) != 0 && gObj[n].Index != lpObj->Index)
		{
			if (gObj[n].Connected > OBJECT_LOGGED)
			{
				gObjTeleport(gObj[n].Index, map, x, y);
			}
		}
	}
	gLog.Output(LOG_COMMAND, "[CommandMoveAll][%s][%s] - (Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, map, x, y);
#endif
	return 1;
}
bool CCommandManager::CommandMoveGuild(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 8)
	char name[9] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	int map = this->GetNumber(arg, 1);
	int x = this->GetNumber(arg, 2);
	int y = this->GetNumber(arg, 3);
	GUILD_INFO_STRUCT* lpGuild = gGuildClass.SearchGuild(name);
	if (lpGuild == NULL)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(698));
		return 0;
	}
	for (int n = 0; n < MAX_GUILD_USER; n++)
	{
		int iGuildMemberIndex = lpGuild->Index[n];
		if (lpGuild->Use[n] > 0 && iGuildMemberIndex != -1)
		{
			LPOBJ lpObj = &gObj[iGuildMemberIndex];
			if (lpObj == NULL)
			{
				continue;
			}
			if (gObjIsConnected(iGuildMemberIndex) == 0)
			{
				continue;
			}
			gObjTeleport(iGuildMemberIndex, map, x, y);
		}
	}
	gLog.Output(LOG_COMMAND, "[CommandMoveGuild][%s][%s] - (Guild: %s, Map: %d, X: %d, Y: %d)", lpObj->Account, lpObj->Name, name, map, x, y);
#endif
	return 1;
}
bool CCommandManager::CommandSpot(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 9)
	int monster = this->GetNumber(arg, 0);
	int qtd = this->GetNumber(arg, 1);
	qtd = (qtd > 0) ? qtd : 1;
	for (int n = 0; n < qtd; n++)
	{
		int index = gObjAddMonster(lpObj->Map);
		if (OBJECT_RANGE(index) == 0)
		{
			return 0;
		}
		LPOBJ lpMonster = &gObj[index];
		int px = lpObj->X;
		int py = lpObj->Y;
		if (gObjGetRandomFreeLocation(lpObj->Map, &px, &py, 3, 3, 50) == 0)
		{
			return 0;
		}
		lpMonster->PosNum = -1;
		lpMonster->X = px;
		lpMonster->Y = py;
		lpMonster->TX = px;
		lpMonster->TY = py;
		lpMonster->OldX = px;
		lpMonster->OldY = py;
		lpMonster->StartX = px;
		lpMonster->StartY = py;
		lpMonster->Dir = 1;
		lpMonster->Map = lpObj->Map;
		lpMonster->MonsterDeleteTime = GetTickCount() + 1800000;
		if (gObjSetMonster(index, monster) == 0)
		{
			gObjDel(index);
			return 0;
		}
	}
	gLog.Output(LOG_COMMAND, "[CommandSpot][%s][%s] - (Monster: %d, Qtd: %d)", lpObj->Account, lpObj->Name, monster, qtd);
#endif
	return 1;
}
bool CCommandManager::CommandSetCoin(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (!lpTarget)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(446));
		return false;
	}
	int coin = this->GetNumber(arg, 1);
	int value = this->GetNumber(arg, 2);
	switch (coin)
	{
	case 1: GDSetCoinSend(lpTarget->Index, value, 0, 0, 0, 0, "GmSetCoin"); break;
	case 2: GDSetCoinSend(lpTarget->Index, 0, value, 0, 0, 0, "GmSetCoin"); break;
	case 3: GDSetCoinSend(lpTarget->Index, 0, 0, value, 0, 0, "GmSetCoin"); break;
	case 4: GDSetCoinSend(lpTarget->Index, 0, 0, 0, value, 0, "GmSetCoin"); break;
	case 5: GDSetCoinSend(lpTarget->Index, 0, 0, 0, 0, value, "GmSetCoin"); break;
	default: return false;
	}
	gLog.Output(LOG_COMMAND, "[CommandSetCoin][%s][%s] - (Coin: %d, Value: %d)", lpObj->Account, lpObj->Name, coin, value);
	return true;
}
bool CCommandManager::CommandSetLevel(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(777));
		return 0;
	}
	int level = this->GetNumber(arg, 1);
	if (level >= MAX_CHARACTER_LEVEL + 1)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(778));
		return 0;
	}
	lpTarget->Level = level;
	gGetInfoCharacter.GCLevelUpSend(lpTarget);
	gLog.Output(LOG_COMMAND, "[CommandSetLevel] Account:[%s] Name:[%s] - (Name:%s, Level: %d)", lpObj->Account, lpObj->Name, name, level);
}
bool CCommandManager::CommandSetReset(LPOBJ lpObj, char* arg)
{
	char name[11] = { 0 };
	this->GetString(arg, name, sizeof(name), 0);
	LPOBJ lpTarget = gObjFind(name);
	if (lpTarget == 0)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(777));
		return 0;
	}
	int Reset = this->GetNumber(arg, 1);
	if (Reset >= 65535)
	{
		gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(778));
		return 0;
	}
	lpTarget->Reset = (lpTarget->Reset + Reset);
	gObjectManager.CharacterCalcAttribute(lpObj->Index);
	gGetInfoCharacter.GCNewCharacterInfoSend(lpObj);
	GDCharacterInfoSaveSend(lpObj->Index);
	GDResetInfoSaveSend(lpObj->Index, 0, 0, 0);
	gNotice.GCNoticeSend(lpObj->Index, 1, 0, 0, 0, 0, 0, pMessage->Text(779), Reset, lpTarget->Reset);
	gLog.Output(LOG_COMMAND, "[CommandSetReset] Account:[%s] Name:[%s] - (Name:%s, Reset: %d)", lpObj->Account, lpObj->Name, name, Reset);
}
bool CCommandManager::CommandSetPass(LPOBJ lpObj, char* arg)
{
	char password[20] = { 0 };
	this->GetString(arg, password, sizeof(password), 0);
	PMSG_RECV_SETPASS_ADD pMsg;
	pMsg.SystemActive = true;
	memcpy(pMsg.Password, password, sizeof(pMsg.Password));
	if (strcmp(password, "") == 0)
	{
		pMsg.SystemActive = false;
	}
	gPartySetPass.PartySetPassSearchAddToList(pMsg, lpObj->Index);
	gLog.Output(LOG_COMMAND, "[CommandSetpass][%s][%s] - Password: %d", lpObj->Account, lpObj->Name, password);
	return 1;
}
bool CCommandManager::CommandStartBsV(LPOBJ lpObj, char* arg)
{
#if (GAMESERVER_CLIENTE_UPDATE >= 9)
#if(EVENT_PK)
	gBsVEvent.StartBSV();
	gLog.Output(LOG_COMMAND, "[CommandStartBSV][%s][%s] - Used", lpObj->Account, lpObj->Name);
#endif
#endif
	return 1;
}
