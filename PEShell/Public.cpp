
#include "Public.h"
#include <windows.h>
#include <lm.h>
#include <Winternl.h>
#include "FileHelper.h"


#pragma comment( lib, "netapi32.lib" )



string Public::getusername() {
	char usernmae[MAX_PATH] = { 0 };
	int ret = 0;
	DWORD len = MAX_PATH;
	ret = GetUserNameA(usernmae, &len);
	return string(usernmae);
}



int Public::getSysVer() {

	WKSTA_INFO_100 *wkstaInfo = NULL;
	NET_API_STATUS netStatus = NetWkstaGetInfo(NULL, 100, (LPBYTE *)&wkstaInfo);
	if (netStatus == NERR_Success)
	{
		DWORD dwMajVer = wkstaInfo->wki100_ver_major;
		DWORD dwMinVer = wkstaInfo->wki100_ver_minor;
		DWORD dwVersion = (DWORD)MAKELONG(dwMinVer, dwMajVer);
		netStatus = NetApiBufferFree(wkstaInfo);

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

int Public::prepareCfg(char* cfgfn,string dstfn) {

	char * lpdata = 0;
	int filesize = 0;
	int ret = FileHelper::fileReader(cfgfn, &lpdata, &filesize);
	if (ret <= 0)
	{
		return FALSE;
	}

	string username = "";
	string ip = "";
	string mode = "";

	char * hdr = strstr(lpdata, "username=");
	char * end = hdr;
	if (hdr)
	{
		hdr += strlen("username=");
		end = strstr(hdr, "\r\n");
		if (end )
		{
			username = string(hdr, end - hdr);
		}
		else {
			username = string(hdr);
		}
	}

	hdr = strstr(lpdata, "ip=");
	end = hdr;
	if (hdr)
	{
		hdr += strlen("ip=");
		end = strstr(hdr, "\r\n");
		if (end)
		{
			ip = string(hdr, end - hdr);
		}
		else {
			ip = string(hdr);
		}
	}

	hdr = strstr(lpdata, "mode=");
	end = hdr;
	if (hdr)
	{
		hdr += strlen("mode=");
		end = strstr(hdr, "\r\n");
		if (end)
		{
			mode = string(hdr, end - hdr);
		}
		else {
			mode = string(hdr);
		}
	}

	delete lpdata;

	if (username != "" || ip != "")
	{
		ATTACK_RUN_PARAM params = { 0 };
		lstrcpyA(params.username, username.c_str());
		lstrcpyA(params.ip, ip.c_str());
		params.mode = atoi(mode.c_str());
		ret = FileHelper::fileWriter(dstfn, (char*)&params, sizeof(ATTACK_RUN_PARAM),TRUE);
		
	}

	return TRUE;
}






int Public::prepareParams(string ip,string username,int mode,string dstfn) {
	int ret = 0;
// 	string username = "";
// 	string ip = "";
// 
// 	char * hdr = strstr(lpdata, "[username:");
// 	char * end = hdr;
// 	if (hdr)
// 	{
// 		hdr += strlen("[username:");
// 		end = strstr(hdr, "]");
// 		if (end)
// 		{
// 			username = string(hdr, end - hdr);
// 		}
// 		else {
// 			username = string(hdr);
// 		}
// 	}
// 
// 	hdr = strstr(lpdata, "[ip:");
// 	end = hdr;
// 	if (hdr)
// 	{
// 		hdr += strlen("[ip:");
// 		end = strstr(hdr, "]");
// 		if (end)
// 		{
// 			ip = string(hdr, end - hdr);
// 		}
// 		else {
// 			ip = string(hdr);
// 		}
// 	}

	if (username != "" || ip != "")
	{
		ATTACK_RUN_PARAM params = { 0 };
		lstrcpyA(params.username, username.c_str());
		lstrcpyA(params.ip, ip.c_str());
		params.mode = mode;
		ret = FileHelper::fileWriter(dstfn, (char*)&params, sizeof(ATTACK_RUN_PARAM), TRUE);
	}

	return FALSE;
}





string Public:: getCurPath() {
	string str = "";
	char szcurdir[MAX_PATH] = { 0 };
	int ret = GetModuleFileNameA(0, szcurdir, MAX_PATH);
	char * pos = strrchr((char*)szcurdir, '\\');
	if (pos > 0)
	{
		*(pos + 1) = 0;
		str = szcurdir;
	}

	return str;
}


int GetPathFromFullName(const char* strFullName, char* strDst)
{
	int srclen = lstrlenA(strFullName);
	lstrcpyA(strDst, strFullName);
	for ( int i = srclen - 1; i >= 0; i--)
	{
		if (strDst[i] == '\\')
		{
			*(strDst + i + 1) = 0;
			return i + 1;
		}
	}

	strDst[0] = 0;
	return FALSE;
}


