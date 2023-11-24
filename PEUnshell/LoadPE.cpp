#include <windows.h>
#include <stdio.h>
#include "LoadPE.h"
#include "PEParser.h"
#include "ExportFunTable.h"
#include "ImportFunTable.h"
#include "Reloc.h"
#include "Public.h"
#include "FileHelper.h"
#include "main.h"
#include "api.h"

//#define _MYDEBUG

PIMAGE_EXPORT_DIRECTORY pThisEAT = 0;


DWORD LoadPE::GetSizeOfImage(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);
	DWORD dwSizeOfImage = pNt->OptionalHeader.SizeOfImage;

	return dwSizeOfImage;
}


ULONGLONG LoadPE::GetImageBase(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);
	ULONGLONG imagebase = pNt->OptionalHeader.ImageBase;

	return imagebase;
}

//why need to modify imagebase？
bool LoadPE::SetImageBase(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	pNt->OptionalHeader.ImageBase = (ULONGLONG)chBaseAddress;

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






bool LoadPE::ImportTable(char* chBaseAddress)
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
			hDll = lpLoadLibraryA(lpDllName);
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
				lpFuncAddress = (FARPROC)lpGetProcAddress(hDll, (LPSTR)(lpImportNameArray[i].u1.Ordinal & 0x0000FFFF));
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

bool LoadPE::RelocationTable(char* chBaseAddress)
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
			if ((DWORD)(pLocData[i] & 0x0000F000) == 0x00003000)
			{
				DWORD* pAddress = (DWORD *)((PBYTE)pDos + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				
				*pAddress += dwDelta;
			}
		}

		pLoc = (PIMAGE_BASE_RELOCATION)((PBYTE)pLoc + pLoc->SizeOfBlock);
	}

	return TRUE;
}

bool LoadPE::MapFile(char* pFileBuff, char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(pFileBuff + pDos->e_lfanew);

	memcpy(chBaseAddress, pFileBuff, pNt->OptionalHeader.SizeOfHeaders);

	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	int nNumerOfSections = pNt->FileHeader.NumberOfSections;
	for (int i = 0; i < nNumerOfSections; i++, pSection++)
	{
		if ((0 == pSection->VirtualAddress) ||(0 == pSection->SizeOfRawData))
		{
			continue;
		}

		char* chDestMem = (char*)(chBaseAddress + pSection->VirtualAddress);
		char* chSrcMem = (char*)(pFileBuff + pSection->PointerToRawData);

		memcpy(chDestMem, chSrcMem, pSection->SizeOfRawData);
	}

	return TRUE;
}






int LoadPE::CallConsoleEntry(char* chBaseAddress)
{
	int ret = 0;

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpmain = (ptrmain)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);

	char szparams[16][256] = { 0 };	//not [1024][16]
	int iArgc = 0;
	wchar_t * *wszparams = lpCommandLineToArgvW(lpGetCommandLineW(), &iArgc);
	for (int i = 0; i < iArgc; i ++)
	{
		char szparam[256] = { 0 };
		ret = WideCharToMultiByte(CP_ACP, 0, wszparams[i], -1, szparam, 256, 0, 0);
		if (ret > 0)
		{
			lstrcpyA(szparams[i], szparam);
		}
	}
#ifndef _WIN64
	__asm {
		mov eax, gRegEax
		mov ecx, gRegEcx
		mov edx, gRegEdx
		mov ebx, gRegEbx
		//mov ebp, gRegEbp
		//mov esp, gRegEsp
		mov esi, gRegEsi
		mov edi, gRegEdi
	}
#endif
	ret = glpmain(iArgc, (char**)szparams);

	return ret;
}


int LoadPE::CallDllEntry(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpDllMainEntry = (ptrDllMainEntry)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);
#ifndef _WIN64
	__asm {
		mov eax, gRegEax
		mov ecx, gRegEcx
		mov edx, gRegEdx
		mov ebx, gRegEbx
		//mov ebp, gRegEbp
		//mov esp, gRegEsp
		mov esi, gRegEsi
		mov edi, gRegEdi
	}
#endif
	int ret = glpDllMainEntry((char*)chBaseAddress, DLL_PROCESS_ATTACH, 0);

	return ret;
}


//MajorOperatingSystemVersion&&MinorOperatingSystemVersion 5.01	= xp
//MajorLinkerVersion和MinorLinkerVersion是链接器版本的高位和低位 14.0 = vs2015
//MajorSubsystemVersion、MinorSubsystemVersion 5.01 = xp
//Subsystem IMAGE_SUBSYSTEM_WINDOWS_CUI=3  IMAGE_SUBSYSTEM_WINDOWS_GUI =2
int LoadPE::CallExeEntry(char* chBaseAddress)
{
	int callret = 0;
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS pNt = (PIMAGE_NT_HEADERS)(chBaseAddress + pDos->e_lfanew);
	glpWinMain = (ptrWinMain)(chBaseAddress + pNt->OptionalHeader.AddressOfEntryPoint);
// 	__asm
// 	{
// 		mov eax, glpWinMain
// 		jmp eax
// 		mov callret,eax
// 	}
#ifdef _MYDEBUG
	char szout[1024];
	wsprintfA(szout, "base:%x,entry:%x", chBaseAddress, glpWinMain);
	MessageBoxA(0, szout, szout, MB_OK);
#endif

	__try {
#ifndef _WIN64
		__asm {
			mov eax,gRegEax
			mov ecx,gRegEcx
			mov edx,gRegEdx
			mov ebx,gRegEbx
			//mov ebp,gRegEbp
			//mov esp,gRegEsp
			mov esi,gRegEsi
			mov edi,gRegEdi
		}
#endif
		callret = glpWinMain((HINSTANCE)chBaseAddress, ghprevInstance, glpCmdLine, gnShowCmd);
	}
	__except (1) {
		printf("main process exception\r\n");
	}

	return callret;
}


int LoadPE::RunPE(char* pFileBuff, DWORD dwSize)
{
	int ret = 0;

	char szout[1024];

	DWORD dwSizeOfImage = GetSizeOfImage(pFileBuff);

	ULONGLONG imagebase = GetImageBase(pFileBuff);
	if (imagebase <= 0)
	{
		imagebase = DEFAULT_PE_BASE_ADDRESS;
	}

#ifdef _MYDEBUG
	wsprintfA(szout, "image base:%x,size:%x", imagebase, dwSizeOfImage);
	MessageBoxA(0, szout, szout, MB_OK);
#endif

	//使用MEM_RESERVE分配类型参数 Windows会以64 KB为边界计算该区域的起始地址 跟PE文件加载边界一致
	//使用MEM_COMMIT分配类型参数 区域的起始和结束地址都被计算到4KB边界
	//VirtualAlloc 当程序访问这部分内存时RAM内存才会被真正分配
	char* chBaseAddress = (char*)lpVirtualAlloc((char*)imagebase, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (NULL == chBaseAddress)
	{
#ifdef _MYDEBUG
		wsprintfA(szout, "VirtualAlloc address:%x error", imagebase);
		MessageBoxA(0, szout, szout, MB_OK);
#endif
		chBaseAddress = (char*)lpVirtualAlloc(0, dwSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (NULL == chBaseAddress)
		{
#ifdef _MYDEBUG
			wsprintfA(szout, "VirtualAlloc address:%x error", imagebase);
			MessageBoxA(0, szout, szout, MB_OK);
#endif
			return NULL;
		}
	}

	RtlZeroMemory(chBaseAddress, dwSizeOfImage);

	ret = MapFile(pFileBuff, chBaseAddress);

	//Reloc::recovery((DWORD)chBaseAddress);
	ret = RelocationTable(chBaseAddress);

	//ImportFunTable::recover((DWORD)chBaseAddress);
	ret = ImportTable(chBaseAddress);

	DWORD dwOldProtect = 0;
	if (FALSE == lpVirtualProtect(chBaseAddress, dwSizeOfImage, PAGE_EXECUTE_READWRITE, &dwOldProtect))
	{
		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
#ifdef _MYDEBUG
		wsprintfA(szout, "VirtualProtect address:%x error", imagebase);
		MessageBoxA(0, szout, szout, MB_OK);
#endif
		return NULL;
	}

	ret = SetImageBase(chBaseAddress);

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(chBaseAddress + dos->e_lfanew);

#ifdef _MYDEBUG
	wsprintfA(szout, "pe type:%x", nt->FileHeader.Characteristics);
	MessageBoxA(0, szout, szout, MB_OK);
#endif

	if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_GUI)
	{

	}
	else if (nt->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI)
	{
		gType = 3;
		ghPEModule = (HMODULE)chBaseAddress;
		gPEImageSize = dwSizeOfImage;

		ret = CallConsoleEntry(chBaseAddress);

		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
		return ret;
	}

	if (nt->FileHeader.Characteristics & 0x2000)
	{
		gType = 2;
		gPEImageSize = dwSizeOfImage;
		ghPEModule = (HMODULE)chBaseAddress;

		ret = recoverEAT(chBaseAddress);
		ret = CallDllEntry(chBaseAddress);
		return ret;
	}
	else {
		gType = 1;
		ghPEModule = (HMODULE)chBaseAddress;
		gPEImageSize = dwSizeOfImage;

		ret = CallExeEntry(chBaseAddress);

		lpVirtualFree(chBaseAddress, dwSizeOfImage, MEM_DECOMMIT);
		lpVirtualFree(chBaseAddress, 0, MEM_RELEASE);
		return ret;
	}

	return TRUE;
}


int LoadPE::load(const char *szFileName)
{
	int ret = 0;
	char * pData = 0;
	int dwFileSize = 0;
	ret = FileHelper::fileReader(szFileName, &pData, &dwFileSize);
	if (ret <= 0)
	{
		return FALSE;
	}

	if (PEParser::isPE(pData))
	{
		ret = RunPE(pData, dwFileSize);
	}

	delete[] pData;
	return 0;
}