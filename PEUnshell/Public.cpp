
#include "Public.h"
#include <windows.h>
#include <lm.h>
#include <Winternl.h>
#include "api.h"


#pragma comment( lib, "netapi32.lib" )

#define LOG_FILE_NAME "log.log"

string Public::getusername() {

	char username[MAX_PATH] = { 0 };
	int ret = 0;
	DWORD len = MAX_PATH;
	ret = lpGetUserNameA(username, &len);

	return string(username);
}



int Public::getSysVer() {

	WKSTA_INFO_100 *wkstaInfo = NULL;
	NET_API_STATUS netStatus = lpNetWkstaGetInfo(NULL, 100, (LPBYTE *)&wkstaInfo);
	if (netStatus == NERR_Success)
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = lpNetApiBufferFree(wkstaInfo);

// 		char strWin9x[] = { 'W','i','n','d','o','w','s',' ','9','.','x',0 };
// 		char strWin2000[] = { 'W','i','n','d','o','w','s',' ','2','0','0','0',0 };
// 		char strWinXP[] = { 'W','i','n','d','o','w','s',' ','X','P',0 };
// 		char strWinVista[] = { 'W','i','n','d','o','w','s',' ','V','i','s','t','a',0 };
// 		char strWin7[] = { 'W','i','n','d','o','w','s',' ','7',0 };
// 		char strWin8[] = { 'W','i','n','d','o','w','s',' ','8',0 };
// 		char strWin10[] = { 'W','i','n','d','o','w','s',' ','1','0',0 };
// 		char strWinUnknow[] = { 'U','n','k','n','o','w',0 };

		int iSystemVersion = 0;
		if (dwVersion < 0x50000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN9X;
		}
		else if (dwVersion == 0x50000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN2000;
		}
		else if (dwVersion > 0x50000 && dwVersion < 0x60000)
		{
			iSystemVersion = SYSTEM_VERSION_XP;
		}
		else if (dwVersion == 0x60000)
		{
			iSystemVersion = SYSTEM_VERSION_VISTA;
		}
		else if (dwVersion == 0x60001)
		{
			iSystemVersion = SYSTEM_VERSION_WIN7;
		}
		else if (dwVersion >= 0x60002 && dwVersion <= 0x60003)
		{
			iSystemVersion = SYSTEM_VERSION_WIN8;
		}
		else if (dwVersion >= 0x60003 || dwVersion == 0x100000)
		{
			iSystemVersion = SYSTEM_VERSION_WIN10;
		}
		else
		{
			iSystemVersion = SYSTEM_VERSION_UNKNOW;
		}
		return iSystemVersion;
	}

	return FALSE;
}


int Public::writelog(string log) {
	FILE * fp = fopen(LOG_FILE_NAME, "ab+");
	if (fp == 0)
	{
		return FALSE;
	}

	log = log + "\r\n";
	int loglen = log.length();
	int ret = fwrite(log.c_str(), 1, loglen, fp);
	fclose(fp);
	return ret;
}


int Public::writelog(char * fmt, ...) {
#ifdef _DEBUG
	FILE * fp = fopen(LOG_FILE_NAME, "ab+");
	if (fp == 0)
	{
		return FALSE;
	}

	va_list val;

	char log[1024];

	va_start(val, fmt);
	int loglen = vsnprintf(log, 1024, fmt, val);
	va_end(val);

	int ret = fwrite(log, 1, loglen, fp);
	fclose(fp);
	return ret;
#else
	return 0;
#endif
}