#include <windows.h>
#include <stdio.h>

extern void StartEventLogMonitor(void);
extern void StopEventLogMonitor(void);

DWORD WINAPI StartThread(LPVOID lpParameter)
{
	StartEventLogMonitor();
	printf("exit thread\n");
	return 0;
}

int main(int argc, char* argv[])
{
	printf("Start event log monitor.\n");

	HANDLE hThr = CreateThread(NULL, 0, StartThread, 0, 0, 0);

	printf("Press any key to stop event log monitor.\n");
	getchar();
	StopEventLogMonitor();

	printf("Stop event log monitor.\n");

	WaitForSingleObject(hThr, INFINITE);

	system("pause");

	return 0;

}