#include "stdafx.h"
#include "MainProtocol.h"
#include "SystemPKSend.h"
#include "NewUIChaosMixRate.h"
#include "NewUICharInfoStats.h"
#include "PetProtocol.h"
#include "CustomPreview.h"
#include <ZzzInterface.h>
#include "CustomReadGuildClient.h"
#include <NewUISystem.h>
#include <MonkSystem.h>
#include "ZzzToolKit.h"
#include "CProtect.h"
#include <iostream>
#include <UIMng.h>
#include "RenderCape.h"
#include "ProtectSend/AntiStreamClient.h"
#include "TrayModeNew.h"
#ifdef RISE_SLAYER_RUNTIME_QA
#include "SlayerRuntimeQA.h"
#define SLAYER_QA_STEP(text) rise::slayerqa::AppendRuntimeQALog(text)
#else
#define SLAYER_QA_STEP(text) ((void)0)
#endif
#include <string>
extern char ReadLang[16];
MainProtocolSub gProtocol;

MainProtocolSub* MainProtocolSub::Instance()
{
	static MainProtocolSub pInstance;

	return &pInstance;
}

void MainProtocolSub::BmdCustomRISE()
{
	char FileName[260];

	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomJewel.bmd", ReadLang);
	g_pCustomJewel->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomRingPen.bmd", ReadLang);
	g_pCustomRingPen->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomModelNpc.bmd", ReadLang);
	g_pModelNPC->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomItemPosition.bmd", ReadLang);
	g_pItemPos->Load(FileName);//
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomSetEffect.bmd", ReadLang);
	g_pSetEffect->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomItemColorName.bmd", ReadLang);
	g_pItemColor->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomNpcName.bmd", ReadLang);
	g_pNPCName->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomIconBuff.bmd", ReadLang);
	g_pIconBuff->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomMonster.bmd", ReadLang);
	gCustomMonster->Load(FileName);

	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\RenderEffect.bmd", ReadLang);
	g_pCustomRenderEffect->Load(FileName);
	sprintf(FileName, "Data\\RISE\\Config%s\\Info\\CustomMonsterGold.bmd", ReadLang);
	g_pMonsterGlow->Load(FileName);
	gCustomWing->Load(gProtect->m_MainInfo.CustomWingInfo);
	gCapeRender.LoadFromWing(gProtect->m_MainInfo.CustomWingInfo, MAX_CUSTOM_WING);
}

void MainProtocolSub::WinHook(HINSTANCE hInstance)
{
	SLAYER_QA_STEP("winhook begin");
	gProtect = new CProtect;

	if (gProtect->ReadMainFile("Data\\RISE\\ServerData.bmd") == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_FF, ERROR_FF, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	SLAYER_QA_STEP("winhook ServerData loaded");

	if (gProtect->ReadConnectFile("Data\\RISE\\ConnectIP.bmd") == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_FF, ERROR_FF, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	SLAYER_QA_STEP("winhook ConnectIP loaded");

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AntiStream_StreamThread, NULL, 0, NULL);

	SLAYER_QA_STEP("winhook BmdRISE begin");
	pKit->BmdRISE();
	SLAYER_QA_STEP("winhook BmdRISE complete");
	gTrayMode.Instance = hInstance;
	#ifndef ENABLE_EDIT
	
	//gProtect->CheckPluginFile();
	//gProtect->CheckClientFile();
#ifndef RISE_SLAYER_RUNTIME_QA
	gProtect->CheckMaxGameInstances();
#endif
	
	#endif

	gCustomItem.Load(gProtect->m_MainInfo.CustomItemInfo);
	gCustomPet.Load(gProtect->m_MainInfo.CustomPetInfo);
	gCustomBow.Load(gProtect->m_MainInfo.CustomBowInfo);
	GInfo.loadnInformation(gProtect->m_MainInfo.m_TRSTooltipData);
	GInfo.loadnText(gProtect->m_MainInfo.m_TRSTooltipText);
	gCustomCEffectPet.Load(gProtect->m_MainInfo.m_PetCEffectBMD);
	gCustomCEffectPet.LoadGlow(gProtect->m_MainInfo.RenderMeshPet);
	gCustomBattleGloves.Load(gProtect->m_MainInfo.CustomGloves);
	g_CMonkSystem.RegistItem();

	SLAYER_QA_STEP("winhook custom data begin");
	this->BmdCustomRISE();
	SLAYER_QA_STEP("winhook complete");
}

void MainProtocolSub::RecvPShop(const BYTE* Data)
{
	auto DataRISE = reinterpret_cast<const RecvOffTradeType*>(Data);
	this->ShopList = 1;
	this->TypeShop = DataRISE->Type;
}

void MainProtocolSub::PShopActiveRecv(const BYTE* Data)
{
	auto DataRISE = reinterpret_cast<const LoadOffTradeData*>(Data);
	this->ShopActive = DataRISE->Active;
	this->TypeShop = DataRISE->Type;
}

void MainProtocolSub::SpeedAttack(const BYTE* lpMsg)
{
	auto DataRISE = reinterpret_cast<const GetValueSpeedAttack*>(lpMsg);
	for (int RISE = 0; RISE < 14; RISE++)
	{
		this->ThisValue[RISE] = DataRISE->ThisValue[RISE];
	}
}

void MainProtocolSub::GCThisCoin(const BYTE* lpMsg)
{
	auto DataRISE = reinterpret_cast<const GetReceivedCoinData*>(lpMsg);

	for (int RISE = 0; RISE < MAX_COIN_POINT; RISE++)
	{
		this->ThisCoin[RISE] = DataRISE->ThisCoin[RISE];
	}
}
