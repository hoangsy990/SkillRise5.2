#include "stdafx.h"
#include "QuestWorldReward.h"
#include "../../Shared/LegacySkillClassColumns.h"
#include "DSProtocol.h"
#include "EffectManager.h"
#include "GensSystem.h"
#include "MemScript.h"
#include "ObjectManager.h"
#include "QuestWorld.h"
#include "ServerInfo.h"
#include "Util.h"
CQuestWorldReward gQuestWorldReward;
CQuestWorldReward::CQuestWorldReward()
{
	this->m_count = 0;
}
void CQuestWorldReward::Load(char* path)
{
	CMemScript* lpMemScript = new CMemScript;
	if(lpMemScript == 0)
	{
		ErrorMessageBox(MEM_SCRIPT_ALLOC_ERROR,path);
		return;
	}
	if(lpMemScript->SetBuffer(path) == 0)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
		delete lpMemScript;
		return;
	}
	this->m_count = 0;
	try
	{
		while(true)
		{
			if(lpMemScript->GetToken() == TOKEN_END)
			{
				break;
			}
			if(strcmp("end",lpMemScript->GetString()) == 0)
			{
				break;
			}
			QUEST_WORLD_REWARD_INFO info;
			memset(&info,0,sizeof(info));
			info.Number = lpMemScript->GetNumber();
			info.Type = lpMemScript->GetAsNumber();
			info.Index = lpMemScript->GetAsNumber();
			info.Value = lpMemScript->GetAsNumber();
			info.Level = lpMemScript->GetAsNumber();
			info.Option1 = lpMemScript->GetAsNumber();
			info.Option2 = lpMemScript->GetAsNumber();
			info.Option3 = lpMemScript->GetAsNumber();
			info.NewOption = lpMemScript->GetAsNumber();
			info.RequireIndex = lpMemScript->GetAsNumber();
			info.RequireGroup = lpMemScript->GetAsNumber();
			info.RequireState = lpMemScript->GetAsNumber();
			rise::growlancer::ReadLegacyClassColumns(info.RequireClass,
				[lpMemScript]() { return lpMemScript->GetAsNumber(); });
			this->SetInfo(info);
		}
	}
	catch(...)
	{
		ErrorMessageBox(lpMemScript->GetLastError());
	}
	delete lpMemScript;
}
void CQuestWorldReward::SetInfo(QUEST_WORLD_REWARD_INFO info)
{
	if(this->m_count < 0 || this->m_count >= MAX_QUEST_WORLD_REWARD)
	{
		return;
	}
	this->m_QuestWorldRewardInfo[this->m_count++] = info;
}
QUEST_WORLD_REWARD_INFO* CQuestWorldReward::GetInfo(int index)
{
	if(index < 0 || index >= this->m_count)
	{
		return 0;
	}
	return &this->m_QuestWorldRewardInfo[index];
}
bool CQuestWorldReward::GetQuestWorldReward(LPOBJ lpObj,int QuestIndex,int QuestGroup,QUEST_WORLD_REWARD* lpReward,int RewardNumber)
{
	for(int n=0;n < this->m_count;n++)
	{
		QUEST_WORLD_REWARD_INFO* lpInfo = this->GetInfo(n);
		if(lpInfo == 0)
		{
			continue;
		}
		if(lpInfo->RequireIndex != QuestIndex)
		{
			continue;
		}
		if(lpInfo->RequireGroup != QuestGroup)
		{
			continue;
		}
		if(this->CheckQuestWorldRewardRequisite(lpObj,lpInfo) == 0)
		{
			continue;
		}
		if(lpInfo->Number != RewardNumber)
		{
			continue;
		}
		lpReward->type = lpInfo->Type;
		lpReward->index = lpInfo->Index;
		lpReward->value = lpInfo->Value;
		if(lpInfo->Type == QUEST_WORLD_REWARD_ITEM)
		{
			CItem item;
			item.m_Level = lpInfo->Level;
			item.Convert(lpInfo->Index,lpInfo->Option1,lpInfo->Option2,lpInfo->Option3,lpInfo->NewOption,0,0,0,0,0xFF);
			gItemManager.ItemByteConvert(lpReward->ItemInfo,item);
		}
		return 1;
	}
	return 0;
}
bool CQuestWorldReward::CheckQuestWorldRewardRequisite(LPOBJ lpObj,QUEST_WORLD_REWARD_INFO* lpInfo)
{
	if(lpInfo->RequireIndex != -1 && gQuestWorld.CheckQuestWorldListState(lpObj,lpInfo->RequireIndex,lpInfo->RequireGroup,lpInfo->RequireState) == 0)
	{
		return 0;
	}
	if(lpInfo->RequireClass[lpObj->Class] == 0 || lpInfo->RequireClass[lpObj->Class] > (lpObj->ChangeUp+1))
	{
		return 0;
	}
	return 1;
}
void CQuestWorldReward::InsertQuestWorldReward(LPOBJ lpObj,int QuestIndex,int QuestGroup)
{
	for(int n=0;n < this->m_count;n++)
	{
		QUEST_WORLD_REWARD_INFO* lpInfo = this->GetInfo(n);
		if(lpInfo == 0)
		{
			continue;
		}
		if(lpInfo->RequireIndex != QuestIndex)
		{
			continue;
		}
		if(lpInfo->RequireGroup != QuestGroup)
		{
			continue;
		}
		if(this->CheckQuestWorldRewardRequisite(lpObj,lpInfo) == 0)
		{
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_NONE)
		{
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_EXPERIENCE)
		{
			if(gEffectManager.CheckEffect(lpObj,EFFECT_SEAL_OF_SUSTENANCE1) != 0 || gEffectManager.CheckEffect(lpObj,EFFECT_SEAL_OF_SUSTENANCE2) != 0)
			{
				continue;
			}
			if(gObjectManager.CharacterLevelUp(lpObj,(lpInfo->Value*gServerInfo.m_AddQuestExperienceRate[lpObj->AccountLevel]),gServerInfo.m_MaxLevelUpQuest,EXPERIENCE_QUEST) != 0)
			{
				continue;
			}
			GCRewardExperienceSend(lpObj->Index,(lpInfo->Value*gServerInfo.m_AddQuestExperienceRate[lpObj->AccountLevel]));
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_MONEY)
		{
			if(gObjCheckMaxMoney(lpObj->Index,lpInfo->Value) == 0)
			{
				lpObj->Money = MAX_MONEY;
			}
			else
			{
				lpObj->Money += lpInfo->Value;
			}
			GCMoneySend(lpObj->Index,lpObj->Money);
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_ITEM)
		{
			for(int i=0;i < lpInfo->Value;i++)
			{
				GDCreateItemSend(lpObj->Index,0xEB,0,0,lpInfo->Index,lpInfo->Level,0,lpInfo->Option1,lpInfo->Option2,lpInfo->Option3,-1,lpInfo->NewOption,0,0,0,0,0xFF,0);
			}
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_EFFECT)
		{
			gEffectManager.AddEffect(lpObj,1,lpInfo->Index,lpInfo->Value,0,0,0,0);
			continue;
		}
		if(lpInfo->Type == QUEST_WORLD_REWARD_CONTRIBUTION)
		{
			lpObj->GensContribution += lpInfo->Value;
			gGensSystem.GDGensSystemUpdateSend(lpObj->Index);
			continue;
		}
	}
}
