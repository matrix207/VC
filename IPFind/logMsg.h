// logMsg.h
// Create by Dennis 2011-04-19

#ifndef _LOGMSG_H_
#define _LOGMSG_H_

// for TCHAR
#include <tchar.h>

#define _LOG_FILE "log.txt"

void logMsg(const char *format, ...);
void logWMsg(const TCHAR *format, ...);

// sample code
/*
int nCount = 1;
logMsg("scan count:%d\n",nCount);
logWMsg(_T("scan count:%d\n"),nCount);
*/

#endif // _LOGMSG_H_
