
#include <windows.h>
#include <UserEnv.h>
#include <Shlobj.h>
#include <stdio.h>
#include <io.h>

#include <tlhelp32.h>
#include <WtsApi32.h>

#include <Shlwapi.h>
#include <Psapi.h>
#include <DbgHelp.h>
#include "utils.h"
#include "api.h"
#include <dbghelp.h>

#pragma comment(lib,"Dbghelp.lib")

#pragma comment(lib,"userenv.lib")


int __cdecl runLog(const WCHAR* format, ...)
{
	int result = 0;

	WCHAR info[2048];

	va_list   arglist;

	va_start(arglist, format);

	SYSTEMTIME st;
	GetLocalTime(&st);
	int offset = wsprintfW(info, L"[ljg]%2u:%2u:%2u %2u/%2u/%4u ", 
		st.wHour, st.wMinute, st.wSecond, st.wMonth, st.wDay, st.wYear);

	offset += vswprintf_s(info+offset, sizeof(info) / sizeof(WCHAR) - offset, format, arglist);

	va_end(arglist);

	lpOutputDebugStringW(info);

	return offset;
}




int __cdecl runLog(const CHAR* format, ...)
{
	int result = 0;

	CHAR info[2048];

	SYSTEMTIME st;
	GetLocalTime(&st);
	int offset = wsprintfA(info, "[ljg]%2u:%2u:%2u %2u/%2u/%4u ",
		st.wHour, st.wMinute, st.wSecond, st.wMonth, st.wDay, st.wYear);

	va_list   arglist;

	va_start(arglist, format);

	offset += vsprintf_s(info + offset, sizeof(info) - offset, format, arglist);

	va_end(arglist);

	lpOutputDebugStringA(info);

	return offset;
}

int commandline(WCHAR* szparam, int wait, int show, DWORD* ret) {
	int result = 0;

	STARTUPINFOW si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD processcode = 0;
	DWORD threadcode = 0;

	si.cb = sizeof(STARTUPINFOW);
	si.lpDesktop = (WCHAR*)L"WinSta0\\Default";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = show;
	DWORD dwCreationFlags = NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT;

	result = lpCreateProcessW(0, szparam, 0, 0, 0, 0, 0, 0, &si, &pi);
	int errorcode = GetLastError();
	if (result) {
		if (wait)
		{
			lpWaitForSingleObject(pi.hProcess, INFINITE);
			GetExitCodeThread(pi.hProcess, &threadcode);
			GetExitCodeProcess(pi.hProcess, &processcode);
		}

		lpCloseHandle(pi.hProcess);
		lpCloseHandle(pi.hThread);
	}
	runLog(L"command:%ws result:%d, process excode:%d, thread excode:%d ,errorcode:%d\r\n",
		szparam, result, processcode, threadcode, errorcode);
	return result;
}

unsigned short crc16(unsigned char* data, int size) {

	int cnt = size >> 1;
	int mod = size % 2;

	unsigned int v = 0;

	unsigned short* crcdata = (unsigned short*)data;

	for (int i = 0; i < cnt; i++)
	{
		v += crcdata[i];
	}

	if (mod)
	{
		v += data[cnt * 2];
	}

	unsigned int high16 = v >> 16;
	v = v & 0xffff;
	v += high16;
	return v;
}

int binarySearch(const char* data, int size, const char* tag, int tagsize) {
	for (int i = 0; i <= size - tagsize; i++)
	{
		if (memcmp(data + i, tag, tagsize) == 0)
		{
			return i;
		}
	}

	return -1;
}


HANDLE  bRunning(BOOL* exist)
{
	HANDLE h = lpCreateMutexA(NULL, TRUE, MY_MUTEX_NAME);
	DWORD dwRet = GetLastError();
	if (h)
	{
		if (ERROR_ALREADY_EXISTS == dwRet)
		{
			*exist = TRUE;
			return h;
		}
		else if (dwRet == FALSE)
		{
			*exist = FALSE;
			return h;
		}
		else
		{
			*exist = FALSE;
			return h;
		}
	}
	else {
		*exist = FALSE;
		return FALSE;
	}
}


int suicide() {
	ExitProcess(0);
	DWORD hp = GetCurrentProcessId();
	TerminateProcess((HANDLE)hp, 0);
	exit(0);
	abort();
	atexit(0);
	while (1)
	{
		Sleep(-1);
	}
	return 0;
}


int wow64()
{
	BOOL bIsWow64 = FALSE;
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function and GetProcAddress to get a pointer to the function if available.

	char szIsWow64Process[] = { 'I','s','W','o','w','6','4','P','r','o','c','e','s','s',0 };

	char szkernel32[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };
	HMODULE hker = (HMODULE)LoadLibraryA(szkernel32);
	if (hker == 0)
	{
		return FALSE;
	}

	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hker, szIsWow64Process);
	if (NULL != fnIsWow64Process)
	{
		int iRet = fnIsWow64Process(lpGetCurrentProcess(), &bIsWow64);
		if (iRet)
		{
			return bIsWow64;
		}
	}
	return 0;
}


int cpuBits() {
	SYSTEM_INFO si;
	lpGetNativeSystemInfo(&si);
	if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		return 64;
	else
		return 32;
}


int getOsBits() {
	int wow = wow64();

	int cpubits = cpuBits();

	if (cpubits == 64 && wow == FALSE)
	{
		return 64;
	}
	else if (cpubits == 64 && wow)
	{
		return 32;
	}
	else if (cpubits == 32 && wow == FALSE) {
		return 32;
	}
	return 32;
}






int createProcessWithToken(LPSTR lpTokenProcessName, LPSTR szProcessName, LPSTR szparam)
{
	int ret = 0;
	HANDLE hToken = 0;

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	for (lpProcess32FirstW(hProcessSnap, &pe32); lpProcess32NextW(hProcessSnap, &pe32);)
	{
		char szParam[MAX_PATH] = { 0 };
		int iRet = WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, szParam, sizeof(szParam) - 1, NULL, NULL);
		if (lstrcmpiA(_strupr(szParam), _strupr(lpTokenProcessName)) == 0) {
			HANDLE hProcess = lpOpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
			if (hProcess)
			{
				ret = lpOpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				lpCloseHandle(hProcess);
			}
			else {

			}
			break;
		}
	}

	lpCloseHandle(hProcessSnap);

	if (hToken == 0) {
		return 0;
	}

	PROCESS_INFORMATION pi = { 0 };
	STARTUPINFOA si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	//lstrcpyA(si.lpDesktop, "winsta0\\default");
	si.lpDesktop = "winsta0\\default";
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	LPVOID lpEnvBlock = NULL;
	BOOL bEnv = lpCreateEnvironmentBlock(&lpEnvBlock, hToken, FALSE);
	DWORD dwFlags = CREATE_NEW_CONSOLE;
	if (bEnv)
	{
		dwFlags |= CREATE_UNICODE_ENVIRONMENT;
	}

	//si.dwFlags |= dwFlags;
	// 环境变量创建失败仍然可以创建进程，但会影响到后面的进程获取环境变量内容
	ret = lpCreateProcessAsUserA(
		hToken,
		szProcessName,
		szparam,
		NULL,
		NULL,
		FALSE,
		dwFlags,
		bEnv ? lpEnvBlock : NULL,
		NULL,
		&si,
		&pi);

	if (bEnv)
	{
		ret = lpDestroyEnvironmentBlock(lpEnvBlock);
	}

	return ret;
}






DWORD getPidByName(const char* szProcessName)
{
	char szProcName[MAX_PATH] = { 0 };
	lstrcpyA(szProcName, szProcessName);
	_strupr_s(szProcName, MAX_PATH);

	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(pe32);
	HANDLE hProcessSnap = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	int iRet = 0;
	BOOL bNext = lpProcess32FirstW(hProcessSnap, &pe32);
	while (bNext)
	{
		char szexefn[MAX_PATH] = { 0 };
		int ret = wcstombs(szexefn, pe32.szExeFile, lstrlenW(pe32.szExeFile));
		_strupr_s(szexefn, MAX_PATH);

		if (lstrcmpA(szProcName, szexefn) == 0)
		{
			lpCloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}
		bNext = lpProcess32NextW(hProcessSnap, &pe32);
	}
	lpCloseHandle(hProcessSnap);
	return FALSE;
}


DWORD getProcNameByPID(DWORD pid, char* procname, int buflen)
{
	HANDLE h = NULL;
	PROCESSENTRY32 pe = { 0 };
	DWORD ppid = 0;
	pe.dwSize = sizeof(PROCESSENTRY32);
	h = lpCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (lpProcess32FirstW(h, &pe))
	{
		do
		{
			if (pe.th32ProcessID == pid)
			{

				int len = WideCharToMultiByte(CP_ACP, 0, pe.szExeFile, -1, procname, buflen, 0, 0);
				if (len > 0)
				{
					procname[len] = 0;
					return len;
				}
				break;
			}
		} while (lpProcess32NextW(h, &pe));
	}
	lpCloseHandle(h);
	return (ppid);
}


int __stdcall shell(const char* cmd) {
	int ret = 0;

	wchar_t wstrcmd[1024];

	char command[1024];

	char cmdfmt[] = { 'c','m','d',' ','/','c',' ','%','s',' ','>',' ','%','s',0 };
	wsprintfA(command, cmdfmt, cmd, CMD_RESULT_FILENAME);

	ret = mbstowcs(wstrcmd, command, sizeof(wstrcmd) / sizeof(wchar_t));

	DWORD result = 0;

	ret = commandline(wstrcmd, TRUE, FALSE, &result);

	return ret;
}



int runShell(const char* cmd) {
	HANDLE ht = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)shell, (LPVOID)cmd, 0, 0);
	if (ht)
	{
		lpCloseHandle(ht);
	}
	return 0;
}


int GetPathFromFullName(char* strFullName, char* strDst)
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