#include <windows.h>
#include <stdio.h>
#include "LoadPE.h"
#include "PEParser.h"
#include "ExportTable.h"
#include "ImportTable.h"
#include "Reloc.h"
#include "Public.h"
#include "FileHelper.h"
#include "main.h"
#include "api.h"
#include "pe64.h"
#include "utils.h"


PIMAGE_EXPORT_DIRECTORY pThisEAT = 0;


int GetPeBits(char* data) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)data;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(data + dos->e_lfanew);
	WORD magic = nt->OptionalHeader.Magic;
	if(magic == 0x10b){
		return 32;
	}
	else if (magic == 0x20b) {
		return 64;
	}
	return 0;
}


int GetPeType(const char* pedata) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pedata;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pedata + dos->e_lfanew);
	int c = nt->FileHeader.Characteristics;
	if (c & 0x2000) {
		return 2;
	}
	else if (c & 2) {
		return 1;
	}
	
	return 0;
}

unsigned long long GetImageEntry(char* module) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(module + dos->e_lfanew);
	int magic = nt->OptionalHeader.Magic;
	if (magic == 0x10b) {
		return (unsigned long long)(module + nt->OptionalHeader.AddressOfEntryPoint);
	}
	else if (magic == 0x20b) {
		PIMAGE_NT_HEADERS64 nt64 = (PIMAGE_NT_HEADERS64)(module + dos->e_lfanew);
		return (unsigned long long)(module + nt64->OptionalHeader.AddressOfEntryPoint);
	}
	return 0;
}

QWORD LoadPE::GetSizeOfImage(char* pe)
{
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pe;

	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pe + dos->e_lfanew);
	int magic = nt->OptionalHeader.Magic;
	if (magic == 0x10b)
	{
		return  nt->OptionalHeader.SizeOfImage;
	}
	else if (magic == 0x20b) {
		PIMAGE_NT_HEADERS64 nt64 = (PIMAGE_NT_HEADERS64)(pe + dos->e_lfanew);
		return nt64->OptionalHeader.SizeOfImage;
	}
	return 0;
}


ULONGLONG LoadPE::GetImageBase(char* pe)
{
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pe;

	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pe + dos->e_lfanew);
	int magic = nt->OptionalHeader.Magic;
	if (magic == 0x10b)
	{
		return  nt->OptionalHeader.ImageBase;
	}
	else if (magic == 0x20b) {
		PIMAGE_NT_HEADERS64 nt64 = (PIMAGE_NT_HEADERS64)(pe + dos->e_lfanew);
		return nt64->OptionalHeader.ImageBase;
	}

	return 0;
}




int LoadPE::SetImageBase(char* pe)
{
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pe;

	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pe + dos->e_lfanew);
	int magic = nt->OptionalHeader.Magic;
	if (magic == 0x10b)
	{
		nt->OptionalHeader.ImageBase =(DWORD) pe;
	}
	else if (magic == 0x20b) {
		PIMAGE_NT_HEADERS64 nt64 = (PIMAGE_NT_HEADERS64)(pe + dos->e_lfanew);
		nt64->OptionalHeader.ImageBase = (QWORD)pe;
	}

	return TRUE;
}




int recoverEAT(char * dllbase) {
	int ret = 0;
	PIMAGE_DOS_HEADER thisdos = (PIMAGE_DOS_HEADER)ghThisHandle;
	PIMAGE_NT_HEADERS thisnt = (PIMAGE_NT_HEADERS)(ghThisHandle + thisdos->e_lfanew);
	PIMAGE_EXPORT_DIRECTORY thiseat = (PIMAGE_EXPORT_DIRECTORY)((char*)ghThisHandle +
		thisnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	pThisEAT = thiseat;

	PIMAGE_DOS_HEADER dlldos = (PIMAGE_DOS_HEADER)dllbase;
	PIMAGE_NT_HEADERS dllnt = (PIMAGE_NT_HEADERS)(dllbase + dlldos->e_lfanew);

	int dlleatsize = dllnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	PIMAGE_EXPORT_DIRECTORY dlleat = (PIMAGE_EXPORT_DIRECTORY)((char*)dllbase +
		dllnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

	DWORD dwOldProtect = 0;
	ret = lpVirtualProtect(ghThisHandle, thisnt->OptionalHeader.SectionAlignment, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	if (FALSE == ret)
	{
		//Public::writelog("VirtualProtect error");
		return NULL;
	}

	int alignsizedlleat = thisnt->OptionalHeader.SectionAlignment - (dlleatsize %thisnt->OptionalHeader.SectionAlignment) + dlleatsize;

	ret = lpVirtualProtect(dlleat, alignsizedlleat, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	if (FALSE == ret)
	{
		//Public::writelog("VirtualProtect error");
		return NULL;
	}

	int rvadelta = dllbase - ghThisHandle;

	dlleat->Name = dlleat->Name + rvadelta;
	DWORD * dlladdresses = (DWORD*)(dllbase + dlleat->AddressOfFunctions);
	int totalfun = dlleat->NumberOfFunctions ;
	for (int i = 0;i < totalfun;i++)
	{
		dlladdresses[i] = dlladdresses[i] + rvadelta;
	}
	dlleat->AddressOfFunctions = dlleat->AddressOfFunctions + rvadelta;

	DWORD * namefun = (DWORD *)(dllbase + dlleat->AddressOfNames);
	int funnames = dlleat->NumberOfNames;
	for (int i = 0 ;i < funnames ; i ++)
	{
		namefun[i] = namefun[i] + rvadelta;
	}

	dlleat->AddressOfNames = dlleat->AddressOfNames + rvadelta;

	dlleat->AddressOfNameOrdinals = dlleat->AddressOfNameOrdinals + rvadelta;

	thisnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress = (char*)dlleat - ghThisHandle;;
	thisnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size =
		dllnt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

	return TRUE;
}






int LoadPE::ImportTable(char* chBaseAddress)
{
// 	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };
// 	char szGetModuleHandleW[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','W',0 };
// 	char szInitializeSListHead[] = { 'I','n','i','t','i','a','l','i','z','e','S','L','i','s','t','H','e','a','d',0 };

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);

	PIMAGE_IMPORT_DESCRIPTOR pImportTable = (PIMAGE_IMPORT_DESCRIPTOR)((char*)pDos +
		pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	while (TRUE)
	{
		if (0 == pImportTable->OriginalFirstThunk)
		{
			break;
		}

		char *lpDllName = (char *)((char*)pDos + pImportTable->Name);
		HMODULE hDll = (HMODULE)lpGetModuleHandleA((LPSTR)lpDllName);
		if (NULL == hDll)
		{
			hDll = LoadLibraryA(lpDllName);
			if (NULL == hDll)
			{
				pImportTable++;
				continue;
			}
		}
		
		DWORD i = 0;

		PIMAGE_THUNK_DATA lpImportNameArray = (PIMAGE_THUNK_DATA)((char*)pDos + pImportTable->OriginalFirstThunk);

		PIMAGE_THUNK_DATA lpImportFuncAddrArray = (PIMAGE_THUNK_DATA)((char*)pDos + pImportTable->FirstThunk);
		while (TRUE)
		{
			if (0 == lpImportNameArray[i].u1.AddressOfData)
			{
				break;
			}

			FARPROC lpFuncAddress = NULL;

			if (0x80000000 & lpImportNameArray[i].u1.Ordinal)
			{
				lpFuncAddress = (FARPROC)GetProcAddress(hDll, (LPSTR)(lpImportNameArray[i].u1.Ordinal & 0x0000FFFF));
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME lpImportByName = (PIMAGE_IMPORT_BY_NAME)((char*)pDos + lpImportNameArray[i].u1.AddressOfData);

				lpFuncAddress = (FARPROC)lpGetProcAddress(hDll, (LPSTR)lpImportByName->Name);
			}

			if (lpFuncAddress > 0)
			{
				lpImportFuncAddrArray[i].u1.Function = (ULONGLONG)lpFuncAddress;
			}
			
			i++;
		}

		pImportTable++;
	}

	return TRUE;
}

int LoadPE::RelocationTable(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)(chBaseAddress +
		pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	if ((char*)pLoc == (char*)pDos)
	{
		return TRUE;
	}

	while ((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0)
	{
		WORD *pLocData = (WORD *)((PBYTE)pLoc + sizeof(IMAGE_BASE_RELOCATION));

		int nNumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

		ULONGLONG dwDelta = (ULONGLONG)pDos - pNt->OptionalHeader.ImageBase;

		for (int i = 0; i < nNumberOfReloc; i++)
		{
			if ((pLocData[i] & 0xF000) == 0x3000 || (pLocData[i] & 0xF000) == 0xA000)
			//if ((DWORD)(pLocData[i] & 0x0000F000) == 0x00003000)
			{
				ULONGLONG* pAddress = (ULONGLONG*)((PBYTE)pDos + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				
				*pAddress += dwDelta;
			}
		}

		pLoc = (PIMAGE_BASE_RELOCATION)((PBYTE)pLoc + pLoc->SizeOfBlock);
	}

	return TRUE;
}





int LoadPE::MapFile(char* pefile, char* buf)
{
	PIMAGE_SECTION_HEADER sections = 0;
	int seccnt = 0;

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)pefile;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(pefile + dos->e_lfanew);
	int magic = nt->OptionalHeader.Magic;
	if (magic == 0x10b) {
		sections = IMAGE_FIRST_SECTION(nt);
		memcpy(buf, pefile, nt->OptionalHeader.SizeOfHeaders);
		seccnt = nt->FileHeader.NumberOfSections;
	}
	else if (magic == 0x20b) {
		PIMAGE_NT_HEADERS64 nt64 = (PIMAGE_NT_HEADERS64)(pefile + dos->e_lfanew);
		sections = IMAGE_FIRST_SECTION(nt64);
		memcpy(buf, pefile, nt64->OptionalHeader.SizeOfHeaders);
		seccnt = nt64->FileHeader.NumberOfSections;
	}
	else {
		return 0;
	}

	for (int i = 0; i < seccnt; i++, sections++)
	{
		if ((0 == sections->VirtualAddress) ||(0 == sections->SizeOfRawData))
		{
			continue;
		}

		char* dst = (char*)(buf + sections->VirtualAddress);
		char* src = (char*)(pefile + sections->PointerToRawData);

		memcpy(dst, src, sections->SizeOfRawData);
	}

	return seccnt;
}



BOOL SetupConsole() {
	INT ret = 0;
	if (!AllocConsole()) {
		runLog("AllocConsole failed (%d)\n", GetLastError());
		return FALSE;
	}

	// 重定向标准流
	FILE * f= freopen("CONOUT$", "w", stdout);
	f = freopen("CONOUT$", "w", stderr);
	f = freopen("CONIN$", "r", stdin);

	// 重置C运行时标准流
	HANDLE hConOut = CreateFileA("CONOUT$", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFileA("CONIN$", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);

	return TRUE;
}



int LoadPE::CallConsoleEntry(char* module)
{
	int ret = 0;

	ret = SetupConsole();

	ptrmain lpmain = (ptrmain)GetImageEntry(module);

	char szparams[16][256] = { 0 };	//not [1024][16]
	int iArgc = 0;
	wchar_t * *wszparams = lpCommandLineToArgvW(lpGetCommandLineW(), &iArgc);
	for (int i = 0; i < iArgc; i ++)
	{
		char szparam[256] = { 0 };
		ret = WideCharToMultiByte(CP_ACP, 0, wszparams[i], -1, szparam, sizeof(szparam), 0, 0);
		if (ret > 0)
		{
			lstrcpyA(szparams[i], szparam);
			runLog("run console param:%s\r\n", szparam);
		}
	}

	ret = lpmain(iArgc, (char**)szparams);

	return ret;
}


int LoadPE::CallDllEntry(char* module)
{
	ptrDllMainEntry dllmain = (ptrDllMainEntry) GetImageEntry(module);

	int ret = dllmain((char*)module, DLL_PROCESS_ATTACH, 0);

	return ret;
}


//MajorOperatingSystemVersion&&MinorOperatingSystemVersion 5.01	= xp
//MajorLinkerVersion和MinorLinkerVersion是链接器版本的高位和低位 14.0 = vs2015
//MajorSubsystemVersion、MinorSubsystemVersion 5.01 = xp
//Subsystem IMAGE_SUBSYSTEM_WINDOWS_CUI=3  IMAGE_SUBSYSTEM_WINDOWS_GUI =2
int LoadPE::CallExeEntry(char* module)
{
	int callret = 0;

	ptrWinMain lpWinMain = (ptrWinMain)GetImageEntry(module);

	__try {
		runLog("run exe address:%p,hinstance:%x,cmdline:%p,showmode:%d\r\n", module,ghPrevInstance,glpCmdLine,gnShowCmd);

		callret = lpWinMain((HINSTANCE)module, ghPrevInstance, glpCmdLine, gnShowCmd);
	}
	__except (1) {
		runLog("run exe exception:%p,hinstance:%x,cmdline:%p,showmode:%d\r\n", module, ghPrevInstance, glpCmdLine, gnShowCmd);
	}

	return callret;
}


int LoadPE::RunPE(char* fbuf, DWORD fs)
{
	int ret = 0;

	int bits = GetPeBits(fbuf);

	unsigned __int64 imagesize = GetSizeOfImage(fbuf);

	ULONGLONG imagebase = GetImageBase(fbuf);
	if (imagebase <= 0)
	{
		imagebase = DEFAULT_PE_BASE_ADDRESS;
	}

	char* base = (char*)lpVirtualAlloc((char*)imagebase, (DWORD)imagesize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (NULL == base)
	{
		base = (char*)lpVirtualAlloc(0, (DWORD)imagesize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (NULL == base)
		{
			runLog("VirtualAlloc address:%x error", imagebase);
			return NULL;
		}
	}

	RtlZeroMemory(base, (DWORD)imagesize);

	if (bits == 64) {
		ret = mapFile64(fbuf, base);
		ret = importTable64(base);
		ret = relocTable64(base, (ULONGLONG)base);
	}
	else if (bits == 32) {
		ret = MapFile(fbuf, base);
		//Reloc::recovery((DWORD)base);
		ret = RelocationTable(base);

		//ImportTable::recover((DWORD)base);
		ret = ImportTable(base);
	}

	DWORD dwOldProtect = 0;
	if (FALSE == lpVirtualProtect(base, (DWORD)imagesize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		lpVirtualFree(base, (DWORD)imagesize, MEM_DECOMMIT|MEM_RELEASE);
		lpVirtualFree(base, 0, MEM_RELEASE);
		runLog( "VirtualProtect address:%x error", imagebase);
		return NULL;
	}

	ret = SetImageBase(base);

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(base + dos->e_lfanew);
	if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	{

	}
	else if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
	{
		ret = CallConsoleEntry(base);
		lpVirtualFree(base, (DWORD)imagesize, MEM_DECOMMIT);
		lpVirtualFree(base, 0, MEM_RELEASE);
		return ret;
	}

	if (nt->FileHeader.Characteristics & 0x2000)
	{
		ret = recoverEAT(base);
		ret = CallDllEntry(base);
		return ret;
	}
	else {
		ret = CallExeEntry(base);
		lpVirtualFree(base, (DWORD)imagesize, MEM_DECOMMIT);
		lpVirtualFree(base, 0, MEM_RELEASE);
		return ret;
	}

	return TRUE;
}


int LoadPE::load(const char *fn)
{
	int ret = 0;
	char * file = 0;
	int fs = 0;
	ret = FileHelper::fileReader(fn, &file, &fs);
	if (ret <= 0)
	{
		return FALSE;
	}

	if (PEParser::isPE(file))
	{
		ret = RunPE(file, fs);
	}

	delete[] file;
	return 0;
}