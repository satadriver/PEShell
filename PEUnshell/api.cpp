#include "api.h"
#include "PEParser.h"
#include "Public.h"

ptrShellExecuteA		lpShellExecuteA;
ptrGetCommandLineW		lpGetCommandLineW = 0;
ptrCommandLineToArgvW	lpCommandLineToArgvW = 0;

ptrGetUserNameA			lpGetUserNameA = 0;
ptrGetComputerNameA		lpGetComputerNameA = 0;

ptrGetProcAddress		lpGetProcAddress = 0;
ptrLoadLibraryA			lpLoadLibraryA = 0;
ptrFreeLibrary			lpFreeLibrary = 0;

ptrTerminateProcess			lpTerminateProcess;
ptrExitProcess				lpExitProcess;

ptrCreateToolhelp32Snapshot  lpCreateToolhelp32Snapshot;

ptrProcess32FirstW lpProcess32FirstW;

ptrProcess32NextW lpProcess32NextW;

ptrVirtualAlloc			lpVirtualAlloc;
ptrVirtualFree			lpVirtualFree;
ptrVirtualProtect		lpVirtualProtect;
ptrWinExec				lpWinExec = 0;
ptrGetSystemDirectoryA	lpGetSystemDirectoryA = 0;
ptrGetModuleHandleA		lpGetModuleHandleA;
ptrNetWkstaGetInfo		lpNetWkstaGetInfo;
ptrNetApiBufferFree		lpNetApiBufferFree;

ptrContinueDebugEvent	lpContinueDebugEvent;

ptrDebugActiveProcess	lpDebugActiveProcess = 0;
ptrWaitForDebugEvent	lpWaitForDebugEvent = 0;
ptrCloseHandle			lpCloseHandle = 0;

ptrGetCurrentProcess	lpGetCurrentProcess = 0;
ptrGetCurrentProcessId	lpGetCurrentProcessId = 0;
ptrCreateMutexA			lpCreateMutexA = 0;

ptrCreateFileA			lpCreateFileA = 0;
ptrCreateThread			lpCreateThread = 0;

ptrIsDebuggerPresent	lpIsDebuggerPresent;

ptrSleep						lpSleep;

ptrGetTickCount					lpGetTickCount;

ptrGetTickCount64				lpGetTickCount64;

ptrSetUnhandledExceptionFilter	lpSetUnhandledExceptionFilter;

ptrMakeSureDirectoryPathExists	lpMakeSureDirectoryPathExists;

ptrOpenProcess				lpOpenProcess;

ptrOpenProcessToken			lpOpenProcessToken;

ptrCreateProcessAsUserA		lpCreateProcessAsUserA;

ptrCheckRemoteDebuggerPresent	lpCheckRemoteDebuggerPresent;

ptrCreateProcessW		lpCreateProcessW;
ptrReadFile				lpReadFile;
ptrFlushFileBuffers		lpFlushFileBuffers;
ptrWriteFile			lpWriteFile;
ptrGetFileSize			lpGetFileSize;
ptrSetFilePointer		lpSetFilePointer;
ptrSetFileAttributesA	lpSetFileAttributesA;

ptrFindNextFileA		lpFindNextFileA;
ptrFindFirstFileA		lpFindFirstFileA;
ptrFindClose			lpFindClose;

ptrWaitForSingleObject			lpWaitForSingleObject;
ptrGetNativeSystemInfo			lpGetNativeSystemInfo;

ptrOutputDebugStringW	lpOutputDebugStringW;
ptrOutputDebugStringA	lpOutputDebugStringA;

ptrCreateEnvironmentBlock		lpCreateEnvironmentBlock;
ptrDestroyEnvironmentBlock		lpDestroyEnvironmentBlock;

ptrRegSetValueExA				lpRegSetValueExA;
ptrRegCreateKeyExA				lpRegCreateKeyExA;
ptrRegCloseKey					lpRegCloseKey;
ptrRegQueryValueExA				lpRegQueryValueExA;

ptrSHGetSpecialFolderPathA			lpSHGetSpecialFolderPathA;


ptrWSAStartup				lpWSAStartup;

HMODULE					lpDllShell32;
HMODULE					lpDllNetApi32 = 0;
HMODULE					lpDlladvapi32;
HMODULE					lpDllKernel32;
HMODULE					lpDllDbghelp;


int getapi() {
	char info[1024];

	int ret = 0;

	char szwsprintfA[] = { 'w','s','p','r','i','n','t','f','A',0 };
	char szwsprintfW[] = { 'w','s','p','r','i','n','t','f','W',0 };
	char szGetCurrentProcess[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s',0 };

	char szDllUser32[] = { 'u','s','e','r','3','2','.','d','l','l',0 };
	char szDllWs2_32[] = { 'w','s','2','_','3','2','.','d','l','l',0 };
	char szDlladvapi32[] = { 'a','d','v','a','p','i','3','2','.','d','l','l',0 };
	char szDlliphlpapi[] = { 'i','p','h','l','p','a','p','i','.','d','l','l',0 };
	char szDllntdll[] = { 'n','t','d','l','l','.','d','l','l',0 };



	char szRegOpenKeyExA[] = { 'R','e','g','O','p','e','n','K','e','y','E','x','A',0 };
	char szRegEnumKeyExA[] = { 'R','e','g','E','n','u','m','K','e','y','E','x','A',0 };
	char szRegDeleteKeyA[] = { 'R','e','g','D','e','l','e','t','e','K','e','y','A',0 };

	char szconnect[] = { 'c','o','n','n','e','c','t',0 };
	char szWSAStartup[] = { 'W','S','A','S','t','a','r','t','u','p',0 };
	char szrecv[] = { 'r','e','c','v',0 };
	char szsend[] = { 's','e','n','d',0 };
	char szsocket[] = { 's','o','c','k','e','t',0 };
	char szclosesocket[] = { 'c','l','o','s','e','s','o','c','k','e','t',0 };
	char szinet_addr[] = { 'i','n','e','t','_','a','d','d','r',0 };
	char szntohs[] = { 'n','t','o','h','s',0 };
	char szsetsockopt[] = { 's','e','t','s','o','c','k','o','p','t',0 };
	char szgethostname[] = { 'g','e','t','h','o','s','t','n','a','m','e',0 };
	char szgethostbyname[] = { 'g','e','t','h','o','s','t','b','y','n','a','m','e',0 };

	char szLoadLibraryA[] = { 'L','o','a','d','L','i','b','r','a','r','y','A',0 };
	char szFreeLibrary[] = { 'F','r','e','e','L','i','b','r','a','r','y',0 };
	char szGetProcAddress[] = { 'G','e','t','P','r','o','c','A','d','d','r','e','s','s',0 };

	char szVirtualProtect[] = { 'V','i','r','t','u','a','l','P','r','o','t','e','c','t',0 };
	char szVirtualAlloc[] = { 'V','i','r','t','u','a','l','A','l','l','o','c',0 };
	char szVirtualFree[] = { 'V','i','r','t','u','a','l','F','r','e','e',0 };
	char szVirtualAllocEx[] = { 'V','i','r','t','u','a','l','A','l','l','o','c','E','x',0 };
	char szVirtualFreeEx[] = { 'V','i','r','t','u','a','l','F','r','e','e','E','x',0 };

	char szGetStartupInfoA[] = { 'G','e','t','S','t','a','r','t','u','p','I','n','f','o','A',0 };
	char szCreateProcessA[] = { 'C','r','e','a','t','e','P','r','o','c','e','s','s','A',0 };
	char szWinExec[] = { 'W','i','n','E','x','e','c',0 };
	char szGetSystemDirectoryA[] = { 'G','e','t','S','y','s','t','e','m','D','i','r','e','c','t','o','r','y','A',0 };
	char szGetModuleFileNameA[] = { 'G','e','t','M','o','d','u','l','e','F','i','l','e','N','a','m','e','A',0 };
	char szExitProcess[] = { 'E','x','i','t','P','r','o','c','e','s','s',0 };
	char szTerminateProcess[]={ 'T','e','r','m','i','n','a','t','e','P','r','o','c','e','s','s',0 }; 
	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };

	char szCreateDirectoryA[] = { 'C','r','e','a','t','e','D','i','r','e','c','t','o','r','y','A',0 };

	char szIsDebuggerPresent[] = { 'I','s','D','e','b','u','g','g','e','r','P','r','e','s','e','n','t',0 };

	char szGetCommandLineA[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','A',0 };
	char szGetCommandLineW[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','W',0 };

	char szCloseHandle[] = { 'C','l','o','s','e','H','a','n','d','l','e',0 };

	char szCreateFileA[] = { 'C','r','e','a','t','e','F','i','l','e','A',0 };
	char szGetNativeSystemInfo[] = { 'G','e','t','N','a','t','i','v','e','S','y','s','t','e','m','I','n','f','o',0 };

	char szSleep[] = { 'S','l','e','e','p',0 };

	char szGetTickCount[] = { 'G','e','t','T','i','c','k','C','o','u','n','t',0 };
	char szGetTickCount64[] = { 'G','e','t','T','i','c','k','C','o','u','n','t','6','4',0 };

	char szGetSystemInfo[] = { 'G','e','t','S','y','s','t','e','m','I','n','f','o',0 };

	char szGetUserNameA[] = { 'G','e','t','U','s','e','r','N','a','m','e','A',0 };
	char szGetComputerNameA[] = { 'G','e','t','C','o','m','p','u','t','e','r','N','a','m','e','A',0 };
	char szCreateToolhelp32Snapshot[] = 
	{ 'C','r','e','a','t','e','T','o','o','l','h','e','l','p','3','2','S','n','a','p','s','h','o','t',0 };
	char szProcess32FirstW[] = { 'P','r','o','c','e','s','s','3','2','F','i','r','s','t','W',0 };
	char szProcess32NextW[] = { 'P','r','o','c','e','s','s','3','2','N','e','x','t','W',0 };

	char szProcess32First[] = { 'P','r','o','c','e','s','s','3','2','F','i','r','s','t','W',0 };
	char szProcess32Next[] = { 'P','r','o','c','e','s','s','3','2','N','e','x','t','W',0 };

	char szDeviceIoControl[] = { 'D','e','v','i','c','e','I','o','C','o','n','t','r','o','l',0 };

	char szCreateThread[] = { 'C','r','e','a','t','e','T','h','r','e','a','d',0 };
	char szCreateMutexA[] = { 'C','r','e','a','t','e','M','u','t','e','x','A',0 };

	char szReadFile[] = { 'R','e','a','d','F','i','l','e',0 };
	char szFlushFileBuffers[] = { 'F','l','u','s','h','F','i','l','e','B','u','f','f','e','r','s',0 };
	char szWriteFile[] = { 'W','r','i','t','e','F','i','l','e',0 };
	char szGetFileSize[] = { 'G','e','t','F','i','l','e','S','i','z','e',0 };
	char szSetFilePointer[] = { 'S','e','t','F','i','l','e','P','o','i','n','t','e','r',0 };
	char szRegSetValueExA[] = { 'R','e','g','S','e','t','V','a','l','u','e','E','x','A',0 };
	char szRegCreateKeyExA[] = { 'R','e','g','C','r','e','a','t','e','K','e','y','E','x','A',0 };
	char szRegCloseKey[] = { 'R','e','g','C','l','o','s','e','K','e','y',0 };
	char szRegQueryValueExA[] = { 'R','e','g','Q','u','e','r','y','V','a','l','u','e','E','x','A',0 };

	char szKernel32Dll[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };

	char szDllPsapi[] = { 'P','s','a','p','i','.','d','l','l',0 };
	char szEnumProcessModules[] = { 'E','n','u','m','P','r','o','c','e','s','s','M','o','d','u','l','e','s',0 };
	char szEnumProcesses[] = { 'E','n','u','m','P','r','o','c','e','s','s','e','s',0 };
	char szGetModuleFileNameExA[] = { 'G','e','t','M','o','d','u','l','e','F','i','l','e','N','a','m','e','E','x','A',0 };

	char szOpenProcess[] = { 'O','p','e','n','P','r','o','c','e','s','s',0 };
	char szOpenProcessToken[] = { 'O','p','e','n','P','r','o','c','e','s','s','T','o','k','e','n',0 };
	char szGetModuleBaseNameA[] = { 'G','e','t','M','o','d','u','l','e','B','a','s','e','N','a','m','e','A',0 };
	char szCreateRemoteThread[] = { 'C','r','e','a','t','e','R','e','m','o','t','e','T','h','r','e','a','d',0 };
	
	char szWriteProcessMemory[] = { 'W','r','i','t','e','P','r','o','c','e','s','s','M','e','m','o','r','y',0 };
	
	char szDebugActiveProcess[] = { 'D','e','b','u','g','A','c','t','i','v','e','P','r','o','c','e','s','s',0 };
	char szWaitForDebugEvent[] = { 'W','a','i','t','F','o','r','D','e','b','u','g','E','v','e','n','t',0 };
	char szWaitForSingleObject[] = { 'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t',0 };
	char szGetExitCodeThread[] = { 'G','e','t','E','x','i','t','C','o','d','e','T','h','r','e','a','d',0 };
	char szWaitForMultipleObjects[] = { 'W','a','i','t','F','o','r','M','u','l','t','i','p','l','e','O','b','j','e','c','t','s',0 };
	char szExitThread[] = { 'E','x','i','t','T','h','r','e','a','d',0 };
	char szContinueDebugEvent[] = {'C','o','n','t','i','n','u','e','D','e','b','u','g','E','v','e','n','t',0};
	char szSetUnhandledExceptionFilter[] =
	{ 'S','e','t','U','n','h','a','n','d','l','e','d','E','x','c','e','p','t','i','o','n','F','i','l','t','e','r',0 };

	char szCreateProcessAsUserA[] = { 'C','r','e','a','t','e','P','r','o','c','e','s','s','A','s','U','s','e','r','A',0 };

	char szCreateProcessW[] = { 'C','r','e','a','t','e','P','r','o','c','e','s','s','W',0 };

	char szOutputDebugStringW[] = { 'O','u','t','p','u','t','D','e','b','u','g','S','t','r','i','n','g','W',0 };
	char szOutputDebugStringA[] = { 'O','u','t','p','u','t','D','e','b','u','g','S','t','r','i','n','g','A',0 };



	char szCreateEventA[] = { 'C','r','e','a','t','e','E','v','e','n','t','A',0 };
	char szSetEvent[] = { 'S','e','t','E','v','e','n','t',0 };

	char szGetCurrentProcessId[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s','I','d',0 };
	char szSetFileAttributesA[] = { 'S','e','t','F','i','l','e','A','t','t','r','i','b','u','t','e','s','A',0 };

	char szDllNetApi32[] = { 'n','e','t','a','p','i','3','2','.','d','l','l',0 };
	char szNetScheduleJobAddW[] = { 'N','e','t','S','c','h','e','d','u','l','e','J','o','b','A','d','d',0 }; ;
	char szNetWkstaGetInfo[] = { 'N','e','t','W','k','s','t','a','G','e','t','I','n','f','o',0 };
	char szNetApiBufferFree[] = { 'N','e','t','A','p','i','B','u','f','f','e','r','F','r','e','e',0 };

	char szDllShell32[] = { 's','h','e','l','l','3','2','.','d','l','l',0 };
	char szShellExecuteExA[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','E','x','A',0 };
	char szShellExecuteExW[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','E','x','W',0 };

	char szDeleteFileW[] = { 'D','e','l','e','t','e','F','i','l','e','W',0 };
	char szFindNextFileA[] = { 'F','i','n','d','N','e','x','t','F','i','l','e','A',0 };
	char szFindFirstFileA[] = { 'F','i','n','d','F','i','r','s','t','F','i','l','e','A',0 };
	char szFindClose[] = { 'F','i','n','d','C','l','o','s','e',0 };

	
	char szCommandLineToArgvW[] = { 'C','o','m','m','a','n','d','L','i','n','e','T','o','A','r','g','v','W',0 };
	char szReleaseMutex[] = { 'R','e','l','e','a','s','e','M','u','t','e','x',0 };

	char szSHGetSpecialFolderPathA[] = { 'S','H','G','e','t','S','p','e','c','i','a','l','F','o','l','d','e','r','P','a','t','h','A',0 };

	char szSetCurrentDirectoryA[] = { 'S','e','t','C','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y','A',0 };

	char szRemoveDirectoryA[] = { 'R','e','m','o','v','e','D','i','r','e','c','t','o','r','y','A',0 };
	char szGetExitCodeProcess[] = { 'G','e','t','E','x','i','t','C','o','d','e','P','r','o','c','e','s','s',0 };
	char szCreatePipe[] = { 'C','r','e','a','t','e','P','i','p','e',0 };
	char szGetClassNameA[] = { 'G','e','t','C','l','a','s','s','N','a','m','e','A',0 };

	char szGetCurrentDirectoryA[] = { 'G','e','t','C','u','r','r','e','n','t','D','i','r','e','c','t','o','r','y','A',0 };
	char szDllShlwapi[] = { 's','h','l','w','a','p','i','.','d','l','l',0 };
	char szPathStripPathA[] = { 'P','a','t','h','S','t','r','i','p','P','a','t','h','A',0 };
	char szShellExecuteA[] = { 'S','h','e','l','l','E','x','e','c','u','t','e','A',0 };

	char szWow64DisableWow64FsRedirection[] =
	{ 'W','o','w','6','4','D','i','s','a','b','l','e','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0 };
	char szWow64RevertWow64FsRedirection[] =
	{ 'W','o','w','6','4','R','e','v','e','r','t','W','o','w','6','4','F','s','R','e','d','i','r','e','c','t','i','o','n',0 };

	char szFindResourceA[] = { 'F','i','n','d','R','e','s','o','u','r','c','e','A',0 };
	char szSizeofResource[] = { 'S','i','z','e','o','f','R','e','s','o','u','r','c','e',0 };
	char szLoadResource[] = { 'L','o','a','d','R','e','s','o','u','r','c','e',0 };
	char szLockResource[] = { 'L','o','c','k','R','e','s','o','u','r','c','e',0 };

	char szCheckRemoteDebuggerPresent[] =
	{ 'C','h','e','c','k','R','e','m','o','t','e','D','e','b','u','g','g','e','r','P','r','e','s','e','n','t',0 };

	char szMakeSureDirectoryPathExists[] =
	{ 'M','a','k','e','S','u','r','e','D','i','r','e','c','t','o','r','y','P','a','t','h','E','x','i','s','t','s',0 };

	PEParser::getBaseApi(&lpDllKernel32,(FARPROC*)&lpGetProcAddress, (FARPROC*)&lpLoadLibraryA);
	if (lpGetProcAddress == 0 || lpLoadLibraryA == 0 || lpDllKernel32 == 0)
	{
		MessageBoxA(0, "getBaseApi error", "getBaseApi error", MB_OK);
		return FALSE;
	}
	lpTerminateProcess = (ptrTerminateProcess)lpGetProcAddress(lpDllKernel32, szTerminateProcess);
	lpExitProcess = (ptrExitProcess)lpGetProcAddress(lpDllKernel32, szExitProcess);

	lpFindNextFileA = (ptrFindNextFileA)lpGetProcAddress(lpDllKernel32, szFindNextFileA);
	lpFindFirstFileA = (ptrFindFirstFileA)lpGetProcAddress(lpDllKernel32, szFindFirstFileA);
	lpFindClose = (ptrFindClose)lpGetProcAddress(lpDllKernel32, szFindClose);

	lpReadFile = (ptrReadFile)lpGetProcAddress(lpDllKernel32, szReadFile);
	lpFlushFileBuffers = (ptrFlushFileBuffers)lpGetProcAddress(lpDllKernel32, szFlushFileBuffers);
	lpWriteFile = (ptrWriteFile)lpGetProcAddress(lpDllKernel32, szWriteFile);
	lpGetFileSize = (ptrGetFileSize)lpGetProcAddress(lpDllKernel32, szGetFileSize);
	lpSetFilePointer = (ptrSetFilePointer)lpGetProcAddress(lpDllKernel32, szSetFilePointer);
	lpSetFileAttributesA = (ptrSetFileAttributesA)lpGetProcAddress(lpDllKernel32, szSetFileAttributesA);
	lpGetNativeSystemInfo = (ptrGetNativeSystemInfo)lpGetProcAddress(lpDllKernel32, szGetNativeSystemInfo);
	lpOutputDebugStringW = (ptrOutputDebugStringW)lpGetProcAddress(lpDllKernel32,szOutputDebugStringW);
	lpOutputDebugStringA = (ptrOutputDebugStringA)lpGetProcAddress(lpDllKernel32, szOutputDebugStringA);
	
	lpGetComputerNameA = (ptrGetComputerNameA)lpGetProcAddress(lpDllKernel32, szGetComputerNameA);
	lpWinExec = (ptrWinExec)lpGetProcAddress(lpDllKernel32, szWinExec);
	lpGetSystemDirectoryA = (ptrGetSystemDirectoryA)lpGetProcAddress(lpDllKernel32, szGetSystemDirectoryA);
	lpGetModuleHandleA = (ptrGetModuleHandleA)lpGetProcAddress(lpDllKernel32, szGetModuleHandleA);
	lpVirtualAlloc = (ptrVirtualAlloc)lpGetProcAddress(lpDllKernel32, szVirtualAlloc);
	lpVirtualFree = (ptrVirtualFree)lpGetProcAddress(lpDllKernel32, szVirtualFree);
	lpVirtualProtect = (ptrVirtualProtect)lpGetProcAddress(lpDllKernel32, szVirtualProtect);
	lpGetCommandLineW = (ptrGetCommandLineW)lpGetProcAddress(lpDllKernel32, szGetCommandLineW);
	lpCreateMutexA = (ptrCreateMutexA)lpGetProcAddress(lpDllKernel32, szCreateMutexA);
	lpCloseHandle = (ptrCloseHandle)lpGetProcAddress(lpDllKernel32, szCloseHandle);
	lpGetCurrentProcessId = (ptrGetCurrentProcessId)lpGetProcAddress(lpDllKernel32, szGetCurrentProcessId);
	lpCreateThread = (ptrCreateThread)lpGetProcAddress(lpDllKernel32, szGetProcAddress);
	lpWaitForDebugEvent = (ptrWaitForDebugEvent)lpGetProcAddress(lpDllKernel32, szWaitForDebugEvent);
	lpDebugActiveProcess = (ptrDebugActiveProcess)lpGetProcAddress(lpDllKernel32, szDebugActiveProcess);

	lpIsDebuggerPresent = (ptrIsDebuggerPresent)lpGetProcAddress(lpDllKernel32, szIsDebuggerPresent);
	lpContinueDebugEvent = (ptrContinueDebugEvent)lpGetProcAddress(lpDllKernel32, szContinueDebugEvent);
	lpCreateFileA = (ptrCreateFileA)lpGetProcAddress(lpDllKernel32, szCreateFileA);
	lpSleep = (ptrSleep)lpGetProcAddress(lpDllKernel32, szSleep);
	lpSetUnhandledExceptionFilter = (ptrSetUnhandledExceptionFilter)lpGetProcAddress(lpDllKernel32, szSetUnhandledExceptionFilter);
	lpWaitForSingleObject = (ptrWaitForSingleObject)lpGetProcAddress(lpDllKernel32, szWaitForSingleObject);

	lpGetTickCount = (ptrGetTickCount)lpGetProcAddress(lpDllKernel32, szGetTickCount);
	lpGetTickCount64 = (ptrGetTickCount64)lpGetProcAddress(lpDllKernel32, szGetTickCount64);

	lpCreateToolhelp32Snapshot = (ptrCreateToolhelp32Snapshot)lpGetProcAddress(lpDllKernel32, szCreateToolhelp32Snapshot);

	lpProcess32FirstW =(ptrProcess32FirstW)lpGetProcAddress(lpDllKernel32, szProcess32FirstW);

	lpProcess32NextW= (ptrProcess32NextW)lpGetProcAddress(lpDllKernel32, szProcess32NextW);

	lpCheckRemoteDebuggerPresent = (ptrCheckRemoteDebuggerPresent)lpGetProcAddress(lpDllKernel32, szCheckRemoteDebuggerPresent);
	lpOpenProcess = (ptrOpenProcess)lpGetProcAddress(lpDllKernel32, szOpenProcess);

	lpCreateProcessAsUserA = (ptrCreateProcessAsUserA)lpGetProcAddress(lpDllKernel32, szCreateProcessAsUserA);


	lpCreateProcessW = (ptrCreateProcessW)lpGetProcAddress(lpDllKernel32, szCreateProcessW);
	lpGetCurrentProcess = (ptrGetCurrentProcess)lpGetProcAddress(lpDllKernel32, szGetCurrentProcess);

	lpDlladvapi32 = lpLoadLibraryA(szDlladvapi32);
	if (lpDlladvapi32 == 0) {
		MessageBoxA(0, "lpDlladvapi32 error", "lpDlladvapi32 error", MB_OK);
		return FALSE;
	}

	lpRegSetValueExA = (ptrRegSetValueExA)lpGetProcAddress(lpDlladvapi32, szRegSetValueExA);
	lpRegCreateKeyExA = (ptrRegCreateKeyExA)lpGetProcAddress(lpDlladvapi32, szRegCreateKeyExA);
	lpRegCloseKey = (ptrRegCloseKey)lpGetProcAddress(lpDlladvapi32, szRegCloseKey);
	lpRegQueryValueExA = (ptrRegQueryValueExA)lpGetProcAddress(lpDlladvapi32, szRegQueryValueExA);

	lpGetUserNameA = (ptrGetUserNameA)lpGetProcAddress(lpDlladvapi32, szGetUserNameA);

	lpOpenProcessToken = (ptrOpenProcessToken)lpGetProcAddress(lpDlladvapi32, szOpenProcessToken);

	lpDllNetApi32 = lpLoadLibraryA(szDllNetApi32);
	if (lpDllNetApi32 == 0) {
		MessageBoxA(0, "lpDllNetApi32 error", "lpDllNetApi32 error", MB_OK);
		return FALSE;
	}
	lpNetWkstaGetInfo = (ptrNetWkstaGetInfo)lpGetProcAddress(lpDllNetApi32, szNetWkstaGetInfo);
	lpNetApiBufferFree = (ptrNetApiBufferFree)lpGetProcAddress(lpDllNetApi32, szNetApiBufferFree);

	lpDllShell32 = lpLoadLibraryA(szDllShell32);
	if (lpDllShell32 == 0) {
		MessageBoxA(0, "lpDllShell32 error", "lpDllShell32 error", MB_OK);
		return FALSE;
	}
	lpShellExecuteA = (ptrShellExecuteA)lpGetProcAddress(lpDllShell32, szShellExecuteA);
	lpCommandLineToArgvW = (ptrCommandLineToArgvW)lpGetProcAddress(lpDllShell32, szCommandLineToArgvW);
	lpSHGetSpecialFolderPathA = (ptrSHGetSpecialFolderPathA)lpGetProcAddress(lpDllShell32, szSHGetSpecialFolderPathA);

	char szDbghelpDll[] = { 'D','b','g','h','e','l','p','.','d','l','l',0 };
	lpDllDbghelp = lpLoadLibraryA(szDbghelpDll);
	if (lpDllDbghelp == 0) {
		MessageBoxA(0, "lpDllDbghelp error", "lpDllDbghelp error", MB_OK);
		return FALSE;
	}
	lpMakeSureDirectoryPathExists = (ptrMakeSureDirectoryPathExists)lpGetProcAddress(lpDllDbghelp, szMakeSureDirectoryPathExists);

	char szUserEnv[] = { 'u','s','e','r','e','n','v','.','d','l','l',0 };
	HMODULE lpDllUserEnv = lpLoadLibraryA(szUserEnv);
	char szDestroyEnvironmentBlock[] = 
	{ 'D','e','s','t','r','o','y','E','n','v','i','r','o','n','m','e','n','t','B','l','o','c','k',0 };
	char szCreateEnvironmentBlock[] = 
	{ 'C','r','e','a','t','e','E','n','v','i','r','o','n','m','e','n','t','B','l','o','c','k',0 };
	lpDestroyEnvironmentBlock = (ptrDestroyEnvironmentBlock)lpGetProcAddress(lpDllUserEnv, szDestroyEnvironmentBlock);
	lpCreateEnvironmentBlock = (ptrCreateEnvironmentBlock)lpGetProcAddress(lpDllUserEnv, szCreateEnvironmentBlock);


// 	sprintf(info, "address:%p", lpMakeSureDirectoryPathExists);
// 	MessageBoxA(0, info, info, MB_OK);


	HMODULE hDllWs2_32 = lpLoadLibraryA(szDllWs2_32);
	lpWSAStartup = (ptrWSAStartup)lpGetProcAddress(hDllWs2_32, szWSAStartup);

	WSAData wsa = { 0 };
	ret = lpWSAStartup(0x0202, &wsa);

	testapi();

	return TRUE;
}



void testapi() {
	int ret = 0;

	return;
}