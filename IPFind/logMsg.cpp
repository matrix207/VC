// logMsg.cpp
// Create by Dennis 2011-04-19
#include "stdafx.h"
#include "logMsg.h"

// for FILE
#include <stdio.h>

// for vsprintf
#include <stdarg.h>

// for wvsprintf
//#include <Windows.h>

#include <afxmt.h>
CCriticalSection g_criticalSection;

#define MAX_BUF_SIZE 1024
//#define ADD_TIME

#ifdef NDEBUG
//#if 0
void logMsg(const char *format, ...)  {((void)0);}
#else
void logMsg(const char *format, ...) 
{
	FILE *pOutput = NULL;
	char buffer[MAX_BUF_SIZE] = {0};
	
	va_list args;

	g_criticalSection.Lock();
	if ((pOutput = fopen(_LOG_FILE, "a")) != NULL)
	{
#if defined(ADD_TIME)
		{
			CTime   time;
			CString strMsg;
			time = CTime::GetCurrentTime();
			char timeBuf[100] = {0};
			sprintf(timeBuf, "%04d-%02d-%02d %02d:%02d:%02d\n"
				, time.GetYear(), time.GetMonth(), time.GetDay()
				, time.GetHour(), time.GetMinute(), time.GetSecond());
			fprintf(pOutput, timeBuf);
		}
#endif
		va_start(args, format);
		vsprintf(buffer, format, args);
		va_end(args);

		fprintf(pOutput, buffer);

		fclose(pOutput);
	}
	g_criticalSection.Unlock();
}

void logWMsg(const TCHAR *format, ...) 
{
	FILE *pOutput = NULL;
	TCHAR buffer[MAX_BUF_SIZE] = {0};

	va_list args;

	g_criticalSection.Lock();
	if ((pOutput = fopen(_LOG_FILE, "a")) != NULL)
	{
		va_start(args, format);
		wvsprintf(buffer, format, args);
		va_end(args);

		fwprintf(pOutput, buffer);

		fclose(pOutput);
	}
	g_criticalSection.Unlock();
}
#endif
