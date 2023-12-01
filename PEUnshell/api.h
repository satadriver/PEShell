#pragma once

#include <windows.h>
#include <lmcons.h>
#include <TlHelp32.h>



typedef UINT(__stdcall *ptrGetSystemDirectoryA)(LPSTR, DWORD);
typedef HANDLE(__stdcall * ptrGetCurrentProcess)();
typedef FARPROC(__stdcall *ptrGetProcAddress)(HMODULE, LPSTR);
typedef HMODULE(__stdcall *ptrLoadLibraryA)(LPSTR);

typedef DWORD(__stdcall* ptrGetTickCount)(VOID);

typedef ULONGLONG(__stdcall* ptrGetTickCount64)(VOID);

typedef DWORD(__stdcall *ptrFreeLibrary)(HMODULE);

typedef HMODULE(__stdcall *ptrGetModuleHandleA)(LPSTR);
typedef DWORD(__stdcall *ptrWinExec)(LPSTR, DWORD);

typedef DWORD(__stdcall *ptrSetCurrentDirectoryA)(_In_ LPCSTR lpPathName);
typedef DWORD(__stdcall *ptrGetUserNameA)(LPSTR lpBuffer, _Inout_ LPDWORD pcbBuffer);
typedef DWORD(__stdcall *ptrGetComputerNameA)(LPSTR lpBuffer, _Inout_ LPDWORD nSize);

typedef LPSTR(__stdcall *ptrVirtualAlloc)(char*, DWORD, DWORD, DWORD);
typedef UINT(__stdcall *ptrVirtualFree)(LPVOID, DWORD, DWORD);
typedef DWORD(WINAPI * ptrNetWkstaGetInfo)(__in_opt IN LMSTR servername OPTIONAL, IN DWORD level, LPBYTE*);
typedef DWORD(WINAPI * ptrNetApiBufferFree)(IN LPVOID Buffer);

typedef LPTOP_LEVEL_EXCEPTION_FILTER(WINAPI* ptrSetUnhandledExceptionFilter)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

typedef HANDLE(WINAPI* ptrCreateToolhelp32Snapshot)(DWORD dwFlags,DWORD th32ProcessID);

typedef BOOL (WINAPI* ptrProcess32FirstW)(HANDLE hSnapshot,LPPROCESSENTRY32W lppe);

typedef BOOL (WINAPI* ptrProcess32NextW)(HANDLE hSnapshot,LPPROCESSENTRY32W lppe);

typedef HANDLE(WINAPI* ptrOpenProcess)(DWORD dwDesiredAccess,BOOL  bInheritHandle,DWORD dwProcessId);

typedef BOOL(WINAPI* ptrOpenProcessToken)(HANDLE  ProcessHandle, DWORD   DesiredAccess,PHANDLE TokenHandle);

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

typedef int (__fastcall *ptrMakeSureDirectoryPathExists)(LPSTR);

typedef BOOL(__fastcall* ptrContinueDebugEvent)(DWORD dwProcessId,DWORD dwThreadId,DWORD dwContinueStatus);


typedef HANDLE(__stdcall* ptrCreateMutexA)(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCSTR lpName);

typedef BOOL (__stdcall * ptrCloseHandle)(HANDLE hObject);

typedef BOOL(__stdcall* ptrIsDebuggerPresent)();

typedef DWORD(__stdcall* ptrGetCurrentProcessId)();

typedef BOOL(__stdcall* ptrDebugActiveProcess)(DWORD dwProcessId);

typedef DWORD(__stdcall* ptrWaitForSingleObject)(
	_In_ HANDLE hHandle,
	_In_ DWORD dwMilliseconds
);

typedef BOOL(__stdcall* ptrCheckRemoteDebuggerPresent)(
	_In_ HANDLE hProcess,
	_Out_ PBOOL pbDebuggerPresent
);

typedef BOOL(__stdcall* ptrSHGetSpecialFolderPathA)(HWND  hwnd,LPSTR pszPath,int   csidl,BOOL  fCreate);

typedef VOID (__stdcall* ptrGetNativeSystemInfo)(
	_Out_ LPSYSTEM_INFO lpSystemInfo
);

typedef BOOL(__stdcall* ptrCreateEnvironmentBlock)(LPVOID* lpEnvironment,HANDLE  hToken,BOOL    bInherit);

typedef BOOL(__stdcall* ptrDestroyEnvironmentBlock)(LPVOID  lpEnvironment);

typedef HANDLE(__stdcall* ptrCreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes,SIZE_T dwStackSize,
LPTHREAD_START_ROUTINE  lpStartAddress,
LPVOID lpParameter,
DWORD                   dwCreationFlags,
LPDWORD                 lpThreadId
);

typedef HANDLE(__stdcall* ptrCreateFileA)(
 LPCSTR                lpFileName,
DWORD                 dwDesiredAccess,
DWORD                 dwShareMode,
LPSECURITY_ATTRIBUTES lpSecurityAttributes,
 DWORD                 dwCreationDisposition,
DWORD                 dwFlagsAndAttributes,
HANDLE                hTemplateFile
);


typedef BOOL(__stdcall* ptrWaitForDebugEvent)(LPDEBUG_EVENT lpDebugEvent,DWORD         dwMilliseconds);

typedef void(__stdcall* ptrSleep)(DWORD dwMilliseconds);

typedef  BOOL(__stdcall* ptrCreateProcessAsUserA)(
	_In_opt_ HANDLE hToken,
	_In_opt_ LPCSTR lpApplicationName,
	_Inout_opt_ LPSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOA lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
);

typedef BOOL(__stdcall* ptrCreateProcessW)(
	_In_opt_ LPCWSTR lpApplicationName,
	_Inout_opt_ LPWSTR lpCommandLine,
	_In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
	_In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPVOID lpEnvironment,
	_In_opt_ LPCWSTR lpCurrentDirectory,
	_In_ LPSTARTUPINFOW lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
);

typedef  DWORD(__stdcall* ptrGetFileSize)(HANDLE hFile,LPDWORD lpFileSizeHigh);

typedef DWORD(__stdcall* ptrSetFilePointer)(
	_In_ HANDLE hFile,
	_In_ LONG lDistanceToMove,
	_Inout_opt_ PLONG lpDistanceToMoveHigh,
	_In_ DWORD dwMoveMethod
);


typedef HANDLE (__stdcall* ptrFindFirstFileA)(
	_In_ LPCSTR lpFileName,
	_Out_ LPWIN32_FIND_DATAA lpFindFileData
);


typedef BOOL(__stdcall* ptrFindNextFileA)(
	_In_ HANDLE hFindFile,
	_Out_ LPWIN32_FIND_DATAA lpFindFileData
);


typedef BOOL(__stdcall* ptrFindClose)(
	_Inout_ HANDLE hFindFile
);

typedef BOOL (__stdcall* ptrSetFileAttributesA)(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwFileAttributes
);

typedef BOOL(__stdcall* ptrWriteFile)(
	_In_ HANDLE hFile,
	_In_reads_bytes_opt_(nNumberOfBytesToWrite) LPCVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToWrite,
	_Out_opt_ LPDWORD lpNumberOfBytesWritten,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
);

typedef BOOL(__stdcall* ptrFlushFileBuffers)(
	_In_ HANDLE hFile
);


typedef BOOL(__stdcall* ptrReadFile)(
	_In_ HANDLE hFile,
	_Out_writes_bytes_to_opt_(nNumberOfBytesToRead, *lpNumberOfBytesRead) __out_data_source(FILE) LPVOID lpBuffer,
	_In_ DWORD nNumberOfBytesToRead,
	_Out_opt_ LPDWORD lpNumberOfBytesRead,
	_Inout_opt_ LPOVERLAPPED lpOverlapped
);

typedef LSTATUS (__stdcall* ptrRegSetValueExA)(
	_In_ HKEY hKey,
	_In_opt_ LPCSTR lpValueName,
	_Reserved_ DWORD Reserved,
	_In_ DWORD dwType,
	_In_reads_bytes_opt_(cbData) CONST BYTE* lpData,
	_In_ DWORD cbData
);

typedef LSTATUS
(__stdcall* ptrRegCloseKey)(
	_In_ HKEY hKey
);

typedef LSTATUS(__stdcall* ptrRegCreateKeyExA)(
	_In_ HKEY hKey,
	_In_ LPCSTR lpSubKey,
	_Reserved_ DWORD Reserved,
	_In_opt_ LPSTR lpClass,
	_In_ DWORD dwOptions,
	_In_ REGSAM samDesired,
	_In_opt_ CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_Out_ PHKEY phkResult,
	_Out_opt_ LPDWORD lpdwDisposition
);

typedef LSTATUS (__stdcall* ptrRegQueryValueExA)(
	_In_ HKEY hKey,
	_In_opt_ LPCSTR lpValueName,
	_Reserved_ LPDWORD lpReserved,
	_Out_opt_ LPDWORD lpType,
	_Out_writes_bytes_to_opt_(*lpcbData, *lpcbData) __out_data_source(REGISTRY) LPBYTE lpData,
	_When_(lpData == NULL, _Out_opt_) _When_(lpData != NULL, _Inout_opt_) LPDWORD lpcbData
);



typedef VOID(__stdcall* ptrExitProcess)(
	_In_ UINT uExitCode
);

typedef  BOOL(__stdcall* ptrTerminateProcess)(
	_In_ HANDLE hProcess,
	_In_ UINT uExitCode
);



typedef int (__stdcall* ptrWSAStartup)(
	_In_ WORD wVersionRequired,
	_Out_ LPWSADATA lpWSAData);


typedef VOID(__stdcall* ptrOutputDebugStringA)(
	_In_opt_ LPCSTR lpOutputString
);

typedef VOID (__stdcall* ptrOutputDebugStringW)(
	_In_opt_ LPCWSTR lpOutputString
);

extern ptrCheckRemoteDebuggerPresent	lpCheckRemoteDebuggerPresent;

extern ptrWSAStartup				lpWSAStartup;

extern ptrOutputDebugStringW	lpOutputDebugStringW;
extern ptrOutputDebugStringA	lpOutputDebugStringA;

extern ptrTerminateProcess			lpTerminateProcess;
extern ptrExitProcess				lpExitProcess;

extern ptrFindNextFileA		lpFindNextFileA;
extern ptrFindFirstFileA		lpFindFirstFileA;
extern ptrFindClose			lpFindClose;

extern ptrSHGetSpecialFolderPathA			lpSHGetSpecialFolderPathA;

extern ptrCreateEnvironmentBlock		lpCreateEnvironmentBlock;

extern ptrDestroyEnvironmentBlock		lpDestroyEnvironmentBlock;


extern ptrGetNativeSystemInfo			lpGetNativeSystemInfo;
extern ptrWaitForSingleObject			lpWaitForSingleObject;

extern ptrRegSetValueExA				lpRegSetValueExA;
extern ptrRegCreateKeyExA				lpRegCreateKeyExA;
extern ptrRegCloseKey					lpRegCloseKey;
extern ptrRegQueryValueExA				lpRegQueryValueExA;

extern ptrReadFile				lpReadFile;
extern ptrFlushFileBuffers		lpFlushFileBuffers;
extern ptrWriteFile				lpWriteFile;
extern ptrGetFileSize			lpGetFileSize;
extern ptrSetFilePointer		lpSetFilePointer;
extern ptrSetFileAttributesA	lpSetFileAttributesA;

extern ptrCreateProcessW lpCreateProcessW;

extern ptrCreateProcessAsUserA lpCreateProcessAsUserA;

extern ptrOpenProcess lpOpenProcess;

extern ptrOpenProcessToken lpOpenProcessToken;

extern ptrCreateToolhelp32Snapshot  lpCreateToolhelp32Snapshot;

extern ptrProcess32FirstW lpProcess32FirstW;

extern ptrProcess32NextW lpProcess32NextW;

extern ptrSetUnhandledExceptionFilter lpSetUnhandledExceptionFilter;

extern ptrSleep					lpSleep;

extern ptrGetTickCount	lpGetTickCount;

extern ptrGetTickCount64 lpGetTickCount64;

extern ptrCreateFileA			lpCreateFileA;
extern ptrContinueDebugEvent	lpContinueDebugEvent;

extern ptrDebugActiveProcess	lpDebugActiveProcess;
extern ptrWaitForDebugEvent		lpWaitForDebugEvent;
extern ptrCreateThread			lpCreateThread;
extern ptrGetCurrentProcessId	lpGetCurrentProcessId;
extern ptrGetCurrentProcess		lpGetCurrentProcess;
extern ptrCreateMutexA			lpCreateMutexA;

extern ptrIsDebuggerPresent			lpIsDebuggerPresent;

extern ptrCloseHandle			lpCloseHandle;

extern ptrGetProcAddress		lpGetProcAddress;
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

void testapi();