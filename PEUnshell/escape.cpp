
#include "escape.h"
#include <windows.h>
#include <time.h>


#define ESCAPE_DELAY_SECONDS 3

int Escape::escape() {
	int ret = 0;

	SYSTEMTIME olddt = { 0 };
	GetLocalTime(&olddt);
	unsigned __int64 oldlow = olddt.wMinute * 60 + olddt.wSecond + olddt.wHour * 3600 + olddt.wDay * 86400;
	DWORD oldhigh = (olddt.wMonth) + (olddt.wYear << 16);

	while (1)
	{
		SYSTEMTIME ndt = { 0 };
		GetLocalTime(&ndt);

		unsigned __int64 low = ndt.wMinute *60 + ndt.wSecond + ndt.wHour*3600 + ndt.wDay*86400;
		DWORD high = (ndt.wMonth) + (ndt.wYear << 16);

		DWORD lowvalue = low - oldlow;
		if ((lowvalue > ESCAPE_DELAY_SECONDS) || (high - oldhigh > 0))
		{
			break;
		}
		Sleep(1000);
	}

	return TRUE;
}