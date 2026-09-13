#include "Stdafx.h"
#include "ZzzToolKit.h"
#include "UIControls.h"
#include "StructSendGs.h"
#include "wsclientinline.h"
#include "MapManager.h"
#include <d3d9.h>
#include "WideData.h"
#include "NewUIInventoryCtrl.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <windows.h> 
#include "..\\..\\Util\\CCRC32.H"
#include <string>
#include <LoginWin.h>
#include <UIMng.h>
#include "Camera3D.h"
#include "RISE/GrowLancerRuntimeCapacity.h"
using namespace MUHelper;

CGToolKit* CGToolKit::Instance()
{
	static CGToolKit pInstance;
	return &pInstance;
}

CGToolKit::CGToolKit()
{
	this->EndKeyPressed = false;
	m_AutoCtrl = false;
	m_PKFreeMode = false;
	m_AutoHpMode = false;
	m_MiniMapOn = false;
	this->FontHeightRead = 14;
}

void CGToolKit::ReceiveAutoHpState(const BYTE* ReceiveBuffer)
{
	m_AutoHpMode = (ReceiveBuffer[4] != 0);
}

void CGToolKit::ChangeSize(int Resolution)
{
	int oldAddPos = g_SetForm->x_GetAddPos;

	g_SetForm->SetDefaultScreen(Resolution);
	m_Resolution = Resolution;

	g_SetForm->x_GetAddPos = oldAddPos;

	SetWindowPos(g_hWnd, HWND_TOP, 0, 0, WindowWidth, WindowHeight, SWP_NOZORDER | SWP_NOACTIVATE);

	if (g_hDC && g_hRC)
	{
		wglMakeCurrent(g_hDC, g_hRC);
		glViewport(0, 0, WindowWidth, WindowHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective2(45.0f, (float)WindowWidth / (float)WindowHeight, 1.0f, 1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	if (g_hFont)
	{
		DeleteObject(g_hFont);
		g_hFont = NULL;
	}

	if (g_hFontBig)
	{
		DeleteObject(g_hFontBig);
		g_hFontBig = NULL;
	}

	if (g_hFontBold)
	{
		DeleteObject(g_hFontBold);
		g_hFontBold = NULL;
	}

	this->WindowFont();

	if (g_pRenderText)
	{
		g_pRenderText->Release();
		g_pRenderText->Create(0, g_hDC);
		g_pRenderText->SetFont(g_hFont);
	}

	HKEY hKey;
	if (RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		RegSetValueExA(hKey, "Resolution", 0, REG_DWORD, (BYTE*)&m_Resolution, sizeof(DWORD));
		RegCloseKey(hKey);
	}

	char buf[8];
	sprintf_s(buf, "%d", m_Resolution);
	WritePrivateProfileStringA("FontConfig", "Resolution", buf, "./RISE.ini");
}


bool CGToolKit::CheckPickedItem()
{
	SEASON3B::CNewUIPickedItem* pPicked = SEASON3B::CNewUIInventoryCtrl::GetPickedItem();

	if (pPicked != NULL)
	{
		return true;
	}
	return false;
}

void CGToolKit::WindowRename()
{
	static DWORD dwLastUpdate = 0;
	DWORD dwNow = GetTickCount();

	if (dwNow - dwLastUpdate < 1000)
		return;

	dwLastUpdate = dwNow;

	char WindowName[500];
	#ifdef RISE_GROW_LANCER_RUNTIME_QA
	const char* baseWindowName = "Engine-Port S21";
	#else
	const char* baseWindowName = gProtect->m_MainInfo.WindowName;
	#endif

	if (SceneFlag == 5)
	{
		if ((Hero->CtlCode == CTLCODE_20OPERATOR) || (Hero->CtlCode == CTLCODE_08OPERATOR))
		{
			sprintf_s(WindowName, sizeof(WindowName), "%s - %s || Level: %d || Reset: %d/%d || FPS : %.1f || Mouse: %d %d [%d] - MapID: %d [World%d]", baseWindowName, Hero->ID, CharacterAttribute->Level, CharacterAttribute->ViewReset, CharacterAttribute->ViewMaxReset, FPS_AVG > gProtect->m_MainInfo.DefaultFPS ? gProtect->m_MainInfo.DefaultFPS : FPS_AVG, MouseX, MouseY, MouseLButtonPush, gMapManager.WorldActive, gMapManager.WorldActive + 1);
		}
		else
		{
			if (gProtect->m_MainInfo.ButtonCharracter[17])
			{
				sprintf_s(WindowName, sizeof(WindowName), "%s - %s || Level: %d || Reset: %d/%d || WC: %d || WP: %d || WG: %d || Ruud: %d", baseWindowName, Hero->ID, CharacterAttribute->Level, CharacterAttribute->ViewReset, CharacterAttribute->ViewMaxReset, pMain->ThisCoin[0], pMain->ThisCoin[1], pMain->ThisCoin[2], pMain->ThisCoin[3]);
			}
			else
			{
				sprintf_s(WindowName, sizeof(WindowName), "%s - %s || Level: %d || Reset: %d/%d", baseWindowName, Hero->ID, CharacterAttribute->Level, CharacterAttribute->ViewReset, CharacterAttribute->ViewMaxReset);
			}
		}
	}
	else
	{
		sprintf_s(WindowName, sizeof(WindowName), "%s", baseWindowName);
	}

	SetWindowText(g_hWnd, WindowName);
}

POINT_F CGToolKit::iPos(int index)
{
	char buf[32];
	char keyX[32];
	char keyY[32];
	char keyW[32];
	char keyH[32];

	sprintf_s(keyX, "BODY_X%d", index);
	sprintf_s(keyY, "BODY_Y%d", index);
	sprintf_s(keyW, "BODY_W%d", index);
	sprintf_s(keyH, "BODY_H%d", index);

	GetPrivateProfileStringA("RISE", keyX, "0.0", buf, sizeof(buf), "./RISE.ini");
	float x = (float)atof(buf);

	GetPrivateProfileStringA("RISE", keyY, "0.0", buf, sizeof(buf), "./RISE.ini");
	float y = (float)atof(buf);

	GetPrivateProfileStringA("RISE", keyW, "0.0", buf, sizeof(buf), "./RISE.ini");
	float w = (float)atof(buf);

	GetPrivateProfileStringA("RISE", keyH, "0.0", buf, sizeof(buf), "./RISE.ini");
	float h = (float)atof(buf);

	return { x, y, w, h };
}

D3DCOLOR CGToolKit::VisibleCor(int index)
{
	char buf[128];
	char keyColor[32];

	sprintf_s(keyColor, "COLOR%d", index);
	GetPrivateProfileStringA("RISE", keyColor, "255, 255, 255, 255", buf, sizeof(buf), "./RISE.ini");

	int r, g, b, a;
	sscanf_s(buf, "%d, %d, %d, %d", &r, &g, &b, &a);

	return D3DCOLOR_RGBA(b, g, r, a);
}
int CGToolKit::GetPositionScreen()
{
	int a = 0;
	switch (m_Resolution)
	{
		case 0: a = 854; break;
		case 1: a = 854; break;
		case 2: a = 854; break;
		case 3: a = 854; break;
		case 4: a = 854; break;
		case 5: a = 854; break;
		case 6: a = 854; break;
		case 7: a = 854; break;
		case 8: a = 915; break;
		case 9: a = 995; break;
		case 10: a = 1218; break;
		default:a = 640;
		break;
	}
	return a;
}

int CGToolKit::GetCreatePosHeight()
{
	return 430;
}

const char* CharacterCode(int a)
{
	switch (a)
	{
		case 0:  return GlobalText[20];
		case 1:  return GlobalText[25];
		case 2:  return GlobalText[1669];

		case 16: return GlobalText[21];
		case 17: return GlobalText[26];
		case 18: return GlobalText[1668];

		case 32: return GlobalText[22];
		case 33: return GlobalText[27];
		case 34: return GlobalText[1670];

		case 48: return GlobalText[23];
		case 50: return GlobalText[1671];

		case 64: return GlobalText[24];
		case 66: return GlobalText[1672];

		case 80: return GlobalText[1687];
		case 81: return GlobalText[1688];
		case 82: return GlobalText[1689];

		case 96: return GlobalText[3150];
		case 98: return GlobalText[3151];
	}
	return GlobalText[2305];
}

void CGToolKit::WindowFont()
{
	// RISE set font face
	char FontFace[100];
	char FontWeightStr[10];
	char FontHeightStr[50];
	char FontAliasStr[50];

	GetPrivateProfileStringA("FontConfig", "FontName", "Arial", FontFace, sizeof(FontFace), "./RISE.ini");
	GetPrivateProfileStringA("FontConfig", "FontWeight", "0", FontWeightStr, sizeof(FontWeightStr), "./RISE.ini");
	GetPrivateProfileStringA("FontConfig", "FontHeight", "13", FontHeightStr, sizeof(FontHeightStr), "./RISE.ini");
	GetPrivateProfileStringA("FontConfig", "FontAlias", "4", FontAliasStr, sizeof(FontAliasStr), "./RISE.ini");

	int FontWeight = atoi(FontWeightStr);
	this->FontHeightRead = atoi(FontHeightStr);
	int FontAliasRead = atoi(FontAliasStr);
	int FontW = (FontWeight == 0) ? 100 : 700;

	switch (m_Resolution)
	{
		case 0: FontHeight = 13; FontW = 100; break;
		case 1: FontHeight = 13; FontW = 100; break;
		case 2: FontHeight = 13; FontW = 100; break;
		case 3: FontHeight = this->FontHeightRead; break;
		case 4: FontHeight = this->FontHeightRead; break;
		case 5: FontHeight = this->FontHeightRead; break;
		case 6: FontHeight = this->FontHeightRead; break;
		case 7: FontHeight = this->FontHeightRead; break;
		case 8: FontHeight = this->FontHeightRead; break;
		case 9: FontHeight = this->FontHeightRead; break;
		case 10: FontHeight = this->FontHeightRead; break;
	}

	g_hFont			= CreateFont(FontHeight, 0, 0, 0, FontW, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FontAliasRead, DEFAULT_PITCH | FF_DONTCARE, FontFace);
	g_hFontBold		= CreateFont(FontHeight, 0, 0, 0, 700, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FontAliasRead, DEFAULT_PITCH | FF_DONTCARE, FontFace);
	g_hFontBig		= CreateFont(20, 0, 0, 0, 700, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, FontAliasRead, DEFAULT_PITCH | FF_DONTCARE, FontFace);

	g_ConsoleDebug->Write(MCD_NORMAL, "Resolution: %d / FontHeight:%d", m_Resolution, FontHeight);
	g_ConsoleDebug->Write(MCD_NORMAL, "Screen: %dx%d", WindowWidth, WindowHeight);
	g_ConsoleDebug->Write(MCD_NORMAL, "WideRate X:%.2f / WideRate Y:%.2f", g_SetForm->x_fScreenRate_x, g_SetForm->x_fScreenRate_y);

}

char* CGToolKit::QN(RISE Number)
{
	if (Number == 0)
	{
		return strdup("0");
	}

	char OutPut[40];
	char Temp[40];

	int Index = 0;
	int CommaCounter = 0;

	while (Number > 0)
	{
		Temp[Index++] = '0' + (int)(Number % 10);
		Number /= 10;

		if (++CommaCounter == 3 && Number > 0)
		{
			Temp[Index++] = ',';
			CommaCounter = 0;
		}
	}

	int OutPutIndex = 0;
	while (Index > 0)
	{
		OutPut[OutPutIndex++] = Temp[--Index];
	}
	OutPut[OutPutIndex] = '\0';

	return strdup(OutPut);
}

void CGToolKit::ThisFont(HFONT TypeFont, bool Type, int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	if (Len <= 0) return; 

	g_pRenderText->SetFont(TypeFont);
	g_pRenderText->SetTextColor(
		(color >> 24) & 0xFF, // Alpha
		(color >> 16) & 0xFF, // Red
		(color >> 8) & 0xFF,  // Green
		color & 0xFF          // Blue
	);
	g_pRenderText->SetBgColor(
		(bkcolor >> 24) & 0xFF,
		(bkcolor >> 16) & 0xFF,
		(bkcolor >> 8) & 0xFF,
		bkcolor & 0xFF
	);

	char* Context = nullptr;
	char* Line = strtok_s(Buff, "\n", &Context);

	while (Line != NULL)
	{
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align);
		PosY += Height + 2;
		Line = strtok_s(NULL, "\n", &Context);
	}
}
void CGToolKit::ThisFontBig(bool Type,int PosX, int PosY, DWORD color, DWORD bkcolor, int Width, int Height, BYTE Align, LPCTSTR Text, ...)
{
	char Buff[2048];
	int BuffLen = sizeof(Buff) - 1;
	ZeroMemory(Buff, BuffLen);

	va_list args;
	va_start(args, Text);
	int Len = vsprintf_s(Buff, BuffLen, Text, args);
	va_end(args);

	if (Len <= 0) return; 

	g_pRenderText->SetFont(g_hFontBig);
	g_pRenderText->SetTextColor(
		(color >> 24) & 0xFF, // Alpha
		(color >> 16) & 0xFF, // Red
		(color >> 8) & 0xFF,  // Green
		color & 0xFF          // Blue
	);
	g_pRenderText->SetBgColor(
		(bkcolor >> 24) & 0xFF,
		(bkcolor >> 16) & 0xFF,
		(bkcolor >> 8) & 0xFF,
		bkcolor & 0xFF
	);

	char* Context = nullptr;
	char* Line = strtok_s(Buff, "\n", &Context);

	while (Line != NULL)
	{
		g_pRenderText->RenderText(PosX, PosY, Line, Width, Height, Align);
		PosY += Height + 2;
		Line = strtok_s(NULL, "\n", &Context);
	}
	g_pRenderText->SetFont(g_hFont);
}


bool CGToolKit::IsWorkZone(float x, float y, float h, float w)
{
	return MouseX >= x && MouseX <= x + h && MouseY >= y && MouseY <= y + w;
}

int CGToolKit::CheckClass()
{
	return (Hero->Class == 0 || Hero->Class == 1 || Hero->Class == 2 ||
		Hero->Class == 3 || Hero->Class == 4 || Hero->Class == 5 ||
		Hero->Class == 6 || Hero->Class == 8 || Hero->Class == 9 ||
		Hero->Class == 10 || Hero->Class == 13);
}

void CGToolKit::RenderToolTipExt(float x, float y, float w, float h, int tHi, const char *text, int Enable, int Val, float fVal)
{
	EnableAlphaTest();

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	RenderColor(x - 1, y - 1, w + 1, 1);         
	RenderColor(x - 1, y - 1, 1, h + 1);         
	RenderColor(x - 1 + w + 1, y - 1, 1, h + 1); 
	RenderColor(x - 1, y - 1 + h + 1, w + 2, 1); 

	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	RenderColor(x, y, w, h);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	g_pRenderText->SetBgColor(50, 0, 0, 255);
	g_pRenderText->SetTextColor(255, 255, 255, 255);
	g_pRenderText->RenderText(x, y - 1, text, w, tHi, 3);

	if (Enable == 1)
	{
		char szMessage[128];
		sprintf(szMessage, "%d", Val);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->RenderText(x, y + h * 0.5f, szMessage, w, tHi, 3);
	}
	else if (Enable == 2)
	{
		char szMessage[128];
		sprintf(szMessage, "%.2f", fVal);
		g_pRenderText->SetBgColor(0, 0, 0, 0);
		g_pRenderText->SetTextColor(255, 255, 255, 255);
		g_pRenderText->RenderText(x, y + h * 0.5f, szMessage, w, tHi, 3);
	}
}

void CGToolKit::RenderToolTip(float x, float y, float w, float h)
{
	EnableAlphaTest();

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	RenderColor(x - 1, y - 1, w + 1, 1);
	RenderColor(x - 1, y - 1, 1, h + 1);
	RenderColor(x - 1 + w + 1, y - 1, 1, h + 1);
	RenderColor(x - 1, y - 1 + h + 1, w + 2, 1);

	glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
	RenderColor(x, y, w, h);

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);
}

void CGToolKit::RenderItemSlot(int X, int Y, int Doc, int Ngang, int Use)
{
	float v6;
	float v7;
	GLfloat red;
	GLfloat green;
	int j;
	int i;

	for (i = 0; i < Doc; ++i)
	{
		for (j = 0; j < Ngang; ++j)
		{
			EnableAlphaTest();
			glEnable(GL_ALPHA_TEST);
			glColor4f(0.3, 0.3, 0.3, 0.60000002);
			v7 = (float)(Y + 20 * i);
			v6 = (float)(X + 20 * j);
			RenderColor(LODWORD(v6), LODWORD(v7), 20.0, 20.0, 0.0, 0);
			glEnable(GL_TEXTURE_2D);
			glColor3f(1.0, 1.0, 1.0);
			green = (float)(Y + 20 * i);
			red = (float)(X + 20 * j);
			SEASON3B::RenderImage(BITMAP_INTERFACE_NEW_INVENTORY_BASE_BEGIN, LODWORD(red), LODWORD(green), 21.0, 21.0);
			glDisable(GL_BLEND);		
			glEnable(GL_ALPHA_TEST);
			DisableAlphaBlend();
			glColor3f(1.f, 1.f, 1.f);
		}
	}
}

DWORD lastToggleTime = 0;
bool isHoverState = true;

void CGToolKit::CreateRenderButton(int ID, int Time, float x, float y, float w, float h)
{
	static DWORD lastSendTime = 0;
	char szCmd[64];

	int MaxCoinSend = 1000000000;
	if (pMain->ThisCoin[0] <= MaxCoinSend / 100)
	{
		if (SEASON3B::CheckMouseIn((int)x, (int)y, (int)w, (int)h))
		{
			RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 1, 1.0, 64.f / 128.f, 1, 1, 0);

			if (SEASON3B::IsPress(VK_LBUTTON))
			{
				DWORD currentTime = GetTickCount();

				if (currentTime - lastSendTime < (Time * 1000))
				{
					g_pChatListBox->AddText("", "Wait please...", SEASON3B::TYPE_SYSTEM_MESSAGE);
					return;
				}

				lastSendTime = currentTime;

				if (pMain->ThisCoin[0] <= MaxCoinSend)
				{
					sprintf(szCmd, "/wc %s %d %d", Hero->ID, 1, MaxCoinSend);
					SendChat(szCmd);
				}

				if (pMain->ThisCoin[1] <= MaxCoinSend)
				{
					sprintf(szCmd, "/wc %s %d %d", Hero->ID, 2, MaxCoinSend);
					SendChat(szCmd);
				}

				if (pMain->ThisCoin[2] <= MaxCoinSend)
				{
					sprintf(szCmd, "/wc %s %d %d", Hero->ID, 3, MaxCoinSend);
					SendChat(szCmd);
				}

				if (pMain->ThisCoin[3] <= MaxCoinSend)
				{
					sprintf(szCmd, "/wc %s %d %d", Hero->ID, 4, MaxCoinSend);
					SendChat(szCmd);
				}

				if (pMain->ThisCoin[4] <= MaxCoinSend)
				{
					sprintf(szCmd, "/wc %s %d %d", Hero->ID, 5, MaxCoinSend);
					SendChat(szCmd);
				}

				return;
			}
		}
		else
		{
			DWORD currentTime = GetTickCount();
			if (currentTime - lastToggleTime >= 500)
			{
				isHoverState = !isHoverState;
				lastToggleTime = currentTime;
			}
			if (isHoverState)
			{
				RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 1, 1.0, 64.f / 128.f, 1, 1, 0);
			}
			else
			{
				RenderBitmap(ID, x, y, w, h, 0.f, (64.f / 128.f) * 0, 1.0, 64.f / 128.f, 1, 1, 0);
			}
		}
	}
}

unsigned int CRC32(const char* data, size_t length)
{
	unsigned int crc = 0xFFFFFFFF;
	unsigned int table[256];

	for (unsigned int i = 0; i < 256; ++i)
	{
		unsigned int crc_value = i;
		for (unsigned int j = 8; j > 0; --j)
		{
			if (crc_value & 1)
				crc_value = (crc_value >> 1) ^ 0xEDB88320;
			else
				crc_value >>= 1;
		}
		table[i] = crc_value;
	}

	for (size_t i = 0; i < length; ++i)
	{
		unsigned char byte = data[i];
		crc = (crc >> 8) ^ table[(crc & 0xFF) ^ byte];
	}

	return ~crc;
}

bool CheckFileCRC(const std::string& filename, unsigned int expectedCRC)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		return false;
	}

	std::string fileData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (fileData.empty())
	{
		return false;
	}

	unsigned int calculatedCRC = CRC32(fileData.c_str(), fileData.size());

	if (calculatedCRC != expectedCRC)
	{
		return false;
	}

	return true;
}

std::string xorEncryptDecrypt(const std::string& data, char key) {
	std::string output = data;
	for (size_t i = 0; i < output.size(); ++i) {
		output[i] ^= key; 
	}
	return output;
}

int CGToolKit::BmdRISE()
{
	std::string filePath = "\x44\x61\x74\x61\x2F\x50\x6C\x61\x79\x65\x72\x2F\x50\x6C\x61\x79\x65\x72\x2E\x62\x6D\x64";
	const unsigned int legacyPlayerCRC = 0x7D56FCEB;
	const bool validPlayer = CheckFileCRC(filePath, legacyPlayerCRC)
		|| CheckFileCRC(filePath, rise::growlancer::kMergedPlayerBmdCrc32);

	if (!validPlayer)
	{
		std::string encodedMessage = "\x18\x52\x5D\x71\x52\x75\x66\x55\x68\x56\x3A\x2C\x66\x57\x51\x1A\x47\x6A\x52\x50\x6E\x2C\x69\x5D";
		std::string decodedMessage = xorEncryptDecrypt(encodedMessage, 0xAA);

		MessageBox(NULL, decodedMessage.c_str(), "RISE", MB_OK | MB_ICONERROR);
		ExitProcess(1);
	}

	return 0;
}

WORD AdjustedDamage(WORD value)
{
	return (value > 255) ? (255 + (value % 256)) : value;
}
int SafeGetItem(int index)
{
	return CHECK_ITEM(index);
}

bool IsValidPasswordChar(const char* str)
{
	for (int i = 0; str[i] != '\0'; ++i)
	{
		unsigned char c = (unsigned char)str[i];
		if (c < 0x20 || c > 0x7E)
		{
			return false;
		}
	}
	return true;
}

void CGToolKit::RenderMixEffect(float x, float y, int w, int h)
{
	EnableAlphaBlend();

	for (int i = 0; i < h; ++i)
	{
		for (int j = 0; j < w; ++j)
		{
			float fx = x + j * 20 + (rand() % 20);
			float fy = y + i * 20 + (rand() % 20);

			glColor3f((float)(rand() % 6 + 6) * 0.1f, (float)(rand() % 4 + 4) * 0.1f, 0.2f);
			float Rotate = (float)((int)(WorldTime) % 100) * 20.f;
			float Scale = 2.f + (rand() % 10);

			RenderBitmapRotate(BITMAP_SHINY, fx, fy, Scale, Scale, 0);
			RenderBitmapRotate(BITMAP_SHINY, fx, fy, Scale, Scale, Rotate);
			RenderBitmapRotate(BITMAP_SHINY + 1, fx, fy, Scale * 3.f, Scale * 3.f, Rotate);
			RenderBitmapRotate(BITMAP_LIGHT, fx, fy, Scale * 6.f, Scale * 6.f, 0);
		}
	}

	DisableAlphaBlend();
}

bool CGToolKit::GetIndexMonster(int index)
{
	switch (index)
	{
		case 750: return true;
		case 751: return true;
		case 752: return true;
		case 753: return true;
		case 754: return true;
		case 755: return true;
		case 756: return true;
	}

	return false;
}

bool CGToolKit::AllowItemUp(const ITEM* pItem)
{
	if (pItem->Type >= GET_ITEM(0, 0) && pItem->Type <= GET_ITEM(11, 512))
	{
		return true;
	}

	if (pItem->Type == GET_ITEM(13, 8)
		|| pItem->Type == GET_ITEM(13, 9)
		|| pItem->Type == GET_ITEM(13, 12)
		|| pItem->Type == GET_ITEM(13, 13)
		|| pItem->Type == GET_ITEM(13, 21)
		|| pItem->Type == GET_ITEM(13, 22)
		|| pItem->Type == GET_ITEM(13, 23)
		|| pItem->Type == GET_ITEM(13, 24)
		|| pItem->Type == GET_ITEM(13, 25)
		|| pItem->Type == GET_ITEM(13, 26)
		|| pItem->Type == GET_ITEM(13, 27)
		|| pItem->Type == GET_ITEM(13, 28)
		|| pItem->Type == GET_ITEM(13, 30)
		)
	{
		return true;
	}

	if (pItem->Type == GET_ITEM(12, 0)
		|| pItem->Type == GET_ITEM(12, 1)
		|| pItem->Type == GET_ITEM(12, 2)
		|| pItem->Type == GET_ITEM(12, 3)
		|| pItem->Type == GET_ITEM(12, 4)
		|| pItem->Type == GET_ITEM(12, 5)
		|| pItem->Type == GET_ITEM(12, 6)
		|| pItem->Type == GET_ITEM(12, 36)
		|| pItem->Type == GET_ITEM(12, 37)
		|| pItem->Type == GET_ITEM(12, 38)
		|| pItem->Type == GET_ITEM(12, 39)
		|| pItem->Type == GET_ITEM(12, 40)
		|| pItem->Type == GET_ITEM(12, 41)
		|| pItem->Type == GET_ITEM(12, 42)
		|| pItem->Type == GET_ITEM(12, 43)
		|| pItem->Type == GET_ITEM(12, 49)
		|| pItem->Type == GET_ITEM(12, 50)

		)
	{
		return true;
	}

	if (pItem->Type >= GET_ITEM(12, 150) && pItem->Type <= GET_ITEM(12, 512))
	{
		return true;
	}

	if (pItem->Type >= GET_ITEM(13, 300) && pItem->Type <= GET_ITEM(13, 512))
	{
		return true;
	}

	return false;
}

bool CGToolKit::IsItemStackCustom(const ITEM* pItem)
{
	if (   pItem->Type == GET_ITEM(14, 13)
		|| pItem->Type == GET_ITEM(14, 14)
		|| pItem->Type == GET_ITEM(14, 16)
		|| pItem->Type == GET_ITEM(14, 22)
		|| pItem->Type == GET_ITEM(14, 31)
		|| pItem->Type == GET_ITEM(14, 41)
		|| pItem->Type == GET_ITEM(14, 42)
		|| pItem->Type == GET_ITEM(14, 43)
		|| pItem->Type == GET_ITEM(14, 44)
		|| pItem->Type == GET_ITEM(12, 15)
		|| pItem->Type == GET_ITEM(12, 32)
		|| pItem->Type == GET_ITEM(12, 34)
		|| pItem->Type == GET_ITEM(14, 52)
		|| pItem->Type == GET_ITEM(14, 157)
		|| pItem->Type == GET_ITEM(14, 158)
		|| pItem->Type == GET_ITEM(14, 159)
		|| pItem->Type == GET_ITEM(14, 234)
		|| pItem->Type == GET_ITEM(14, 235)

		|| pItem->Type == GET_ITEM(14, 244)
		|| pItem->Type == GET_ITEM(14, 245)
		|| pItem->Type == GET_ITEM(14, 246)
		|| pItem->Type == GET_ITEM(14, 247)
		|| pItem->Type == GET_ITEM(14, 248)
		|| pItem->Type == GET_ITEM(14, 249)
		|| pItem->Type == GET_ITEM(14, 250)
		|| pItem->Type == GET_ITEM(14, 251)
		|| pItem->Type == GET_ITEM(14, 252)
		|| pItem->Type == GET_ITEM(14, 259)

		|| pItem->Type == GET_ITEM(14, 220)
		|| pItem->Type == GET_ITEM(14, 221)
		|| pItem->Type == GET_ITEM(14, 222)
		|| pItem->Type == GET_ITEM(14, 223)
		|| pItem->Type == GET_ITEM(14, 224)
		|| pItem->Type == GET_ITEM(14, 225)
		|| pItem->Type == GET_ITEM(14, 226)
		|| pItem->Type == GET_ITEM(14, 227)
		|| pItem->Type == GET_ITEM(14, 228)
		|| pItem->Type == GET_ITEM(14, 229)
		|| pItem->Type == GET_ITEM(14, 230)

		|| pItem->Type == GET_ITEM(14, 236)
		|| pItem->Type == GET_ITEM(14, 237)
		|| pItem->Type == GET_ITEM(14, 238)
		|| pItem->Type == GET_ITEM(14, 239)
		|| (pItem->Type == GET_ITEM(14, 11) && ((pItem->Level >> 3) & 15) == 8)
		|| (pItem->Type == GET_ITEM(14, 11) && ((pItem->Level >> 3) & 15) == 9)
		|| (pItem->Type == GET_ITEM(14, 11) && ((pItem->Level >> 3) & 15) == 10)
		|| (pItem->Type == GET_ITEM(14, 11) && ((pItem->Level >> 3) & 15) == 11)
		|| (pItem->Type == GET_ITEM(14, 11) && ((pItem->Level >> 3) & 15) == 12)
		)
	{
		return true;
	}

	return false;
}

DWORD GetGoldValue(DWORD Gold)
{
	if (Gold <= 99999)
	{
		return 0xFFFFFFFF;
	}
	else if (Gold <= 999999)
	{
		return 0x1DCC2EFF;
	}
	else if (Gold < 7000000)
	{
		return 0xFF6F00FF;
	}
	else
	{
		return 0xFF0000FF;
	}
}

bool IsFilteredWeapon(int WeaponType)
{
	if (WeaponType >= MODEL_SWORD && WeaponType <= MODEL_SHIELD + 512) 
	{
		return true;
	}

	return false;
}


std::string EncryptXOR(const std::string& input)
{
	std::string output = input;
	for (size_t i = 0; i + 1 < output.size(); i += 2)
	{
		output[i] ^= 0xA;
		output[i + 1] ^= 0xF;
	}
	return output;
}
