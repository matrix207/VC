#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <stdio.h>
#include "provider.h"

#pragma comment(lib, "advapi32.lib")

#define PROVIDER_NAME L"MyEventProvider"

// Hardcoded insert string for the event messages.
CONST LPWSTR pBadCommand = L"The command that was not valid";
CONST LPWSTR pFilename = L"c:\\folder\\file.ext";
CONST LPWSTR pNumberOfRetries = L"3";
CONST LPWSTR pSuccessfulRetries = L"0";
CONST LPWSTR pQuarts = L"8";
CONST LPWSTR pGallons = L"2";

void wmain(void)
{
	HANDLE hEventLog = NULL;
	LPWSTR pInsertStrings[2] = {NULL, NULL};
	DWORD dwEventDataSize = 0;

	// The source name (provider) must exist as a subkey of Application.
	hEventLog = RegisterEventSource(NULL, PROVIDER_NAME);
	if (NULL == hEventLog)
	{
		wprintf(L"RegisterEventSource failed with 0x%x.\n", GetLastError());
		goto cleanup;
	}

	// This event includes user-defined data as part of the event. The event message
	// does not use insert strings.
	dwEventDataSize = ((DWORD)wcslen(pBadCommand) + 1) * sizeof(WCHAR);
	if (!ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, UI_CATEGORY, MSG_INVALID_COMMAND, NULL, 0, dwEventDataSize, NULL, pBadCommand))
	{
		wprintf(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_INVALID_COMMAND);
		goto cleanup;
	}

	// This event uses insert strings.
	pInsertStrings[0] = pFilename;
	if (!ReportEvent(hEventLog, EVENTLOG_ERROR_TYPE, DATABASE_CATEGORY, MSG_BAD_FILE_CONTENTS, NULL, 1, 0, (LPCWSTR*)pInsertStrings, NULL))
	{
		wprintf(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_BAD_FILE_CONTENTS);
		goto cleanup;
	}

	// This event uses insert strings.
	pInsertStrings[0] = pNumberOfRetries;
	pInsertStrings[1] = pSuccessfulRetries;
	if (!ReportEvent(hEventLog, EVENTLOG_WARNING_TYPE, NETWORK_CATEGORY, MSG_RETRIES, NULL, 2, 0, (LPCWSTR*)pInsertStrings, NULL))
	{
		wprintf(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_RETRIES);
		goto cleanup;
	}

	// This event uses insert strings.
	pInsertStrings[0] = pQuarts;
	pInsertStrings[1] = pGallons;
	if (!ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, UI_CATEGORY, MSG_COMPUTE_CONVERSION, NULL, 2, 0, (LPCWSTR*)pInsertStrings, NULL))
	{
		wprintf(L"ReportEvent failed with 0x%x for event 0x%x.\n", GetLastError(), MSG_COMPUTE_CONVERSION);
		goto cleanup;
	}

	wprintf(L"All events successfully reported.\n");

cleanup:

	if (hEventLog)
		DeregisterEventSource(hEventLog);
}