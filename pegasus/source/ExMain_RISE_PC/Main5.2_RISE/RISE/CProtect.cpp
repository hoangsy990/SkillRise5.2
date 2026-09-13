#include "stdafx.h"
#include "RISE/CProtect.h"
#if PROTECT_IP
#include "..\\..\\Include\\CCRC32.H"
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include "Util.h"
CProtect* gProtect;

CProtect::CProtect()
{
	EncDecKey[0] = 0;
	EncDecKey[1] = 0;
}

unsigned char BuffRam[] =
	"%s error, please contact RISE Developer.\n"
	"Please contact the RISE administrator.";

unsigned char xORKey[2] = 
{
	0x20, 0x20
};

bool CProtect::ReadMainFile(char* Path)
{
	CCRC32 CRC32;

	if (CRC32.FileCRC(Path, &this->m_ClientFileCRC, 1024) == 0)
	{
		return 0;
	}

	HANDLE zFile = CreateFile(Path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (zFile == INVALID_HANDLE_VALUE) { return 0; }

	if (GetFileSize(zFile, 0) != sizeof(MAIN_FILE_INFO))
	{
		CloseHandle(zFile);
		return 0;
	}

	DWORD OutSize = 0;

	if (ReadFile(zFile, &this->m_MainInfo, sizeof(MAIN_FILE_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(zFile);
		return 0;
	}

	for (int i = 0; i < sizeof(MAIN_FILE_INFO); i++)
	{
		((BYTE*)&this->m_MainInfo)[i] ^= xORKey[i % 2];
	}

	CloseHandle(zFile);
	return 1;
}

bool CProtect::ReadConnectFile(char* Path)
{
	HANDLE zFile = CreateFile(Path, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE, 0);

	if (zFile == INVALID_HANDLE_VALUE) { return 0; }

	if (GetFileSize(zFile, 0) != sizeof(CONNECT_IP_INFO))
	{
		CloseHandle(zFile);
		return 0;
	}

	DWORD OutSize = 0;

	if (ReadFile(zFile, &this->m_ConnectInfo, sizeof(CONNECT_IP_INFO), &OutSize, 0) == 0)
	{
		CloseHandle(zFile);
		return 0;
	}

	for (int i = 0; i < sizeof(CONNECT_IP_INFO); i++)
	{
		((BYTE*)&this->m_ConnectInfo)[i] ^= xORKey[i % 2];
	}

	CloseHandle(zFile);
	return 1;
}
char* CProtect::GetScreenPath()
{
	return this->m_MainInfo.ScreenShotPath;
}

WORD CProtect::AddressPort()
{
	return this->m_ConnectInfo.IpAddressPort;
}

void CProtect::PreparetEnc()
{
	WORD EncDec = 0;

	for (int n = 0; n < sizeof(m_MainInfo.CustomerName); n++)
	{
		EncDec += (BYTE)(m_MainInfo.CustomerName[n] ^ m_MainInfo.ClientSerial[(n % sizeof(m_MainInfo.ClientSerial))]);
	}

	this->EncDecKey[0] = (BYTE)0xF1;
	this->EncDecKey[1] = (BYTE)0x1A;

	this->EncDecKey[0] += LOBYTE(EncDec);
	this->EncDecKey[1] += HIBYTE(EncDec);
}

bool IsProcessRunning(const char* ProcessName, int maxProcess)
{
	PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	int Temp = 0;
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			if (_tcsicmp(pe32.szExeFile, ProcessName) == 0)
			{
				Temp++;
				if (Temp > maxProcess)
				{
					CloseHandle(hSnapshot);
					return true;
				}
			}
		} while (Process32Next(hSnapshot, &pe32));
	}

	CloseHandle(hSnapshot);
	return false;
}

void CProtect::CheckPluginFile()
{
	if (this->m_MainInfo.PluginCRC32 == 0)
	{
		return;
	}

	CCRC32 CRC32;
	DWORD PluginCRC32;

	HMODULE module = LoadLibrary(this->m_MainInfo.PluginName);

	if (CRC32.FileCRC(this->m_MainInfo.PluginName, &PluginCRC32, 1024) == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_01, ERROR_01, MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	
	if (this->m_MainInfo.PluginCRC32 != PluginCRC32)
	{
		MessageBoxW(0, RENDER_UNICODE_02, ERROR_02, MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}
	 
	if (module == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_03, ERROR_03, MB_OK | MB_ICONSTOP);
		ExitProcess(0);
	}

	if (module != NULL)
	{
		void (*EntryProc)() = (void(*)())GetProcAddress(module, "EntryProc");
		if (EntryProc != 0)
		{
			EntryProc();
		}
	}
}

void CProtect::CheckClientFile()
{
	if (this->m_MainInfo.ClientCRC32 == 0)
	{
		return;
	}

	char name[MAX_PATH] = { 0 };

	if (GetModuleFileName(0, name, sizeof(name)) == 0)
	{
		ExitProcess(0);
	}

	if (_stricmp(ConvertModuleFileName(name), this->m_MainInfo.ClientName) != 0)
	{
		ExitProcess(0);
	}

	CCRC32 CRC32;

	DWORD ClientCRC32;

	if (CRC32.FileCRC(this->m_MainInfo.ClientName, &ClientCRC32, 1024) == 0)
	{
		MessageBoxW(0, RENDER_UNICODE_04, ERROR_04, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}

	if (this->m_MainInfo.ClientCRC32 != ClientCRC32)
	{
		MessageBoxW(0, RENDER_UNICODE_05, ERROR_05, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
}
void CProtect::CheckMaxGameInstances()
{
	if (gProtect->m_MainInfo.MaxGameInstances == 0)
	{
		return;
	}
#if defined(PEGASUS_ISOLATED_TEST)
	const char* processName = "Engine-Pegasus.exe";
#else
	const char* processName = gProtect->m_MainInfo.ClientName;
#endif
	if (IsProcessRunning(processName, gProtect->m_MainInfo.MaxGameInstances))
	{
		wchar_t NameAccess[256];
		wchar_t WinName[256];
		wchar_t ClientNameW[32];

		MultiByteToWideChar(CP_ACP, 0, gProtect->m_MainInfo.ClientName, -1, ClientNameW, 32);

		swprintf(NameAccess, 256, RENDER_UNICODE_0E, gProtect->m_MainInfo.MaxGameInstances);
		swprintf(WinName, 256, ERROR_0E, ClientNameW);

		MessageBoxW(NULL, NameAccess, WinName, MB_OK | MB_ICONSTOP);
		ExitProcess(1);
	}
}

#endif
