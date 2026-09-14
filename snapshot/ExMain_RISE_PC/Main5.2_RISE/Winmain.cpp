#include "stdafx.h"
#ifdef RISE_SLAYER_RUNTIME_QA
#include "RISE/SlayerRuntimeQA.h"
#endif
#include <shellapi.h>

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <locale.h>
#include <zmouse.h>
#include "UIWindows.h"
#include "UIManager.h"
#include "ZzzOpenglUtil.h"
#include "ZzzTexture.h"
#include "ZzzOpenData.h"
#include "ZzzScene.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzInterface.h"
#include "ZzzInventory.h"
#include "zzzLodTerrain.h"
#include "DSPlaySound.h"
#include "wsclientinline.h"
#include "Resource.h"
#include "RISE/CProtect.h"
#include <imm.h>
#include "zzzpath.h"
#include "Nprotect.h"
#include "Local.h"
#include "PersonalShopTitleImp.h"
#include "UIMapName.h"
#include "./ExternalObject/leaf/ExceptionHandler.h"
#include "./Utilities/Dump/CrashReporter.h"
#include "./Utilities/Log/muConsoleDebug.h"
#include "ProtectSysKey.h"
#include "CBTMessageBox.h"
#include "./ExternalObject/leaf/regkey.h"
#include "CSChaosCastle.h"
#include "GMHellas.h"
#include <io.h>
#include "Input.h"
#include "./Time/Timer.h"
#include "UIMng.h"
#include "GameCensorship.h"
#include "w_MapHeaders.h"
#include "w_PetProcess.h"
#include <ThemidaInclude.h>
#include "MultiLanguage.h"
#include <iostream>
#include "RISE/WideData.h"
#include "RISE/TrayModeNew.h"
#include "RISE/CustomNpcName.h"
#include "RISE/CustomWing.h"
#include "RISE/CustomMonster.h"
#include "RISE/CustomPet.h"

#include "RISE/ZzzToolKit.h"
#include <tlhelp32.h>
#include <MonkSystem.h>
#include <RISE/CustomIconBuff.h>
#include <RISE/CustomSetEffect.h>
#include <wzAudio.h>
#include <RISE/CustomItemColorName.h>
#include <RISE/MainProtocol.h>
#pragma comment(lib, "wzAudio.lib")
#include <objbase.h>
#include <gdiplus.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <ProtectSend/AntiStreamClient.h>
extern RISE_SHARED_INFO g_RISESharedInfo;
#ifdef RISE_SLAYER_RUNTIME_QA
#define SLAYER_QA_STEP(text) rise::slayerqa::AppendRuntimeQALog(text)
#else
#define SLAYER_QA_STEP(text) ((void)0)
#endif
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "dbghelp.lib")
using namespace Gdiplus;

CUIMercenaryInputBox* g_pMercenaryInputBox = NULL;
CUITextInputBox* g_pSingleTextInputBox = NULL;
CUITextInputBox* g_pSinglePasswdInputBox = NULL;
int g_iChatInputType = 1;
extern BOOL g_bIMEBlock;

CChatRoomSocketList* g_pChatRoomSocketList = NULL;

CMultiLanguage* pMultiLanguage = NULL;

extern DWORD g_dwTopWindow;
CUIManager* g_pUIManager = NULL;
CUIMapName* g_pUIMapName = NULL;

float Time_Effect = 0;
bool ashies = false;
int weather = rand() % 3;

HWND      g_hWnd = NULL;
HINSTANCE g_hInst = NULL;
HDC       g_hDC = NULL;
HGLRC     g_hRC = NULL;

HFONT     g_hFont = NULL;
HFONT     g_hFontBold = NULL;
HFONT     g_hFontBig = NULL;

CTimer* g_pTimer = new CTimer();

bool      Destroy = false;
bool      ActiveIME = false;

BYTE* RendomMemoryDump;
ITEM_ATTRIBUTE* ItemAttRibuteMemoryDump;
CHARACTER* CharacterMemoryDump;

int       RandomTable[100];

BOOL g_bMinimizedEnabled = FALSE;
int g_iScreenSaverOldValue = 60 * 15;

#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
BOOL g_bUseWindowMode = TRUE;
#endif

char Mp3FileName[256];


void StopMp3(char* Name, BOOL bEnforce)
{
	if (!m_MusicOnOff && !bEnforce) return;

	if (Mp3FileName[0] != NULL)
	{
		if (strcmp(Name, Mp3FileName) == 0) {
			wzAudioStop();
			Mp3FileName[0] = NULL;
		}
	}
}

void PlayMp3(char* Name, BOOL bEnforce)
{
	if (Destroy) return;
	if (!m_MusicOnOff && !bEnforce) return;

	if (strcmp(Name, Mp3FileName) == 0)
	{
		return;
	}
	else
	{
		wzAudioPlay(Name, 1);
		strcpy(Mp3FileName, Name);
	}
}

bool IsEndMp3()
{
	if (100 == wzAudioGetStreamOffsetRange())
		return true;
	return false;
}

int GetMp3PlayPosition()
{
	return wzAudioGetStreamOffsetRange();
}

extern int  LogIn;
extern char LogInID[];

void CheckHack(void)
{
	SendCheck();
}

GLvoid KillGLWindow(GLvoid)
{
	if (g_hRC)
	{
#ifdef jdk_shader_local330
		if (Models != NULL)
		{
			for (int i = MODEL_LOGO; i < MAX_MODELS; ++i)
			{
				Models[i].ReleaseGpuMeshes();
			}
		}
		OGL330::Release();
#endif

		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, "Release Of DC And RC Failed.", "Error", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(g_hRC))
		{
			MessageBox(NULL, "Release Rendering Context Failed.", "Error", MB_OK | MB_ICONINFORMATION);
		}

		g_hRC = NULL;
	}

	if (g_hDC && !ReleaseDC(g_hWnd, g_hDC))
	{
		MessageBox(NULL, "OpenGL Release Error.", "Error", MB_OK | MB_ICONINFORMATION);
		g_hDC = NULL;
	}

	if (g_bUseWindowMode == FALSE)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}
}


BOOL GetFileNameOfFilePath(char* lpszFile, char* lpszPath)
{
#ifdef RISE_SLAYER_PORT
	char modulePath[MAX_PATH];
	const DWORD length = GetModuleFileNameA(NULL, modulePath, MAX_PATH);
	if (length == 0 || length >= MAX_PATH)
	{
		lpszFile[0] = '\0';
		return FALSE;
	}
	const char* separator = strrchr(modulePath, '\\');
	strcpy(lpszFile, separator ? separator + 1 : modulePath);
	return TRUE;
#else
	int iFind = (int)'\\';
	char* lpFound = lpszPath;
	char* lpOld = lpFound;
	while (lpFound)
	{
		lpOld = lpFound;
		lpFound = strchr(lpFound + 1, iFind);
	}

	if (strchr(lpszPath, iFind))
	{
		strcpy(lpszFile, lpOld + 1);
	}
	else
	{
		strcpy(lpszFile, lpOld);
	}

	BOOL bCheck = TRUE;
	for (char* lpTemp = lpszFile; bCheck; ++lpTemp)
	{
		switch (*lpTemp)
		{
		case '\"':
		case '\\':
		case '/':
		case ' ':
			*lpTemp = '\0';
		case '\0':
			bCheck = FALSE;
			break;
		}
	}

	return (TRUE);
#endif
}

HANDLE g_hMainExe = INVALID_HANDLE_VALUE;


BOOL OpenMainExe(void)
{
#ifdef _DEBUG
	return (TRUE);
#endif
	char lpszFile[MAX_PATH];
	char* lpszCommandLine = GetCommandLine();
	GetFileNameOfFilePath(lpszFile, lpszCommandLine);

	g_hMainExe = CreateFile((char*)lpszFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	return (INVALID_HANDLE_VALUE != g_hMainExe);
}

void CloseMainExe(void)
{
	CloseHandle(g_hMainExe);
}

WORD DecryptCheckSumKey(WORD wSource)
{
	WORD wAcc = wSource ^ 0xB479;
	return ((wAcc >> 10) << 4) | (wAcc & 0xF);
}

DWORD GenerateCheckSum(BYTE* pbyBuffer, DWORD dwSize, WORD wKey)
{
	auto dwKey = (DWORD)wKey;
	DWORD dwResult = dwKey << 9;
	for (DWORD dwChecked = 0; dwChecked <= dwSize - 4; dwChecked += 4)
	{
		DWORD dwTemp;
		memcpy(&dwTemp, pbyBuffer + dwChecked, sizeof(DWORD));

		switch ((dwChecked / 4 + wKey) % 3)
		{
		case 0:
			dwResult ^= dwTemp;
			break;
		case 1:
			dwResult += dwTemp;
			break;
		case 2:
			dwResult <<= (dwTemp % 11);
			dwResult ^= dwTemp;
			break;
		}

		if (0 == (dwChecked % 4))
		{
			dwResult ^= ((dwKey + dwResult) >> ((dwChecked / 4) % 16 + 3));
		}
	}

	return (dwResult);
}

DWORD GetCheckSum(WORD wKey)
{
	wKey = DecryptCheckSumKey(wKey);

	char lpszFile[MAX_PATH];

	strcpy(lpszFile, "data\\local\\Gameguard.csr");

	HANDLE hFile = CreateFile((char*)lpszFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		return (0);
	}

	DWORD dwSize = GetFileSize(hFile, NULL);
	BYTE* pbyBuffer = new BYTE[dwSize];
	DWORD dwNumber;
	ReadFile(hFile, pbyBuffer, dwSize, &dwNumber, 0);
	CloseHandle(hFile);

	DWORD dwCheckSum = GenerateCheckSum(pbyBuffer, dwSize, wKey);
	delete[] pbyBuffer;

	return (dwCheckSum);
}


BOOL GetFileVersion(char* lpszFileName, WORD* pwVersion)
{
	DWORD dwHandle;
	DWORD dwLen = GetFileVersionInfoSize(lpszFileName, &dwHandle);
	if (dwLen <= 0)
	{
		return (FALSE);
	}

	BYTE* pbyData = new BYTE[dwLen];
	if (!GetFileVersionInfo(lpszFileName, dwHandle, dwLen, pbyData))
	{
		delete[] pbyData;
		return (FALSE);
	}

	VS_FIXEDFILEINFO* pffi;
	UINT uLen;
	if (!VerQueryValue(pbyData, "\\", (LPVOID*)&pffi, &uLen))
	{
		delete[] pbyData;
		return (FALSE);
	}

	pwVersion[0] = HIWORD(pffi->dwFileVersionMS);
	pwVersion[1] = LOWORD(pffi->dwFileVersionMS);
	pwVersion[2] = HIWORD(pffi->dwFileVersionLS);
	pwVersion[3] = LOWORD(pffi->dwFileVersionLS);

	delete[] pbyData;
	return (TRUE);
}

extern PATH* path;

void DestroyWindow()
{

	leaf::CRegKey regkey;
	regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config");
	regkey.WriteDword("VolumeLevel", g_pOption->GetVolumeLevel());

	CUIMng::Instance().Release();

	if (g_hFont)		DeleteObject((HGDIOBJ)g_hFont);
	if (g_hFontBig)		DeleteObject((HGDIOBJ)g_hFontBig);
	if (g_hFontBold)		DeleteObject((HGDIOBJ)g_hFontBold);

	ReleaseCharacters();

	delete path;

	SAFE_DELETE(GateAttribute);

	SAFE_DELETE(SkillAttribute);

	SAFE_DELETE(CharacterMachine);

	DeleteWaterTerrain();

	gMapManager.DeleteObjects();

	// Object.
	for (int i = MODEL_LOGO; i < MAX_MODELS; i++)
	{
		Models[i].Release();
	}

#ifdef jdk_shader_local330
	OGL330::Release();
#endif

	// Bitmap
	Bitmaps.UnloadAllImages();

	SAFE_DELETE_ARRAY(CharacterMemoryDump);
	SAFE_DELETE_ARRAY(ItemAttRibuteMemoryDump);
	SAFE_DELETE_ARRAY(RendomMemoryDump);
	SAFE_DELETE_ARRAY(ModelsDump);

	SAFE_DELETE(g_pMercenaryInputBox);
	SAFE_DELETE(g_pSingleTextInputBox);
	SAFE_DELETE(g_pSinglePasswdInputBox);

	SAFE_DELETE(g_pChatRoomSocketList);
	SAFE_DELETE(g_pUIMapName);
	SAFE_DELETE(g_pTimer);
	SAFE_DELETE(g_pUIManager);

	SAFE_DELETE(pMultiLanguage);
	BoostRest(g_BuffSystem);
	BoostRest(g_MapProcess);
	BoostRest(g_petProcess);

	HWND shWnd = FindWindow(NULL, "MuPlayer");
	if (shWnd)
		SendMessage(shWnd, WM_DESTROY, 0, 0);
}
void DestroySound()
{
	for (int i = 0; i < MAX_BUFFER; i++)
		ReleaseBuffer(i);

	FreeDirectSound();
	wzAudioDestroy();
}

int g_iInactiveTime = 0;
int g_iNoMouseTime = 0;
int g_iInactiveWarning = 0;
bool g_bWndActive = false;
bool HangulDelete = false;
int Hangul = 0;
bool g_bEnterPressed = false;

int g_iMousePopPosition_x = 0;
int g_iMousePopPosition_y = 0;

extern bool EnableFastInput;
void MainScene(HDC hDC);

LONG FAR PASCAL WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_SYSKEYDOWN:
	{
		return 0;
	}
	break;
#if defined PROTECT_SYSTEMKEY && defined NDEBUG
#ifndef FOR_WORK
	case WM_SYSCOMMAND:
	{
		if (wParam == SC_KEYMENU || wParam == SC_SCREENSAVE)
		{
			return 0;
		}
	}
	break;
#endif // !FOR_WORK
#endif // PROTECT_SYSTEMKEY && NDEBUG
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
#ifdef ACTIVE_FOCUS_OUT
			if (g_bUseWindowMode == FALSE)
#endif	// ACTIVE_FOCUS_OUT
				g_bWndActive = false;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
			if (g_bUseWindowMode == TRUE)
			{
				MouseLButton = false;
				MouseLButtonPop = false;

				MouseRButton = false;
				MouseRButtonPop = false;
				MouseRButtonPush = false;
				MouseLButtonDBClick = false;
				MouseMButton = false;
				MouseMButtonPop = false;
				MouseMButtonPush = false;
				MouseWheel = 0;
			}
#endif
		}
		else
		{
			g_bWndActive = true;
		}
		break;
	case WM_TIMER:

		switch (wParam)
		{
		case HACK_TIMER:
			VM_START
				CheckHack();
			VM_END
				break;
		case WINDOWMINIMIZED_TIMER:
			PostMessage(g_hWnd, WM_CLOSE, 0, 0);
			break;
		case CHATCONNECT_TIMER:
			g_pFriendMenu->SendChatRoomConnectCheck();
			break;
		case SLIDEHELP_TIMER:
			if (g_bWndActive)
			{
				if (g_pSlideHelpMgr)
					g_pSlideHelpMgr->CreateSlideText();
			}
			break;
		}
		break;
	case WM_USER_MEMORYHACK:

		KillGLWindow();
		break;
	case WM_NPROTECT_EXIT_TWO:
		SendHackingChecked(0x04, 0);
		SetTimer(g_hWnd, WINDOWMINIMIZED_TIMER, 1 * 1000, NULL);
		MessageBox(NULL, GlobalText[16], "Error", MB_OK);
		break;
	case WM_ASYNCSELECTMSG:
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT:
			break;
		case FD_READ:
			SocketClient.nRecv();
			break;
		case FD_WRITE:
			SocketClient.FDWriteSend();
			break;
		case FD_CLOSE:
			g_pChatListBox->AddText("", GlobalText[3], SEASON3B::TYPE_SYSTEM_MESSAGE);
#ifdef CONSOLE_DEBUG
			switch (WSAGETSELECTERROR(lParam))
			{
			case WSAECONNRESET:
				g_ConsoleDebug->Write(MCD_ERROR, "The connection was reset by the remote side.");
				break;
			case WSAECONNABORTED:
				g_ConsoleDebug->Write(MCD_ERROR, "The connection was terminated due to a time-out or other failure.");
				break;
			}
#endif // CONSOLE_DEBUG

			SocketClient.Close();

			CUIMng::Instance().PopUpMsgWin(MESSAGE_SERVER_LOST);
			break;
		}
		break;
	case WM_CTLCOLOREDIT:
		SetBkColor((HDC)wParam, RGB(0, 0, 0));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (LRESULT)GetStockObject(BLACK_BRUSH);
		break;
	case WM_ERASEBKGND:
		return TRUE;
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}
	return 0;
	break;
	case WM_DESTROY:
	{
		Destroy = true;
		SocketClient.Close();

		DestroySound();

		KillGLWindow();
		CloseMainExe();
		PostQuitMessage(0);
	}
	break;
	case WM_SETCURSOR:
		ShowCursor(false);
		break;
#if (defined WINDOWMODE)
	case WM_SIZE:
		if (SIZE_MINIMIZED == wParam && g_bUseWindowMode == FALSE)
		{
			if (!(g_bMinimizedEnabled))
			{
				DWORD dwMess[SIZE_ENCRYPTION_KEY];
				for (int i = 0; i < SIZE_ENCRYPTION_KEY; ++i)
				{
					dwMess[i] = GetTickCount();
				}
				g_SimpleModulusCS.LoadKeyFromBuffer((BYTE*)dwMess, FALSE, FALSE, FALSE, TRUE);
			}
		}
		break;
#else
#ifdef NDEBUG
#ifndef FOR_WORK
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
	case WM_SIZE:
		if (SIZE_MINIMIZED == wParam
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
			&& g_bUseWindowMode == FALSE
#endif
			)
		{
			if (!(g_bMinimizedEnabled))
			{
				SendHackingChecked(0x05, 0);
				DWORD dwMess[SIZE_ENCRYPTION_KEY];
				for (int i = 0; i < SIZE_ENCRYPTION_KEY; ++i)
				{
					dwMess[i] = GetTickCount();
				}
				g_SimpleModulusCS.LoadKeyFromBuffer((BYTE*)dwMess, FALSE, FALSE, FALSE, TRUE);
			}
		}
		break;
#endif
#endif
#endif
#endif	//WINDOWMODE(#else)
		//-----------------------------
	default:
		if (msg >= WM_CHATROOMMSG_BEGIN && msg < WM_CHATROOMMSG_END)
			g_pChatRoomSocketList->ProcessSocketMessage(msg - WM_CHATROOMMSG_BEGIN, WSAGETSELECTEVENT(lParam));
		break;
	}

	MouseLButtonDBClick = false;
	if (MouseLButtonPop == true && (g_iMousePopPosition_x != MouseX || g_iMousePopPosition_y != MouseY))
		MouseLButtonPop = false;
	switch (msg)
	{
	case WM_MOUSEMOVE:
	{
		MouseX = (float)LOWORD(lParam) / g_SetForm->x_fScreenRate_x;
		MouseY = (float)HIWORD(lParam) / g_SetForm->x_fScreenRate_y;
		if (MouseX < 0)
			MouseX = 0;

		if (MouseX > GetWindowsX())
			MouseX = GetWindowsX();

		if (MouseY < 0)
			MouseY = 0;

		if (MouseY > GetWindowsY())
			MouseY = GetWindowsY();
	}
	break;
	case WM_LBUTTONDOWN:
		g_iNoMouseTime = 0;
		MouseLButtonPop = false;
		if (!MouseLButton)
			MouseLButtonPush = true;
		MouseLButton = true;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		SetCapture(g_hWnd);
#endif
		break;
	case WM_LBUTTONUP:
		g_iNoMouseTime = 0;
		MouseLButtonPush = false;

		MouseLButtonPop = true;
		MouseLButton = false;
		g_iMousePopPosition_x = MouseX;
		g_iMousePopPosition_y = MouseY;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		ReleaseCapture();
#endif
		break;
	case WM_RBUTTONDOWN:
		g_iNoMouseTime = 0;
		MouseRButtonPop = false;
		if (!MouseRButton) MouseRButtonPush = true;
		MouseRButton = true;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		SetCapture(g_hWnd);
#endif
		break;
	case WM_RBUTTONUP:
		g_iNoMouseTime = 0;
		MouseRButtonPush = false;
		if (MouseRButton) MouseRButtonPop = true;
		MouseRButton = false;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		ReleaseCapture();
#endif
		break;
	case WM_LBUTTONDBLCLK:
		g_iNoMouseTime = 0;
		MouseLButtonDBClick = true;
		break;
	case WM_MBUTTONDOWN:
		g_iNoMouseTime = 0;
		MouseMButtonPop = false;
		if (!MouseMButton) MouseMButtonPush = true;
		MouseMButton = true;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		SetCapture(g_hWnd);
#endif
		break;
	case WM_MBUTTONUP:
		g_iNoMouseTime = 0;
		MouseMButtonPush = false;
		if (MouseMButton) MouseMButtonPop = true;
		MouseRButton = false;
#if defined USER_WINDOW_MODE || (defined WINDOWMODE)
		ReleaseCapture();
#endif
		break;
	case WM_MOUSEWHEEL:
	{
		MouseWheel = (short)HIWORD(wParam) / WHEEL_DELTA;
	}
	break;
	case WM_IME_NOTIFY:
	{
		if (g_iChatInputType == 1)
		{
			switch (wParam)
			{
			case IMN_SETCONVERSIONMODE:
				if (GetFocus() == g_hWnd)
				{
					CheckTextInputBoxIME(IME_CONVERSIONMODE);
				}
				break;
			case IMN_SETSENTENCEMODE:
				if (GetFocus() == g_hWnd)
				{
					CheckTextInputBoxIME(IME_SENTENCEMODE);
				}
				break;
			default:
				break;
			}
		}
	}
	break;

	case WM_CHAR:
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			SetEnterPressed(true);
		}
		break;
		}
	}
	break;
	}

	if (g_BuffSystem) {
		LRESULT result;
		TheBuffStateSystem().HandleWindowMessage(msg, wParam, lParam, result);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool CreateOpenglWindow()
{
	PIXELFORMATDESCRIPTOR pfd;

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 16;

	if (!(g_hDC = GetDC(g_hWnd)))
	{
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Get DC Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	GLuint PixelFormat;

	if (!(PixelFormat = ChoosePixelFormat(g_hDC, &pfd)))
	{
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Choose Pixel Format Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(g_hDC, PixelFormat, &pfd))
	{
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Set Pixel Format Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(g_hRC = wglCreateContext(g_hDC)))
	{
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Create Context Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(g_hDC, g_hRC))
	{
		KillGLWindow();
		MessageBox(NULL, GlobalText[4], "OpenGL Make Current Error.", MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	glewExperimental = GL_TRUE;
	glewInit();
	glGetError();

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
	return true;
}

HWND StartWindow(HINSTANCE hInstance, int nCmdShow)
{
	char* WindowNameMU = new char[MAX_LEN_CHAR];
	memset(WindowNameMU, 0, sizeof(char) * MAX_LEN_CHAR);
#ifdef RISE_SLAYER_PORT
	strcpy_s(WindowNameMU, MAX_LEN_CHAR, "Engine-Slayer S21");
#else
	memcpy(WindowNameMU, gProtect->m_MainInfo.WindowName, sizeof(char) * MAX_LEN_CHAR);
#endif
	WNDCLASS wndClass;
	HWND hWnd;

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = WindowNameMU;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(NULL, "Windows aplication error!", "Aplication Error", MB_ICONERROR);
		return 0;
	}

	if (g_bUseWindowMode == TRUE)
	{
		RECT rc = { 0, 0, WindowWidth, WindowHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CLIPCHILDREN, NULL);
		hWnd = CreateWindow(
			WindowNameMU, WindowNameMU,
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_BORDER | WS_CLIPCHILDREN,
			(GetSystemMetrics(SM_CXSCREEN) - rc.right) / 2,
			(GetSystemMetrics(SM_CYSCREEN) - rc.bottom) / 2,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL, NULL, hInstance, NULL);
	}
	else
	{
		hWnd = CreateWindowEx(
			WS_EX_TOPMOST | WS_EX_APPWINDOW,
			WindowNameMU, WindowNameMU,
			WS_POPUP,
			0, 0,
			WindowWidth,
			WindowHeight,
			NULL, NULL, hInstance, NULL);
	}
	return hWnd;
}

char m_ID[11];
char m_PW[11];
char m_Version[11];
char m_ExeVersion[11];
int  m_SoundOnOff;
int  m_MusicOnOff;
int  m_Resolution;
int	m_nColorDepth;
int	g_iRenderTextType = 0;

char g_aszMLSelection[MAX_LANGUAGE_NAME_LENGTH] = { '\0' };
std::string g_strSelectedML = "";
char ReadLang[16] = { '\0' };

BOOL OpenInitFile()
{
	char szIniFilePath[256 + 20] = "";
	char szCurrentDir[256];

	GetCurrentDirectory(256, szCurrentDir);

	strcpy(szIniFilePath, szCurrentDir);
	if (szCurrentDir[strlen(szCurrentDir) - 1] == '\\')
		strcat(szIniFilePath, "RISE.ini");
	else strcat(szIniFilePath, "\\RISE.ini");

	GetPrivateProfileString("Version", "MainCode", "", m_Version, 11, szIniFilePath);

	char szReadLang[16] = { '\0' };

	GetPrivateProfileStringA("FontConfig", "Lang", "0", szReadLang, sizeof(szReadLang), szIniFilePath);

	ReadLang[0] = '\0';

	if (_stricmp(szReadLang, "0") != 0 && _stricmp(szReadLang, "En") != 0)
	{
		strncpy(ReadLang, szReadLang, sizeof(ReadLang) - 1);
		ReadLang[sizeof(ReadLang) - 1] = '\0';
	}

	szServerIpAddress = new char[32];

	memset(szServerIpAddress, 0, sizeof(char) * 32);
	memcpy(szServerIpAddress, gProtect->m_ConnectInfo.IpAddress, sizeof(char) * 32);

	g_ServerPort = gProtect->m_ConnectInfo.IpAddressPort;

	Version[0] = gProtect->m_MainInfo.ClientVersion[0] + 1;
	Version[1] = gProtect->m_MainInfo.ClientVersion[2] + 2;
	Version[2] = gProtect->m_MainInfo.ClientVersion[3] + 3;
	Version[3] = gProtect->m_MainInfo.ClientVersion[5] + 4;
	Version[4] = gProtect->m_MainInfo.ClientVersion[6] + 5;

	memcpy(Serial, gProtect->m_MainInfo.ClientSerial, sizeof(Serial));

	strncpy(g_RISESharedInfo.ServerIP, szServerIpAddress, sizeof(g_RISESharedInfo.ServerIP) - 1);
	g_RISESharedInfo.hWnd = g_hWnd;

	// --- System Info Logging ---
	gAntiStreamWork.LogSystemInfo();

	gProtect->PreparetEnc();

	char* lpszCommandLine = GetCommandLine();
	char lpszFile[MAX_PATH];
	if (GetFileNameOfFilePath(lpszFile, lpszCommandLine))
	{
		WORD wVersion[4];
		if (GetFileVersion(lpszFile, wVersion))
		{
			char lpszMinorVersion[3] = "a";
			sprintf(m_ExeVersion, "%d.%02d", wVersion[0], wVersion[1]);
			if (wVersion[2] > 0)
			{
				if (wVersion[2] > 26)
				{
					lpszMinorVersion[0] = 'A';
					lpszMinorVersion[0] += (wVersion[2] - 27);
					lpszMinorVersion[1] = '+';
				}
				else
				{
					lpszMinorVersion[0] += (wVersion[2] - 1);
				}
				strcat(m_ExeVersion, lpszMinorVersion);
			}
		}
		else
		{
			strcpy(m_ExeVersion, m_Version);
		}
	}
	else
	{
		strcpy(m_ExeVersion, m_Version);
	}



	m_ID[0] = '\0';
	m_PW[0] = '\0';
	m_SoundOnOff = 1;
	m_MusicOnOff = 1;
	m_Resolution = 0;
	m_nColorDepth = 0;

	HKEY hKey;
	DWORD dwDisp;
	DWORD dwSize;
	if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisp))
	{
		dwSize = 11;
		if (RegQueryValueEx(hKey, "ID", 0, NULL, (LPBYTE)m_ID, &dwSize) != ERROR_SUCCESS)
		{
		}
		dwSize = 11;
		if (RegQueryValueEx(hKey, "PW", 0, NULL, (LPBYTE)m_PW, &dwSize) != ERROR_SUCCESS)
		{
		}
		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "SoundOnOff", 0, NULL, (LPBYTE)&m_SoundOnOff, &dwSize) != ERROR_SUCCESS)
		{
			m_SoundOnOff = true;
		}
		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "MusicOnOff", 0, NULL, (LPBYTE)&m_MusicOnOff, &dwSize) != ERROR_SUCCESS)
		{
			m_MusicOnOff = false;
		}
		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "Resolution", 0, NULL, (LPBYTE)&m_Resolution, &dwSize) != ERROR_SUCCESS)
			m_Resolution = 1;

		if (0 == m_Resolution)
			m_Resolution = 1;

		if (RegQueryValueEx(hKey, "ColorDepth", 0, NULL, (LPBYTE)&m_nColorDepth, &dwSize) != ERROR_SUCCESS)
		{
			m_nColorDepth = 0;
		}
		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "TextOut", 0, NULL, (LPBYTE)&g_iRenderTextType, &dwSize) != ERROR_SUCCESS)
		{
			g_iRenderTextType = 0;
		}

		g_iChatInputType = 1;

		dwSize = sizeof(int);
		if (RegQueryValueEx(hKey, "WindowMode", 0, NULL, (LPBYTE)&g_bUseWindowMode, &dwSize) != ERROR_SUCCESS)
		{
			g_bUseWindowMode = FALSE;
		}

		dwSize = MAX_LANGUAGE_NAME_LENGTH;
		if (RegQueryValueEx(hKey, "LangSelection", 0, NULL, (LPBYTE)g_aszMLSelection, &dwSize) != ERROR_SUCCESS)
		{
			strcpy(g_aszMLSelection, "Eng");
		}
		g_strSelectedML = g_aszMLSelection;
	}
	RegCloseKey(hKey);

	g_SetForm->SetDefaultScreen(m_Resolution);

	gTrayMode.Load();

	return TRUE;
}

BOOL Util_CheckOption(char* lpszCommandLine, unsigned char cOption, char* lpszString)
{
	unsigned char cComp[2];
	cComp[0] = cOption; cComp[1] = cOption;
	if (islower((int)cOption))
	{
		cComp[1] = toupper((int)cOption);
	}
	else if (isupper((int)cOption))
	{
		cComp[1] = tolower((int)cOption);
	}

	int nFind = (int)'/';
	auto* lpFound = (unsigned char*)lpszCommandLine;
	while (lpFound)
	{
		lpFound = (unsigned char*)strchr((char*)(lpFound + 1), nFind);
		if (lpFound && (*(lpFound + 1) == cComp[0] || *(lpFound + 1) == cComp[1]))
		{
			if (lpszString)
			{
				int nCount = 0;
				for (unsigned char* lpSeek = lpFound + 2; *lpSeek != ' ' && *lpSeek != '\0'; lpSeek++)
				{
					nCount++;
				}

				memcpy(lpszString, lpFound + 2, nCount);
				lpszString[nCount] = '\0';
			}
			return (TRUE);
		}
	}

	return (FALSE);
}

BOOL UpdateFile(char* lpszOld, char* lpszNew)
{
	SetFileAttributes(lpszOld, FILE_ATTRIBUTE_NORMAL);
	SetFileAttributes(lpszNew, FILE_ATTRIBUTE_NORMAL);

	DWORD dwStartTickCount = ::GetTickCount();
	while (::GetTickCount() - dwStartTickCount < 5000) {
		if (CopyFile(lpszOld, lpszNew, FALSE))
		{
			DeleteFile(lpszOld);
			return (TRUE);
		}
	}
	return (FALSE);
}



BOOL KillExeProcess(char* lpszExe)
{
	HANDLE hProcessSnap = NULL;
	BOOL bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (stricmp(pe32.szExeFile, lpszExe) == 0)
			{
				HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

				if (process)
				{
					TerminateProcess(process, 0);
				}
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = TRUE;
	}
	else
		bRet = FALSE;

	CloseHandle(hProcessSnap);

	return bRet;
}

char g_lpszCmdURL[50];
BOOL GetConnectServerInfo(PSTR szCmdLine, char* lpszURL, WORD* pwPort)
{
	char lpszTemp[256] = { 0, };
	if (Util_CheckOption(szCmdLine, 'y', lpszTemp))
	{
		BYTE bySuffle[] = { 0x0C, 0x07, 0x03, 0x13 };

		for (int i = 0; i < (int)strlen(lpszTemp); i++)
			lpszTemp[i] -= bySuffle[i % 4];
		strcpy(lpszURL, lpszTemp);

		if (Util_CheckOption(szCmdLine, 'z', lpszTemp))
		{
			for (int j = 0; j < (int)strlen(lpszTemp); j++)
				lpszTemp[j] -= bySuffle[j % 4];
			*pwPort = atoi(lpszTemp);
		}
		return (TRUE);
	}
	if (!Util_CheckOption(szCmdLine, 'u', lpszTemp))
	{
		return (FALSE);
	}
	strcpy(lpszURL, lpszTemp);
	if (!Util_CheckOption(szCmdLine, 'p', lpszTemp))
	{
		return (FALSE);
	}
	*pwPort = atoi(lpszTemp);

	return (TRUE);
}


BOOL g_bInactiveTimeChecked = FALSE;
void MoveObject(OBJECT* o);
void WriteCrashBreadcrumb(const char* message);

static bool GetCrashDumpDirectory(char* path, size_t pathSize)
{
	if (GetModuleFileNameA(NULL, path, static_cast<DWORD>(pathSize)) == 0)
		return false;

	char* slash = strrchr(path, '\\');
	if (slash == NULL)
		return false;

	*(slash + 1) = '\0';
	if (FAILED(StringCchCatA(path, pathSize, "CrashDumps")))
		return false;

	if (!CreateDirectoryA(path, NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		return false;

	return true;
}

static bool AppendCrashJsonEscaped(char* output, size_t outputSize, size_t* length,
	const char* value, size_t valueLimit)
{
	if (output == NULL || length == NULL || *length >= outputSize)
		return false;

	const char* source = value ? value : "";
	for (size_t i = 0; source[i] != '\0' && i < valueLimit; ++i)
	{
		const unsigned char ch = static_cast<unsigned char>(source[i]);
		const char* escaped = NULL;
		char unicodeEscape[7] = { 0 };
		switch (ch)
		{
		case '\\': escaped = "\\\\"; break;
		case '"': escaped = "\\\""; break;
		case '\r': escaped = "\\r"; break;
		case '\n': escaped = "\\n"; break;
		case '\t': escaped = "\\t"; break;
		default:
			if (ch < 0x20)
			{
				StringCchPrintfA(unicodeEscape, _countof(unicodeEscape), "\\u%04X", ch);
				escaped = unicodeEscape;
			}
			break;
		}

		if (escaped != NULL)
		{
			const size_t escapedLength = strlen(escaped);
			if (*length + escapedLength >= outputSize)
				return false;
			memcpy(output + *length, escaped, escapedLength);
			*length += escapedLength;
		}
		else
		{
			if (*length + 1 >= outputSize)
				return false;
			output[(*length)++] = static_cast<char>(ch);
		}
	}

	output[*length] = '\0';
	return true;
}

static void WritePendingCrashReport(const char* dumpDirectory, const char* stem,
	const SYSTEMTIME& time, DWORD code, PVOID address, const char* modulePath,
	DWORD moduleOffset, BOOL dumpWritten, DWORD dumpError)
{
	char sessionPath[MAX_PATH] = { 0 };
	char pendingPath[MAX_PATH] = { 0 };
	if (FAILED(StringCchPrintfA(sessionPath, _countof(sessionPath),
		"%s\\Session.log", dumpDirectory)) ||
		FAILED(StringCchPrintfA(pendingPath, _countof(pendingPath),
		"%s\\Pending_%s.json", dumpDirectory, stem)))
		return;

	char sessionTail[8193] = { 0 };
	HANDLE session = CreateFileA(sessionPath, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if (session != INVALID_HANDLE_VALUE)
	{
		LARGE_INTEGER size = { 0 };
		if (GetFileSizeEx(session, &size) && size.QuadPart > 0)
		{
			const DWORD bytesToRead = static_cast<DWORD>(size.QuadPart < 8192 ? size.QuadPart : 8192);
			LARGE_INTEGER offset;
			offset.QuadPart = size.QuadPart - bytesToRead;
			SetFilePointerEx(session, offset, NULL, FILE_BEGIN);
			DWORD bytesRead = 0;
			ReadFile(session, sessionTail, bytesToRead, &bytesRead, NULL);
			sessionTail[bytesRead] = '\0';
		}
		CloseHandle(session);
	}

	const char* moduleName = strrchr(modulePath ? modulePath : "", '\\');
	moduleName = moduleName ? moduleName + 1 : (modulePath ? modulePath : "unknown");
	const char* dumpName = stem;
	char json[16384] = { 0 };
	HRESULT prefixResult = StringCchPrintfA(json, _countof(json),
		"{\"source\":\"windows_exception\",\"platform\":\"windows\","
		"\"timestamp\":\"%04u-%02u-%02uT%02u:%02u:%02u.%03u\","
		"\"exceptionCode\":\"0x%08lX\",\"address\":\"%p\","
		"\"module\":\"",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
		time.wSecond, time.wMilliseconds, code, address);
	if (FAILED(prefixResult))
		return;

	size_t length = strlen(json);
	if (!AppendCrashJsonEscaped(json, _countof(json), &length, moduleName, MAX_PATH))
		return;

	char details[512] = { 0 };
	if (FAILED(StringCchPrintfA(details, _countof(details),
		"\",\"moduleOffset\":\"0x%08lX\",\"processId\":%lu,\"threadId\":%lu,"
		"\"dumpFile\":\"%s.dmp\",\"dumpWritten\":%s,\"dumpError\":%lu,"
		"\"sessionTail\":\"",
		moduleOffset, GetCurrentProcessId(), GetCurrentThreadId(), dumpName,
		dumpWritten ? "true" : "false", dumpError)))
		return;

	const size_t detailsLength = strlen(details);
	if (length + detailsLength >= _countof(json))
		return;
	memcpy(json + length, details, detailsLength);
	length += detailsLength;
	json[length] = '\0';
	AppendCrashJsonEscaped(json, _countof(json), &length, sessionTail, 8192);
	if (length + 3 >= _countof(json))
		return;
	memcpy(json + length, "\"}\r\n", 4);
	length += 4;

	HANDLE pending = CreateFileA(pendingPath, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pending == INVALID_HANDLE_VALUE)
		return;
	DWORD written = 0;
	WriteFile(pending, json, static_cast<DWORD>(length), &written, NULL);
	FlushFileBuffers(pending);
	CloseHandle(pending);
}

enum
{
	CRASH_REPORT_PROTOCOL_VERSION = 1,
	CRASH_REPORT_PLATFORM_PC = 1,
	CRASH_REPORT_MAX_SIZE = 16 * 1024,
	CRASH_REPORT_CHUNK_SIZE = 1024,
	CRASH_REPORT_RESULT_READY = 0,
	CRASH_REPORT_RESULT_ACCEPTED = 1,
	CRASH_REPORT_RESULT_RETRY = 2,
	CRASH_REPORT_RESULT_REJECTED = 3,
};

#pragma pack(push, 1)
struct PMSG_CRASH_REPORT_BEGIN_SEND
{
	PSBMSG_HEAD header;
	BYTE version;
	BYTE platform;
	DWORD reportId;
	WORD totalSize;
	DWORD checksum;
	BYTE chunkCount;
};

struct PMSG_CRASH_REPORT_CHUNK_SEND
{
	PSWMSG_HEAD header;
	DWORD reportId;
	BYTE chunkIndex;
	BYTE chunkCount;
	WORD payloadSize;
	BYTE payload[CRASH_REPORT_CHUNK_SIZE];
};

struct PMSG_CRASH_REPORT_ACK_RECV
{
	PSBMSG_HEAD header;
	BYTE version;
	BYTE result;
	DWORD reportId;
};
#pragma pack(pop)

struct CRASH_REPORT_UPLOAD_STATE
{
	char filePath[MAX_PATH];
	BYTE* data;
	DWORD size;
	DWORD reportId;
	DWORD checksum;
	BYTE chunkCount;
	bool waitingForAck;
};

static CRASH_REPORT_UPLOAD_STATE g_crashUpload = { 0 };

static DWORD CalculateCrashReportCRC32(const BYTE* data, DWORD size)
{
	DWORD crc = 0xFFFFFFFF;
	for (DWORD i = 0; i < size; ++i)
	{
		crc ^= data[i];
		for (int bit = 0; bit < 8; ++bit)
			crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320 : 0);
	}
	return crc ^ 0xFFFFFFFF;
}

static void ResetCrashReportUpload()
{
	if (g_crashUpload.data != NULL)
		HeapFree(GetProcessHeap(), 0, g_crashUpload.data);
	memset(&g_crashUpload, 0, sizeof(g_crashUpload));
}

static void QuarantineCrashReport(const char* filePath)
{
	if (filePath == NULL || filePath[0] == '\0')
		return;
	const char* fileName = strrchr(filePath, '\\');
	fileName = fileName ? fileName + 1 : filePath;
	if (strncmp(fileName, "Pending_", 8) != 0)
		return;

	char rejectedPath[MAX_PATH] = { 0 };
	const size_t directoryLength = static_cast<size_t>(fileName - filePath);
	if (directoryLength >= _countof(rejectedPath))
		return;
	memcpy(rejectedPath, filePath, directoryLength);
	StringCchCatA(rejectedPath, _countof(rejectedPath), "Rejected_");
	StringCchCatA(rejectedPath, _countof(rejectedPath), fileName + 8);
	MoveFileExA(filePath, rejectedPath, MOVEFILE_REPLACE_EXISTING);
}

static void SendCrashReportChunks()
{
	if (g_crashUpload.data == NULL || g_crashUpload.size == 0)
		return;

	for (BYTE chunkIndex = 0; chunkIndex < g_crashUpload.chunkCount; ++chunkIndex)
	{
		const DWORD offset = chunkIndex * CRASH_REPORT_CHUNK_SIZE;
		const DWORD remaining = g_crashUpload.size - offset;
		const WORD payloadSize = static_cast<WORD>(remaining < CRASH_REPORT_CHUNK_SIZE ?
			remaining : CRASH_REPORT_CHUNK_SIZE);
		PMSG_CRASH_REPORT_CHUNK_SEND packet = { 0 };
		const WORD packetSize = static_cast<WORD>(offsetof(PMSG_CRASH_REPORT_CHUNK_SEND, payload) + payloadSize);
		packet.header.set(0xF4, 0xE1, packetSize);
		packet.reportId = g_crashUpload.reportId;
		packet.chunkIndex = chunkIndex;
		packet.chunkCount = g_crashUpload.chunkCount;
		packet.payloadSize = payloadSize;
		memcpy(packet.payload, g_crashUpload.data + offset, payloadSize);
		DataSend(reinterpret_cast<BYTE*>(&packet), packetSize);
	}
	WriteCrashBreadcrumb("Crash upload: chunks sent to ConnectServer");
}

void CrashReportOnConnectServer()
{
	ResetCrashReportUpload();
	char directory[MAX_PATH] = { 0 };
	if (!GetCrashDumpDirectory(directory, _countof(directory)))
		return;

	char pattern[MAX_PATH] = { 0 };
	StringCchPrintfA(pattern, _countof(pattern), "%s\\Pending_Crash_*.json", directory);
	WIN32_FIND_DATAA findData = { 0 };
	HANDLE find = FindFirstFileA(pattern, &findData);
	if (find == INVALID_HANDLE_VALUE)
		return;

	char filePath[MAX_PATH] = { 0 };
	StringCchPrintfA(filePath, _countof(filePath), "%s\\%s", directory, findData.cFileName);
	FindClose(find);

	HANDLE file = CreateFileA(filePath, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return;

	const DWORD size = GetFileSize(file, NULL);
	if (size == INVALID_FILE_SIZE || size == 0 || size > CRASH_REPORT_MAX_SIZE)
	{
		CloseHandle(file);
		QuarantineCrashReport(filePath);
		WriteCrashBreadcrumb("Crash upload: pending report has invalid size");
		return;
	}

	BYTE* data = static_cast<BYTE*>(HeapAlloc(GetProcessHeap(), 0, size));
	DWORD bytesRead = 0;
	const bool readOk = data != NULL && ReadFile(file, data, size, &bytesRead, NULL) && bytesRead == size;
	CloseHandle(file);
	if (!readOk)
	{
		if (data != NULL) HeapFree(GetProcessHeap(), 0, data);
		return;
	}

	StringCchCopyA(g_crashUpload.filePath, _countof(g_crashUpload.filePath), filePath);
	g_crashUpload.data = data;
	g_crashUpload.size = size;
	g_crashUpload.checksum = CalculateCrashReportCRC32(data, size);
	g_crashUpload.reportId = g_crashUpload.checksum ^ GetTickCount() ^ GetCurrentProcessId();
	g_crashUpload.chunkCount = static_cast<BYTE>((size + CRASH_REPORT_CHUNK_SIZE - 1) / CRASH_REPORT_CHUNK_SIZE);
	g_crashUpload.waitingForAck = true;

	PMSG_CRASH_REPORT_BEGIN_SEND packet = { 0 };
	packet.header.set(0xF4, 0xE0, sizeof(packet));
	packet.version = CRASH_REPORT_PROTOCOL_VERSION;
	packet.platform = CRASH_REPORT_PLATFORM_PC;
	packet.reportId = g_crashUpload.reportId;
	packet.totalSize = static_cast<WORD>(size);
	packet.checksum = g_crashUpload.checksum;
	packet.chunkCount = g_crashUpload.chunkCount;
	DataSend(reinterpret_cast<BYTE*>(&packet), sizeof(packet));
	WriteCrashBreadcrumb("Crash upload: begin sent to ConnectServer");
}

void CrashReportReceiveAck(const BYTE* receiveBuffer, int size)
{
	if (receiveBuffer == NULL || size != sizeof(PMSG_CRASH_REPORT_ACK_RECV))
		return;
	const PMSG_CRASH_REPORT_ACK_RECV* packet =
		reinterpret_cast<const PMSG_CRASH_REPORT_ACK_RECV*>(receiveBuffer);
	if (packet->version != CRASH_REPORT_PROTOCOL_VERSION ||
		!g_crashUpload.waitingForAck || packet->reportId != g_crashUpload.reportId)
		return;

	if (packet->result == CRASH_REPORT_RESULT_READY)
	{
		SendCrashReportChunks();
		return;
	}

	if (packet->result == CRASH_REPORT_RESULT_ACCEPTED)
	{
		DeleteFileA(g_crashUpload.filePath);
		WriteCrashBreadcrumb("Crash upload: report accepted by ConnectServer");
	}
	else if (packet->result == CRASH_REPORT_RESULT_RETRY)
	{
		WriteCrashBreadcrumb("Crash upload: ConnectServer rate limit, report retained");
	}
	else
	{
		QuarantineCrashReport(g_crashUpload.filePath);
		WriteCrashBreadcrumb("Crash upload: ConnectServer rejected report");
	}
	ResetCrashReportUpload();
}

static void ConfigureWindowsCrashDumps()
{
	char dumpDirectory[MAX_PATH] = { 0 };
	if (!GetCrashDumpDirectory(dumpDirectory, _countof(dumpDirectory)))
		return;

	HKEY key = NULL;
	if (RegCreateKeyExA(HKEY_CURRENT_USER,
		"Software\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\Engine.exe",
		0, NULL, 0, KEY_SET_VALUE, NULL, &key, NULL) != ERROR_SUCCESS)
		return;

	const DWORD dumpType = 2;
	const DWORD dumpCount = 10;
	RegSetValueExA(key, "DumpFolder", 0, REG_EXPAND_SZ,
		reinterpret_cast<const BYTE*>(dumpDirectory),
		static_cast<DWORD>(strlen(dumpDirectory) + 1));
	RegSetValueExA(key, "DumpType", 0, REG_DWORD,
		reinterpret_cast<const BYTE*>(&dumpType), sizeof(dumpType));
	RegSetValueExA(key, "DumpCount", 0, REG_DWORD,
		reinterpret_cast<const BYTE*>(&dumpCount), sizeof(dumpCount));
	RegCloseKey(key);
}

void WriteCrashBreadcrumb(const char* message)
{
	char dumpDirectory[MAX_PATH] = { 0 };
	if (!GetCrashDumpDirectory(dumpDirectory, _countof(dumpDirectory)))
		return;

	char path[MAX_PATH] = { 0 };
	if (FAILED(StringCchPrintfA(path, _countof(path), "%s\\Session.log", dumpDirectory)))
		return;

	HANDLE file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE)
		return;

	SYSTEMTIME time;
	GetLocalTime(&time);
	char line[1024] = { 0 };
	StringCchPrintfA(line, _countof(line),
		"%04u-%02u-%02u %02u:%02u:%02u.%03u pid=%lu tid=%lu %s\r\n",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
		time.wSecond, time.wMilliseconds, GetCurrentProcessId(),
		GetCurrentThreadId(), message ? message : "(null)");
	DWORD written = 0;
	WriteFile(file, line, static_cast<DWORD>(strlen(line)), &written, NULL);
	FlushFileBuffers(file);
	CloseHandle(file);
}

bool ExceptionCallback(_EXCEPTION_POINTERS* pExceptionInfo)
{
	char dumpDirectory[MAX_PATH] = { 0 };
	if (!GetCrashDumpDirectory(dumpDirectory, _countof(dumpDirectory)))
		return false;

	SYSTEMTIME time;
	GetLocalTime(&time);
	char stem[96] = { 0 };
	StringCchPrintfA(stem, _countof(stem), "Crash_%04u%02u%02u_%02u%02u%02u_%lu",
		time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
		time.wSecond, GetCurrentProcessId());

	char dumpPath[MAX_PATH] = { 0 };
	char logPath[MAX_PATH] = { 0 };
	StringCchPrintfA(dumpPath, _countof(dumpPath), "%s\\%s.dmp", dumpDirectory, stem);
	StringCchPrintfA(logPath, _countof(logPath), "%s\\%s.log", dumpDirectory, stem);

	BOOL dumpWritten = FALSE;
	DWORD dumpError = ERROR_SUCCESS;
	HANDLE dump = CreateFileA(dumpPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (dump != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION info = { 0 };
		info.ThreadId = GetCurrentThreadId();
		info.ExceptionPointers = pExceptionInfo;
		info.ClientPointers = FALSE;
		dumpWritten = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
			dump, static_cast<MINIDUMP_TYPE>(MiniDumpWithThreadInfo |
				MiniDumpWithIndirectlyReferencedMemory), &info, NULL, NULL);
		if (!dumpWritten)
			dumpError = GetLastError();
		CloseHandle(dump);
	}
	else
	{
		dumpError = GetLastError();
	}

	DWORD code = 0;
	PVOID address = NULL;
	if (pExceptionInfo != NULL && pExceptionInfo->ExceptionRecord != NULL)
	{
		code = pExceptionInfo->ExceptionRecord->ExceptionCode;
		address = pExceptionInfo->ExceptionRecord->ExceptionAddress;
	}

	HMODULE module = NULL;
	char modulePath[MAX_PATH] = "unknown";
	DWORD moduleOffset = 0;
	if (address != NULL && GetModuleHandleExA(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		reinterpret_cast<LPCSTR>(address), &module))
	{
		GetModuleFileNameA(module, modulePath, _countof(modulePath));
		moduleOffset = static_cast<DWORD>(reinterpret_cast<BYTE*>(address) -
			reinterpret_cast<BYTE*>(module));
	}

	HANDLE log = CreateFileA(logPath, GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (log != INVALID_HANDLE_VALUE)
	{
		char text[2048] = { 0 };
		StringCchPrintfA(text, _countof(text),
			"RISE client crash\r\nTime=%04u-%02u-%02u %02u:%02u:%02u.%03u\r\n"
			"Exception=0x%08lX Address=%p\r\nModule=%s Offset=0x%08lX\r\n"
			"Dump=%s Written=%s Error=%lu\r\n",
			time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute,
			time.wSecond, time.wMilliseconds, code, address, modulePath,
			moduleOffset, dumpPath, dumpWritten ? "true" : "false", dumpError);
		DWORD written = 0;
		WriteFile(log, text, static_cast<DWORD>(strlen(text)), &written, NULL);
		FlushFileBuffers(log);
		CloseHandle(log);
	}

	WritePendingCrashReport(dumpDirectory, stem, time, code, address, modulePath,
		moduleOffset, dumpWritten, dumpError);

	return dumpWritten || log != INVALID_HANDLE_VALUE;
}

void StartAddress(LPVOID lpThreadParameter)
{
	HANDLE CurProcess, MaxProcess;

	while (TRUE)
	{
		Sleep(5000);

		CurProcess = GetCurrentProcess();
		SetProcessWorkingSetSize(CurProcess, 0xFFFFFFFF, 0xFFFFFFFF);

		MaxProcess = GetCurrentProcess();
		SetThreadPriority(MaxProcess, -2);
	}
}

extern "C"
{
	_declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;
}

// Force NVidia Optimus to use NVidia GPU on drivers 302 and later.
extern "C" 
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

int __stdcall APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int nCmdShow)
{
#ifdef RISE_SLAYER_PORT
	// Keep Mix.bmd and every relative Data path bound to the isolated package,
	// even when a launcher or QA runner starts the process from another cwd.
	wchar_t slayerModule[MAX_PATH] = {};
	const DWORD slayerLength = GetModuleFileNameW(NULL, slayerModule, MAX_PATH);
	if (!slayerLength || slayerLength >= MAX_PATH)
		return 1;
	wchar_t* slayerSlash = wcsrchr(slayerModule, L'\\');
	if (!slayerSlash)
		return 1;
	*slayerSlash = L'\0';
	if (!SetCurrentDirectoryW(slayerModule))
		return 1;
#ifdef RISE_SLAYER_RUNTIME_QA
	rise::slayerqa::AppendRuntimeQALog("startup isolated-cwd-ready title=Engine-Slayer-S21");
#endif
#endif
	/*if (strstr(szCmdLine, "RISELaunch") == NULL)
	{
		ShellExecute(NULL, "open", "Launcher.exe", NULL, NULL, SW_SHOWNORMAL);
		return 0;
	}*/

	pMain->WinHook(hInstance);
	SLAYER_QA_STEP("winmain WinHook returned");

	MSG msg;
	ConfigureWindowsCrashDumps();
	WriteCrashBreadcrumb("WinMain: crash diagnostics installed");
	leaf::AttachExceptionHandler(ExceptionCallback);

	char lpszExeVersion[256] = "unknown";
	char* lpszCommandLine = GetCommandLine();
	char lpszFile[MAX_PATH];
	WORD wVersion[4] = { 0, };
	if (GetFileNameOfFilePath(lpszFile, lpszCommandLine))
	{
		if (GetFileVersion(lpszFile, wVersion))
		{
			sprintf(lpszExeVersion, "%d.%02d", wVersion[0], wVersion[1]);
			if (wVersion[2] > 0)
			{
				char lpszMinorVersion[2] = "a";
				lpszMinorVersion[0] += (wVersion[2] - 1);
				strcat(lpszExeVersion, lpszMinorVersion);
			}
		}
	}
	g_ConsoleDebug->Write(MCD_NORMAL, "Mu Online 6.3 (Ver: %d.%d.%d.%d)", wVersion[0], wVersion[1], wVersion[2], wVersion[3]);

	VM_START
	WORD wPortNumber;
	if (GetConnectServerInfo(szCmdLine, g_lpszCmdURL, &wPortNumber))
	{
		szServerIpAddress = g_lpszCmdURL;
		g_ServerPort = wPortNumber;
	}
	VM_END

	if (!OpenMainExe())
	{
		return false;
	}
	SLAYER_QA_STEP("winmain OpenMainExe returned");

	VM_START
	g_SimpleModulusCS.LoadEncryptionKey("Data\\Enc1.dat");
	g_SimpleModulusSC.LoadDecryptionKey("Data\\Dec2.dat");
	VM_END
	SLAYER_QA_STEP("winmain encryption keys loaded");

	if (OpenInitFile() == FALSE)
	{
		return false;
	}
	SLAYER_QA_STEP("winmain OpenInitFile returned");

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)StartAddress, 0, 0, 0);

	pMultiLanguage = new CMultiLanguage(g_strSelectedML);

	if (g_iChatInputType == 1)
		ShowCursor(FALSE);

	DEVMODE DevMode;
	DEVMODE* pDevmodes;
	int nModes = 0;
	while (EnumDisplaySettings(NULL, nModes, &DevMode)) nModes++;
	pDevmodes = new DEVMODE[nModes + 1];
	nModes = 0;
	while (EnumDisplaySettings(NULL, nModes, &pDevmodes[nModes])) nModes++;
	DWORD dwBitsPerPel = 16;
	for (int n1 = 0; n1 < nModes; n1++)
	{
		if (pDevmodes[n1].dmBitsPerPel == 16 && m_nColorDepth == 0) {
			dwBitsPerPel = 16; break;
		}
		if (pDevmodes[n1].dmBitsPerPel == 24 && m_nColorDepth == 1) {
			dwBitsPerPel = 24; break;
		}
		if (pDevmodes[n1].dmBitsPerPel == 32 && m_nColorDepth == 1) {
			dwBitsPerPel = 32; break;
		}
	}

	delete[] pDevmodes;
	g_hInst = hInstance;
	g_hWnd = StartWindow(hInstance, nCmdShow);
	SLAYER_QA_STEP("winmain StartWindow returned");
	if (!CreateOpenglWindow())
	{
		return FALSE;
	}
	SLAYER_QA_STEP("winmain CreateOpenglWindow returned");
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	if (IsVSyncAvailable())
	{
		EnableVSync();
		InitVSync();
	}
	pKit->WindowFont();

	setlocale(LC_ALL, "english");

	CInput::Instance().Create(g_hWnd, WindowWidth, WindowHeight);

	g_pNewUISystem->Create();
	SLAYER_QA_STEP("winmain NewUISystem created");
	if (m_MusicOnOff)
	{
		wzAudioCreate(g_hWnd);
		wzAudioOption(WZAOPT_STOPBEFOREPLAY, 1);
	}
	if (m_SoundOnOff)
	{
		InitDirectSound(g_hWnd);
		leaf::CRegKey regkey;
		regkey.SetKey(leaf::CRegKey::_HKEY_CURRENT_USER, "SOFTWARE\\Webzen\\Mu\\Config");
		DWORD value;
		if (!regkey.ReadDword("VolumeLevel", value))
		{
			value = 5;
			regkey.WriteDword("VolumeLevel", value);
		}
		if (value < 0 || value >= 10)
			value = 5;

		g_pOption->SetVolumeLevel(int(value));
		SetEffectVolumeLevel(g_pOption->GetVolumeLevel());
	}
	SetTimer(g_hWnd, HACK_TIMER, 20 * 1000, NULL);
	SetTimer(g_hWnd, MUHELPER_TIMER, 250 /* ms */, MUHelper::CMuHelper::TimerProc);
	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++)
		RandomTable[i] = rand() % 360;

	RendomMemoryDump = new BYTE[rand() % 100 + 1];
	GateAttribute = new GATE_ATTRIBUTE[MAX_GATES];
	SkillAttribute = new SKILL_ATTRIBUTE[MAX_SKILLS];

	ItemAttRibuteMemoryDump = new ITEM_ATTRIBUTE[MAX_ITEM + 1024];
	ItemAttribute = ((ITEM_ATTRIBUTE*)ItemAttRibuteMemoryDump) + rand() % 1024;

	CharacterMemoryDump = new CHARACTER[MAX_CHARACTERS_CLIENT + 1 + 128];
	CharactersClient = ((CHARACTER*)CharacterMemoryDump) + rand() % 128;
	CharacterMachine = new CHARACTER_MACHINE;
	memset(GateAttribute, 0, sizeof(GATE_ATTRIBUTE) * (MAX_GATES));
	memset(ItemAttribute, 0, sizeof(ITEM_ATTRIBUTE) * (MAX_ITEM));
	memset(SkillAttribute, 0, sizeof(SKILL_ATTRIBUTE) * (MAX_SKILLS));
	memset(CharacterMachine, 0, sizeof(CHARACTER_MACHINE));
	CharacterAttribute = &CharacterMachine->Character;
	CharacterMachine->Init();
	SLAYER_QA_STEP("winmain CharacterMachine initialized");
	Hero = &CharactersClient[0];
	if (g_iChatInputType == 1)
	{
		g_pMercenaryInputBox = new CUIMercenaryInputBox;
		g_pSingleTextInputBox = new CUITextInputBox;
		g_pSinglePasswdInputBox = new CUITextInputBox;
	}

	g_pChatRoomSocketList = new CChatRoomSocketList;
	g_pUIManager = new CUIManager;
	g_pUIMapName = new CUIMapName;


#ifdef jdk_shader_local330
	OGL330::Init();
#endif
	g_BuffSystem = BuffStateSystem::Make();
	g_MapProcess = MapProcess::Make();
	g_petProcess = PetProcess::Make();
	CUIMng::Instance().Create();
	if (g_iChatInputType == 1)
	{
		g_pMercenaryInputBox->Init(g_hWnd);
		g_pSingleTextInputBox->Init(g_hWnd, 200, 20);
		g_pSinglePasswdInputBox->Init(g_hWnd, 200, 20, 9, TRUE);
		g_pSingleTextInputBox->SetState(UISTATE_HIDE);
		g_pSinglePasswdInputBox->SetState(UISTATE_HIDE);
		g_pMercenaryInputBox->SetFont(g_hFont);
		g_pSingleTextInputBox->SetFont(g_hFont);
		g_pSinglePasswdInputBox->SetFont(g_hFont);
		g_bIMEBlock = FALSE;
		HIMC  hIMC = ImmGetContext(g_hWnd);
		ImmSetConversionStatus(hIMC, IME_CMODE_ALPHANUMERIC, IME_SMODE_NONE);
		ImmReleaseContext(g_hWnd, hIMC);
		SaveIMEStatus();
		g_bIMEBlock = TRUE;
	}
	if (g_bUseWindowMode == FALSE)
	{
		int nOldVal;
		SystemParametersInfo(SPI_SCREENSAVERRUNNING, 1, &nOldVal, 0);
		SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &g_iScreenSaverOldValue, 0);
		SystemParametersInfo(SPI_SETSCREENSAVETIMEOUT, 300 * 60, NULL, 0);
	}
	SLAYER_QA_STEP("winmain entering main loop");

#ifdef SAVE_PACKET
	DeleteFile(PACKET_SAVE_FILE);
#endif
#if defined PROTECT_SYSTEMKEY && defined NDEBUG
#ifndef FOR_WORK
	ProtectSysKey::AttachProtectSysKey(g_hInst, g_hWnd);
#endif // !FOR_WORK
#endif // PROTECT_SYSTEMKEY && NDEBUG
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//Scene
			if (g_bUseWindowMode == TRUE)
			{
				Scene(g_hDC);
			}
			else if (g_bWndActive)
			{
				Scene(g_hDC);
			}
		}
		ProtocolCompiler();
		g_pChatRoomSocketList->ProtocolCompile();
	}
	DestroyWindow();

	return msg.wParam;
}


