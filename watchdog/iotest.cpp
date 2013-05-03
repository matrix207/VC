
#include <stdio.h>
#include <tchar.h>

#include <windows.h>
#include "winio.h"
#include "watchdog.h"

#define BUILD_MAJOR 1
#define BUILD_MINOR 5


LRESULT CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTimer)   
{   
	printf("reset timeout to be 10 \n");
	/* reset watchdog timeout value to be 10 minutes */
	supermicro_keepalive(10);

	Sleep(1000);

	printf("Show current status:\n");
	supermicro_watchdog_status();

	return   0;   
}

void print_version(void)
{
	printf("Version: %d.%d build %s %s\n\n", BUILD_MAJOR, BUILD_MINOR, __DATE__, __TIME__);
}

int main(int argc, char* argv[])
{
	print_version();

	if (0 != winio_init())
	{
		goto ExitApp;
	}

	supermicro_watchdog_status();

	MSG   msg;   
	UINT nTimerID = SetTimer(NULL, 0, 60*1000, (TIMERPROC)TimerProc);   
	while (GetMessage(&msg, NULL, 0, 0))   
	{   
		TranslateMessage(&msg);   
		DispatchMessage(&msg);   
	}

ExitApp:
	winio_clean();
	printf("Press any key to continue.\n");
	getchar();
	return 0;
}
