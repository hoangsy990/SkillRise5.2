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
#include <string>
#ifdef RISE_GROW_LANCER_RUNTIME_QA
extern void WriteCrashBreadcrumb(const char* message);
#endif
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
	gProtect = new CProtect;

	if (gProtect->ReadMainFile("Data\\RISE\\ServerData.bmd") == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_FF, ERROR_FF, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	if (gProtect->ReadConnectFile("Data\\RISE\\ConnectIP.bmd") == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_FF, ERROR_FF, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

#ifdef RISE_GROW_LANCER_RUNTIME_QA
	char localQA[4] = { 0 };
	const bool localOnly = GetEnvironmentVariableA("RISE_GL_LOCAL_SERVER_QA", localQA, sizeof(localQA)) == 1 && localQA[0] == '1';
	if (localOnly)
	{
		// This stream uses ConnectIP.bmd's remote AntiPort and is separate
		// from the native game socket. The loopback QA stack has no stream peer.
		WriteCrashBreadcrumb("Grow Lancer local QA: remote AntiStream thread omitted");
	}
	else
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AntiStream_StreamThread, NULL, 0, NULL);
	}
#else
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AntiStream_StreamThread, NULL, 0, NULL);
#endif

	pKit->BmdRISE();
	gTrayMode.Instance = hInstance;
	#ifndef ENABLE_EDIT
	
	//gProtect->CheckPluginFile();
	//gProtect->CheckClientFile();
	gProtect->CheckMaxGameInstances();
	
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

	this->BmdCustomRISE();
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
