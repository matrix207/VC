#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include "watchdog.h"

#define USE_TIMER

#define BUILD_MAJOR 1
#define BUILD_MINOR 4


#define EnterFunc() do {printf("+ %s \n", __FUNCTION__);} while (0);
#define ExitFunc() do {printf("- %s \n", __FUNCTION__);} while (0);

typedef bool (WINAPI *InitializeWinIo)();
typedef void (WINAPI *ShutdownWinIo)();
typedef bool (WINAPI *GetPortVal)(WORD wPortAddr, PDWORD pdwPortVal, BYTE bSize);
typedef bool (WINAPI *SetPortVal)(WORD wPortAddr, DWORD dwPortVal, BYTE bSize);
typedef bool (WINAPI *InstallWinIoDriver)(PWSTR pszWinIoDriverPath, bool IsDemandLoaded);

InitializeWinIo		pInitializeWinIo	= NULL;
ShutdownWinIo		pShutdownWinIo		= NULL;
GetPortVal			pGetPortVal			= NULL;
SetPortVal			pSetPortVal			= NULL;
InstallWinIoDriver	pInstallWinIoDriver = NULL;

HINSTANCE hinstLib = NULL;

VOID SafeGetNativeSystemInfo(__out LPSYSTEM_INFO lpSystemInfo)
{
	if (NULL==lpSystemInfo)	return;
	typedef VOID (WINAPI *LPFN_GetNativeSystemInfo)(LPSYSTEM_INFO lpSystemInfo);
	LPFN_GetNativeSystemInfo fnGetNativeSystemInfo = 
		(LPFN_GetNativeSystemInfo)GetProcAddress( GetModuleHandle(_T("kernel32")), "GetNativeSystemInfo");
	if (NULL != fnGetNativeSystemInfo)
	{
		fnGetNativeSystemInfo(lpSystemInfo);
	}
	else
	{
		GetSystemInfo(lpSystemInfo);
	}
}

int GetSystemBits()
{
	SYSTEM_INFO si;
	SafeGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
		si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
	{
		return 64;
	}
	return 32;
}

int LoadLib()
{
	TCHAR *tcLib = NULL;
	tcLib = (32 == GetSystemBits()) ? TEXT("WinIo32.dll") : TEXT("WinIo64.dll");
	hinstLib = LoadLibrary(tcLib); 
	wprintf(TEXT("Load %s %s\n"), tcLib, hinstLib ? TEXT("OK") : TEXT("Fail"));
	return (hinstLib ? 0 : 1);
}

void FreeLib()
{
	if (hinstLib)
	{
		FreeLibrary(hinstLib);
		hinstLib = NULL;
	}
}

int GetFuncAddr()
{
	if (hinstLib != NULL) 
	{ 
		pInitializeWinIo = (InitializeWinIo) GetProcAddress(hinstLib, "InitializeWinIo");
		if (!pInitializeWinIo) return 1;

		pShutdownWinIo = (ShutdownWinIo) GetProcAddress(hinstLib, "ShutdownWinIo");
		if (!pShutdownWinIo) return 1;

		pGetPortVal = (GetPortVal) GetProcAddress(hinstLib, "GetPortVal");
		if (!pGetPortVal) return 1;

		pSetPortVal = (SetPortVal) GetProcAddress(hinstLib, "SetPortVal");
		if (!pSetPortVal) return 1;

		pInstallWinIoDriver = (InstallWinIoDriver) GetProcAddress(hinstLib, "InstallWinIoDriver");
		if (!pInstallWinIoDriver) return 1;
	}

	return 0;
}

int winio_init()
{
	if (0 != LoadLib())
	{
		printf("Failed to load library.\n");
		return 1;
	}

	if (0 != GetFuncAddr())
	{
		printf("Failed to get function address.\n");
		return 2;
	}

	if (!pInitializeWinIo())
	{
		printf("Failed to initialize WinIo.\n");
		return 3;
	}

	return 0;
}

void winio_clean()
{
	if (pShutdownWinIo)	pShutdownWinIo();
	FreeLib();
}

void outb(const unsigned char value, const unsigned char index)
{
	if (pSetPortVal(index, value, 1))
	{
		//printf("set port:0x%x, value:0x%02x \n", index, value);
	}
	else
	{
		printf("Failed to call GetPortVal.\n");
	}
}

void inb(unsigned char *pvalue, const unsigned char index)
{
	DWORD dwVal = 0;
	if (pGetPortVal(index, &dwVal, 1))
	{
		*pvalue = (unsigned char) dwVal;
		//printf("get port:0x%x, value:0x%02x \n", index, *pvalue);
	}
	else
	{
		printf("Failed to call SetPortVal.\n");
	}
}
