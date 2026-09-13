//*****************************************************************************
// File: LoginWin.cpp
//*****************************************************************************

#include "stdafx.h"
#include "LoginWin.h"
#include "Input.h"
#include "UIMng.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "UIControls.h"
#include "ZzzScene.h"
#include "wsclientinline.h"
#include "DSPlaySound.h"
#include "./Utilities/Log/muConsoleDebug.h"
#include "RISE/CProtect.h"
#include "RISE/PegasusRuntimeTrace.h"

#include "ServerListManager.h"
#include "RISE/WideData.h"
#include "RISE/Reconnect.h"
#include <vector>
#include <string>
#include <algorithm>
#include "ProtectSend/AntiStreamClient.h"
#ifdef min
#undef min
#endif
#include <Local.h>
#ifdef max
#undef max
#endif

#define	LIW_ACCOUNT		0
#define	LIW_PASSWORD	1

#define LIW_OK			0
#define LIW_CANCEL		1

extern RISE_SHARED_INFO g_RISESharedInfo;
extern int g_iChatInputType;

CLoginWin::CLoginWin()
{
	m_pIDInputBox	= NULL;
	m_pPassInputBox = NULL;
	this->FirstLoad = 1;
	this->SavePass = 0;
}

CLoginWin::~CLoginWin()
{
	SAFE_DELETE(m_pIDInputBox);
	SAFE_DELETE(m_pPassInputBox);
}

static const int LIW_MAX = 10;

#if defined(PEGASUS_ISOLATED_TEST)
static bool IsPegasusAutoQaEnabled()
{
	static const char* markers[] = {
		"PegasusAutoQA.enable",
		"PegasusLocalVisualQA.enable",
		"PegasusSwordBlowPreview.enable",
	};
	for (int i = 0; i < 3; ++i)
	{
		const DWORD attributes = GetFileAttributesA(markers[i]);
		if (attributes != INVALID_FILE_ATTRIBUTES &&
			(attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			return true;
	}
	return false;
}
#endif


static void ReadAccountList(std::vector<std::string>& ids, std::vector<std::string>& pws)
{
	HKEY hKey; ids.clear(); pws.clear();
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, KEY_READ, &hKey) != ERROR_SUCCESS) return;
	DWORD cb = 0;
	RegQueryValueExA(hKey, "LiwID", 0, 0, 0, &cb);
	std::vector<char> bufID(cb ? cb : (LIW_MAX * (MAX_ID_SIZE + 1)), 0);
	DWORD type = 0, sz = (DWORD)bufID.size();
	if (sz && RegQueryValueExA(hKey, "LiwID", 0, &type, (BYTE*)bufID.data(), &sz) == ERROR_SUCCESS && type == REG_BINARY)
	{
		int step = MAX_ID_SIZE + 1;
		int cnt = (int)(sz / step);
		for (int i = 0; i < cnt; ++i) ids.emplace_back(bufID.data() + i * step);
	}
	RegQueryValueExA(hKey, "LiwKey", 0, 0, 0, &cb);
	std::vector<char> bufPW(cb ? cb : (LIW_MAX * (MAX_PASSWORD_SIZE + 1)), 0);
	sz = (DWORD)bufPW.size(); type = 0;
	if (sz && RegQueryValueExA(hKey, "LiwKey", 0, &type, (BYTE*)bufPW.data(), &sz) == ERROR_SUCCESS && type == REG_BINARY)
	{
		int step = MAX_PASSWORD_SIZE + 1;
		int cnt = (int)(sz / step);
		for (int i = 0; i < cnt; ++i) pws.emplace_back(bufPW.data() + i * step);
	}
	RegCloseKey(hKey);
	if (ids.size() != pws.size()) { size_t m = (std::min)(ids.size(), pws.size()); ids.resize(m); pws.resize(m); }
	if (ids.size() > LIW_MAX) { ids.resize(LIW_MAX); pws.resize(LIW_MAX); }
}

static void WriteAccountList(const std::vector<std::string>& ids, const std::vector<std::string>& pws)
{
	HKEY hKey; if (RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, 0, 0, KEY_WRITE, 0, &hKey, 0) != ERROR_SUCCESS) return;
	int n = (int)std::min(ids.size(), pws.size()); if (n < 0) n = 0; if (n > LIW_MAX) n = LIW_MAX;
	std::vector<char> bufID(n * (MAX_ID_SIZE + 1), 0);
	std::vector<char> bufPW(n * (MAX_PASSWORD_SIZE + 1), 0);
	for (int i = 0; i < n; ++i)
	{
		strncpy(&bufID[i * (MAX_ID_SIZE + 1)], ids[i].c_str(), MAX_ID_SIZE);
		strncpy(&bufPW[i * (MAX_PASSWORD_SIZE + 1)], pws[i].c_str(), MAX_PASSWORD_SIZE);
	}
	if (!bufID.empty()) RegSetValueExA(hKey, "LiwID", 0, REG_BINARY, (const BYTE*)bufID.data(), (DWORD)bufID.size()); else RegDeleteValueA(hKey, "LiwID");
	if (!bufPW.empty()) RegSetValueExA(hKey, "LiwKey", 0, REG_BINARY, (const BYTE*)bufPW.data(), (DWORD)bufPW.size()); else RegDeleteValueA(hKey, "LiwKey");
	RegCloseKey(hKey);
}

static void AppendAccountToList(const char* id, const char* pw)
{
	std::vector<std::string> ids, pws; ReadAccountList(ids, pws);
	int exist = -1;
	for (int i = 0; i < (int)ids.size(); ++i) if (_stricmp(ids[i].c_str(), id) == 0) { exist = i; break; }
	if (exist >= 0) { ids.erase(ids.begin() + exist); pws.erase(pws.begin() + exist); }
	ids.insert(ids.begin(), id); pws.insert(pws.begin(), pw);
	if ((int)ids.size() > LIW_MAX) { ids.resize(LIW_MAX); pws.resize(LIW_MAX); }
	WriteAccountList(ids, pws);
}

static DWORD ReadSavePass() 
{
	HKEY hKey; DWORD dwValue = 0; DWORD dwSize = sizeof(dwValue);
	if (RegOpenKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
	{
		RegQueryValueExA(hKey, "SavePass", 0, 0, (BYTE*)&dwValue, &dwSize);
		RegCloseKey(hKey);
	}
	return dwValue;
}

static bool WriteDword(DWORD value) 
{
	HKEY hKey = NULL; DWORD dwDisp; DWORD dwSize = sizeof(DWORD);
	if (ERROR_SUCCESS != RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, &dwDisp))
		return false;
	RegSetValueExA(hKey, "SavePass", 0L, REG_DWORD, (BYTE*)&value, dwSize);
	RegCloseKey(hKey);
	return true;
}

static void DeleteAccountAt(int idx)
{
	std::vector<std::string> ids, pws; ReadAccountList(ids, pws);
	if (idx >= 0 && idx < (int)ids.size()) 
	{ 
		ids.erase(ids.begin() + idx); pws.erase(pws.begin() + idx); 
	}
	WriteAccountList(ids, pws);
}


void CLoginWin::Create()
{
	CWin::Create(329, 245, BITMAP_LOG_IN + 7);

	 m_asprInputBox[LIW_ACCOUNT].Create(170, 21, BITMAP_LOG_IN + 8);
	m_asprInputBox[LIW_PASSWORD].Create(170, 21, BITMAP_LOG_IN + 8);

	for (int i = 0; i < 2; ++i)
	{
		m_aBtn[i].Create(54, 30, BITMAP_BUTTON + i, 3, 2, 1);
		CWin::RegisterButton(&m_aBtn[i]);
	}

	std::vector<std::string> __ids, __pws;
	ReadAccountList(__ids, __pws);

	SAFE_DELETE(m_pIDInputBox);

	m_pIDInputBox = new CUITextInputBox;
	m_pIDInputBox->Init(g_hWnd, 140, 14, MAX_ID_SIZE);
	m_pIDInputBox->SetBackColor(0, 0, 0, 255);
	m_pIDInputBox->SetTextColor(255, 255, 230, 210);
	m_pIDInputBox->SetFont(g_hFont);
	m_pIDInputBox->SetState(UISTATE_NORMAL);

	if (!__ids.empty()) 
	{ 
		strncpy(m_ID, __ids[0].c_str(), MAX_ID_SIZE); m_ID[MAX_ID_SIZE] = '\0'; 
	}

	m_pIDInputBox->SetText(m_ID);

	SAFE_DELETE(m_pPassInputBox);

	m_pPassInputBox = new CUITextInputBox;
	m_pPassInputBox->Init(g_hWnd, 140, 14, MAX_PASSWORD_SIZE, TRUE);
	m_pPassInputBox->SetBackColor(0, 0, 0, 255);
	m_pPassInputBox->SetTextColor(255, 255, 230, 210);
	m_pPassInputBox->SetFont(g_hFont);
	m_pPassInputBox->SetState(UISTATE_NORMAL);

	if (!__pws.empty()) 
	{ 
		strncpy(m_PW, __pws[0].c_str(), MAX_PASSWORD_SIZE); m_PW[MAX_PASSWORD_SIZE] = '\0';
	}

	m_pPassInputBox->SetText(m_PW);

	m_pIDInputBox->SetTabTarget(m_pPassInputBox);
	m_pPassInputBox->SetTabTarget(m_pIDInputBox);

	SavePass = ReadSavePass();

	if (strlen(m_ID) > 0 && strlen(m_PW) > 0 && SavePass)
	{
#if defined(PEGASUS_ISOLATED_TEST)
		if (IsPegasusAutoQaEnabled())
		{
			this->FirstLoad = 2;
			rise::pegasus::TraceRuntime("AUTO_QA_LOGIN", 1, 0, "ARMED");
		}
		else
#endif
		{
			this->FirstLoad = 3;
		}
	}
}

void CLoginWin::PreRelease()
{
	for (int i = 0; i < 2; ++i)
		m_asprInputBox[i].Release();
}

void CLoginWin::SetPosition(int nXCoord, int nYCoord)
{
	CWin::SetPosition(nXCoord, nYCoord);

	m_asprInputBox[LIW_ACCOUNT].SetPosition( nXCoord + 24, nYCoord + 80);
	m_asprInputBox[LIW_PASSWORD].SetPosition(nXCoord + 24, nYCoord + 119);

	if (g_iChatInputType == 1)
	{
		m_pIDInputBox->SetPosition(
			int((m_asprInputBox[LIW_ACCOUNT].GetXPos() + 78) / g_SetForm->x_fScreenRate_x),
			int((m_asprInputBox[LIW_ACCOUNT].GetYPos() + 5) / g_SetForm->x_fScreenRate_y));

		m_pPassInputBox->SetPosition(
			int((m_asprInputBox[LIW_PASSWORD].GetXPos() + 78) / g_SetForm->x_fScreenRate_x),
			int((m_asprInputBox[LIW_PASSWORD].GetYPos() + 5) / g_SetForm->x_fScreenRate_y));
	}

	m_aBtn[LIW_OK].SetPosition(nXCoord + 18, nYCoord + 183);
	m_aBtn[LIW_CANCEL].SetPosition(nXCoord + 146, nYCoord + 183);
}

void CLoginWin::Show(bool bShow)
{
	CWin::Show(bShow);

	for (int i = 0; i < 2; ++i)
	{
		m_asprInputBox[i].Show(bShow);
		m_aBtn[i].Show(bShow);
	}
}

bool CLoginWin::CursorInWin(int nArea)
{
	if (!CWin::m_bShow)
		return false;

	switch (nArea)
	{
		case WA_MOVE:
		return false;
	}

	return CWin::CursorInWin(nArea);
}

void CLoginWin::UpdateWhileActive(double dDeltaTick)
{
	CInput& rInput = CInput::Instance();

	if (TryPegasusAutoQaLogin())
		return;

	if (m_aBtn[LIW_OK].IsClick())
		RequestLogin();
	else if (m_aBtn[LIW_CANCEL].IsClick())
		CancelLogin();
	else if (CInput::Instance().IsKeyDown(VK_RETURN))
	{
		::PlayBuffer(SOUND_CLICK01);
		RequestLogin();
	}
	else if (CInput::Instance().IsKeyDown(VK_ESCAPE))
	{
		::PlayBuffer(SOUND_CLICK01);
		CancelLogin();
		CUIMng::Instance().SetSysMenuWinShow(false);
	}
}

bool CLoginWin::TryPegasusAutoQaLogin()
{
#if defined(PEGASUS_ISOLATED_TEST)
	if (!IsPegasusAutoQaEnabled())
		return false;

	// LoginWin can be shown without becoming CUIMng's active modal window.
	// Trace only transitions, and allow both UpdateWhileActive and the known
	// render path to drive the one-shot request after the join handshake.
	static int lastFirstLoad = -1;
	static int lastProtocolState = -1;
	if (lastFirstLoad != this->FirstLoad ||
		lastProtocolState != CurrentProtocolState)
	{
		lastFirstLoad = this->FirstLoad;
		lastProtocolState = CurrentProtocolState;
		rise::pegasus::TraceRuntime("AUTO_QA_LOGIN_STATE",
			this->FirstLoad, CurrentProtocolState, "OBSERVED");
	}

	if (this->FirstLoad == 2 &&
		CurrentProtocolState == RECEIVE_JOIN_SERVER_SUCCESS)
	{
		// Advance the guard before RequestLogin can hide/re-enter the window.
		this->FirstLoad = 3;
		rise::pegasus::TraceRuntime("AUTO_QA_LOGIN", 2,
			CurrentProtocolState, "REQUEST");
		RequestLogin();
		return true;
	}
#endif
	return false;
}

void CLoginWin::UpdateWhileShow(double dDeltaTick)
{
	m_pIDInputBox->DoAction();
	m_pPassInputBox->DoAction();
}

inline int LoginSavePassX()
{
	int Value = 0;
	switch (m_Resolution)
	{
		case 0:  Value = 200; break;
		case 1:  Value = 200; break;
		case 2:  Value = 200; break;
		case 3:  Value = 194; break;
		case 4:  Value = 193; break;
		case 5:  Value = 191; break;
		case 6:  Value = 191; break;
		case 7:  Value = 187; break;
		case 8:  Value = 187; break;
		case 9:  Value = 187; break;
		case 10: Value = 187; break;
	}
	return Value;
}

inline int LoginSavePassY()
{
	int Value = 0;
	switch (m_Resolution)
	{
		case 0:  Value = 162; break;
		case 1:  Value = 162; break;
		case 2:  Value = 162; break;
		case 3:  Value = 162; break;
		case 4:  Value = 162; break;
		case 5:  Value = 158; break;
		case 6:  Value = 159; break;
		case 7:  Value = 157; break;
		case 8:  Value = 157; break;
		case 9:  Value = 157; break;
		case 10:  Value = 157; break;
	}
	return Value;
}
inline int LoginSavePassTextY()
{
	int Value = 0;
	switch (m_Resolution)
	{
		case 0:  Value = 165; break;
		case 1:  Value = 165; break;
		case 2:  Value = 165; break;
		case 3:  Value = 165; break;
		case 4:  Value = 165; break;
		case 5:  Value = 163; break;
		case 6:  Value = 165; break;
		case 7:  Value = 164; break;
		case 8:  Value = 164; break;
		case 9:  Value = 165; break;
		case 10:  Value = 166; break;
	}
	return Value;
}
void CLoginWin::RenderControls()
{
	if (TryPegasusAutoQaLogin())
		return;

	if (this->FirstLoad == 1)
	{
		if (strlen(m_ID) > 0)
			CUIMng::Instance().m_LoginWin.GetPassInputBox()->GiveFocus();
		else
			CUIMng::Instance().m_LoginWin.GetIDInputBox()->GiveFocus();
		this->FirstLoad = 0;
	}

	CWin::RenderButtons();

	for (int i = 0; i < 2; ++i)
		m_asprInputBox[i].Render();

	m_pIDInputBox->Render();
	m_pPassInputBox->Render();

	g_pRenderText->SetFont(g_hFont);
	g_pRenderText->SetBgColor(0);
	g_pRenderText->SetTextColor(CLRDW_WHITE);

	g_pRenderText->RenderText(
		int((CWin::GetXPos() + 22) / g_SetForm->x_fScreenRate_x),		
		int((CWin::GetYPos() + 84) / g_SetForm->x_fScreenRate_y),
		GlobalText[450]);

	g_pRenderText->RenderText(
		int((CWin::GetXPos() + 22) / g_SetForm->x_fScreenRate_x),
		int((CWin::GetYPos() + 120) / g_SetForm->x_fScreenRate_y),
		GlobalText[451]);

	g_pRenderText->RenderText(
		int((CWin::GetXPos() + 45) / g_SetForm->x_fScreenRate_x),
		int((CWin::GetYPos() + LoginSavePassTextY() - 7) / g_SetForm->x_fScreenRate_y),
		GlobalText[3766]);
	
	unicode::t_char szServerName[MAX_TEXT_LENGTH];

	const char* apszGlobalText[4] = 
	{ 
		gProtect->m_MainInfo.ServerName[0], 
		gProtect->m_MainInfo.ServerName[1],
		gProtect->m_MainInfo.ServerName[2], 
		gProtect->m_MainInfo.ServerName[3]
	};

	sprintf(szServerName, apszGlobalText[g_ServerListManager->GetNonPVPInfo()],	g_ServerListManager->GetSelectServerName(), g_ServerListManager->GetSelectServerIndex(), "");

	g_pRenderText->RenderText(int((CWin::GetXPos() + 20) / g_SetForm->x_fScreenRate_x), int((CWin::GetYPos() + 35) / g_SetForm->x_fScreenRate_y), szServerName, 190);

	int ClickX = int((CWin::GetXPos() + LoginSavePassX() - 175) / g_SetForm->x_fScreenRate_x);
	int ClickY = int((CWin::GetYPos() + LoginSavePassY() - 10) / g_SetForm->x_fScreenRate_y);

	std::vector<std::string> ids, pws;
	ReadAccountList(ids, pws);
	int cnt = (int)ids.size();
	if (cnt > 8) cnt = 8;

	int listX = int((CWin::GetXPos() + 225) / g_SetForm->x_fScreenRate_x);
	int listY = int((CWin::GetYPos() + 64) / g_SetForm->x_fScreenRate_y);
	int itemW = 40;
	int itemH = 12;

	for (int i = 0; i < cnt; ++i)
	{
		int iy = listY + i * itemH;

		bool hovered = CheckMouseIn(listX, iy, itemW + 4, itemH);
		if (hovered)
			g_pUIForm->RenderHover((float)listX - 4, (float)iy - 1, (float)itemW + 7, (float)itemH - 1);

		g_pRenderText->SetFont(g_hFont);
		g_pRenderText->SetBgColor(0);
		g_pRenderText->SetTextColor(hovered ? CLRDW_YELLOW : CLRDW_WHITE);
		g_pRenderText->RenderText(listX, iy, ids[i].c_str());

		if (SEASON3B::IsPress(VK_LBUTTON) && hovered)
		{
			m_pIDInputBox->SetText(ids[i].c_str());
			const char* pw = (i < (int)pws.size()) ? pws[i].c_str() : "";
			m_pPassInputBox->SetText(pw);
			RequestLogin();
			return;
		}

		float delX = (float)(listX + itemW + 4);
		float delY = (float)(iy - 0.5f);

		SEASON3B::RenderImageF(BITMAP_START_RISE_LOGIN_WIN_NAVI, delX, delY, 10.0f, 10.0f, 0.0f, 0.0f, 15.0f, 15.0f);

		if (CheckMouseIn((int)delX, (int)delY, 10, 10))
		{
			g_pUIForm->RenderHover(delX, delY, 10.0f, 10.0f, 0x303030A0);

			if (SEASON3B::IsPress(VK_LBUTTON))
			{
				DeleteAccountAt(i);
				return;
			}
		}
	}

	if (SEASON3B::IsPress(VK_LBUTTON) && CheckMouseIn(ClickX, ClickY, 15, 15))
	{
		SavePass = (SavePass ? 0 : 1);
		WriteDword(SavePass);
	}

	if (SavePass)
		SEASON3B::RenderImage(SEASON3B::CNewUIOptionWindow::IMAGE_OPTION_BTN_CHECK, ClickX, ClickY, 15, 15, 0, 0);
	else
		SEASON3B::RenderImage(SEASON3B::CNewUIOptionWindow::IMAGE_OPTION_BTN_CHECK, ClickX, ClickY, 15, 15, 0, 15.0f);

}

void CLoginWin::RequestLogin()
{
	if (CurrentProtocolState == REQUEST_JOIN_SERVER)
		return;

	CUIMng::Instance().HideWin(this);

	char szID[MAX_ID_SIZE + 1] = { 0, };
	char szPass[MAX_PASSWORD_SIZE + 1] = { 0, };
	m_pIDInputBox->GetText(szID, MAX_ID_SIZE + 1);
	m_pPassInputBox->GetText(szPass, MAX_PASSWORD_SIZE + 1);

	if (unicode::_strlen(szID) <= 0)
		CUIMng::Instance().PopUpMsgWin(MESSAGE_INPUT_ID);
	else if (unicode::_strlen(szPass) <= 0)
		CUIMng::Instance().PopUpMsgWin(MESSAGE_INPUT_PASSWORD);
	else if (CheckSpecialText(szID) || CheckSpecialText(szPass))
		CUIMng::Instance().PopUpMsgWin(REQUEST_LOGIN_CHARTEXT);
	else
	{
		if (CurrentProtocolState == RECEIVE_JOIN_SERVER_SUCCESS)
		{
			g_ConsoleDebug->Write(MCD_NORMAL, "Login with the following account: %s / %s ", szID, szPass);

			memcpy(g_pReconnect->s_Data.ReconnectAccount, szID, 11);
			memcpy(g_pReconnect->s_Data.ReconnectPassword, szPass, 11); 

			strncpy(g_RISESharedInfo.AccountID, szID, 10);
			g_RISESharedInfo.AccountID[10] = '\0';

			SendRequestLogIn(szID, szPass);

			SavePass = ReadSavePass();

			if (SavePass == 1)
			{
				AppendAccountToList(szID, szPass);
			}

		}
	}
}

void CLoginWin::CancelLogin()
{
	ConnectConnectionServer();
	CUIMng::Instance().HideWin(this);
}
void CLoginWin::ConnectConnectionServer()
{
	LogIn = 0;
	CurrentProtocolState = REQUEST_JOIN_SERVER;
	CreateSocket(szServerIpAddress, g_ServerPort);
}
