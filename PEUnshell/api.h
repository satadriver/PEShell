#pragma once

#include <windows.h>
#include <lmcons.h>

typedef UINT(__stdcall *ptrGetSystemDirectoryA)(LPSTR, DWORD);
typedef HANDLE(__stdcall * ptrGetCurrentProcess)();
typedef DWORD(__stdcall *ptrGetProcAddress)(HMODULE, LPSTR);
typedef HMODULE(__stdcall *ptrLoadLibraryA)(LPSTR);

typedef DWORD(__stdcall *ptrFreeLibrary)(HMODULE);

typedef HANDLE(__stdcall *ptrGetModuleHandleA)(LPSTR);
typedef DWORD(__stdcall *ptrWinExec)(LPSTR, DWORD);

typedef DWORD(__stdcall *ptrSetCurrentDirectoryA)(_In_ LPCSTR lpPathName);
typedef DWORD(__stdcall *ptrGetUserNameA)(LPSTR lpBuffer, _Inout_ LPDWORD pcbBuffer);
typedef DWORD(__stdcall *ptrGetComputerNameA)(LPSTR lpBuffer, _Inout_ LPDWORD nSize);

typedef LPSTR(__stdcall *ptrVirtualAlloc)(DWORD, DWORD, DWORD, DWORD);
typedef UINT(__stdcall *ptrVirtualFree)(LPVOID, DWORD, DWORD);
typedef DWORD(WINAPI * ptrNetWkstaGetInfo)(__in_opt IN LMSTR servername OPTIONAL, IN DWORD level, LPBYTE*);
typedef DWORD(WINAPI * ptrNetApiBufferFree)(IN LPVOID Buffer);

typedef BOOL(__stdcall *ptrVirtualProtect)(
	__in  LPVOID lpAddress,
	__in  SIZE_T dwSize,
	__in  DWORD flNewProtect,
	__out PDWORD lpflOldProtect
);

typedef HINSTANCE(__stdcall *ptrShellExecuteA)(
	HWND hwnd,
	LPCSTR lpOperation,
	LPCSTR lpFile,
	LPCSTR lpParameters,
	LPCSTR lpDirectory,
	INT nShowCmd);

typedef wchar_t ** (__stdcall *ptrCommandLineToArgvW)(__in LPCWSTR lpCmdLine, __out int* pNumArgs);
typedef LPCWSTR(__stdcall *ptrGetCommandLineW)();

typedef int (__stdcall *ptrMakeSureDirectoryPathExists)(LPSTR);

extern ptrGetProcAddress		lpGetProcAddress ;
extern ptrLoadLibraryA			lpLoadLibraryA;
extern ptrFreeLibrary			lpFreeLibrary;

extern ptrShellExecuteA			lpShellExecuteA;
extern ptrGetCommandLineW						lpGetCommandLineW;
extern ptrCommandLineToArgvW					lpCommandLineToArgvW;

extern ptrMakeSureDirectoryPathExists			lpMakeSureDirectoryPathExists;

extern ptrGetUserNameA							lpGetUserNameA;
extern ptrGetComputerNameA						lpGetComputerNameA;
extern ptrNetWkstaGetInfo						lpNetWkstaGetInfo;
extern ptrNetApiBufferFree						lpNetApiBufferFree;
extern ptrGetSystemDirectoryA					lpGetSystemDirectoryA;
extern ptrWinExec								lpWinExec;
extern ptrGetModuleHandleA						lpGetModuleHandleA;
extern ptrVirtualAlloc							lpVirtualAlloc;
extern ptrVirtualFree							lpVirtualFree;
extern ptrVirtualProtect						lpVirtualProtect;

int getapi();