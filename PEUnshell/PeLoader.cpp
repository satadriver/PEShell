


#include "PEloader.h"


typedef BOOL(__stdcall* ProcDllMain)(HINSTANCE, DWORD, LPVOID);

typedef INT(__stdcall* ProcExeMain)(HINSTANCE,HINSTANCE,int,int);
typedef DWORD(__stdcall* STARTWORK)(LPVOID lpParamter);


BOOL g_bIsLoadOk = FALSE;
ProcDllMain pDllMain = NULL;
ProcExeMain pExeMain = NULL;

void* g_pMemoryAddress = NULL;



void DoRelocation(void* lpNewImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;

	pDosHeader = (PIMAGE_DOS_HEADER)(POINTER_TYPE)lpNewImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS)((POINTER_TYPE)lpNewImageBase + pDosHeader->e_lfanew);

	POINTER_TYPE OffSet = (POINTER_TYPE)lpNewImageBase - pNTHeader->OptionalHeader.ImageBase;

	PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)((POINTER_TYPE)lpNewImageBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	while ((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0)
	{
		WORD* pLocData = (WORD*)((POINTER_TYPE)pLoc + sizeof(IMAGE_BASE_RELOCATION));

		int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

		for (int i = 0; i < NumberOfReloc; i++)
		{
			if ((pLocData[i] & 0xF000) == 0x3000 || (pLocData[i] & 0xF000) == 0xA000)
			{
				POINTER_TYPE* pAddress = (POINTER_TYPE*)((POINTER_TYPE)lpNewImageBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				*pAddress += OffSet;
			}
		}
		pLoc = (PIMAGE_BASE_RELOCATION)((POINTER_TYPE)pLoc + pLoc->SizeOfBlock);
	}

	pNTHeader->OptionalHeader.ImageBase = (POINTER_TYPE)lpNewImageBase;

	return;
}


void DoNewRelocation(void* lpNewImageBase, void* lpOldImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;

	pDosHeader = (PIMAGE_DOS_HEADER)(POINTER_TYPE)lpOldImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS)((POINTER_TYPE)lpOldImageBase + pDosHeader->e_lfanew);

	POINTER_TYPE OffSet = (POINTER_TYPE)lpNewImageBase - (POINTER_TYPE)lpOldImageBase;

	PIMAGE_BASE_RELOCATION pLoc = (PIMAGE_BASE_RELOCATION)((POINTER_TYPE)lpOldImageBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	while ((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0)
	{
		WORD* pLocData = (WORD*)((POINTER_TYPE)pLoc + sizeof(IMAGE_BASE_RELOCATION));
		int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		for (int i = 0; i < NumberOfReloc; i++)
		{
			if ((pLocData[i] & 0xF000) == 0x3000 || (pLocData[i] & 0xF000) == 0xA000)
			{
				POINTER_TYPE* pAddress = (POINTER_TYPE*)((POINTER_TYPE)lpOldImageBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				*pAddress += OffSet;
			}
		}
		pLoc = (PIMAGE_BASE_RELOCATION)((POINTER_TYPE)pLoc + pLoc->SizeOfBlock);
	}

	pNTHeader->OptionalHeader.ImageBase = (POINTER_TYPE)lpNewImageBase;

	return;
}



BOOL FillRavAddress(void* pImageBase)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;

	pDosHeader = (PIMAGE_DOS_HEADER)(POINTER_TYPE)pImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS)((POINTER_TYPE)pImageBase + pDosHeader->e_lfanew);
	POINTER_TYPE Offset = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (Offset == 0)
		return TRUE;

	PIMAGE_IMPORT_DESCRIPTOR pID = (PIMAGE_IMPORT_DESCRIPTOR)((POINTER_TYPE)pImageBase + Offset);

	char* sDllName = NULL;
	HINSTANCE hDll = NULL;

	while (1)
	{

		if ((pID->TimeDateStamp == 0) && (pID->Name == 0))
			break;
		sDllName = (char*)(pID->Name + (POINTER_TYPE)pImageBase);

		HMODULE hDll = GetModuleHandleA(sDllName);

		if (hDll == NULL)
		{
			hDll = LoadLibraryA(sDllName);
			if (hDll == NULL)
				return FALSE;
		}

		PIMAGE_THUNK_DATA pOriginalIAT = NULL;
		PIMAGE_THUNK_DATA pRealIAT = (PIMAGE_THUNK_DATA)((POINTER_TYPE)pImageBase + pID->FirstThunk);

		if (pID->OriginalFirstThunk)
			pOriginalIAT = (PIMAGE_THUNK_DATA)((POINTER_TYPE)pImageBase + pID->OriginalFirstThunk);
		else
			pOriginalIAT = (PIMAGE_THUNK_DATA)((POINTER_TYPE)pImageBase + pID->FirstThunk);

		for (int i = 0;; i++)
		{
			if (pOriginalIAT[i].u1.Function == 0)
				break;

			FARPROC lpFunction = NULL;
			if (pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				lpFunction = GetProcAddress(hDll, (LPCSTR)IMAGE_ORDINAL((pOriginalIAT[i].u1.Ordinal)));
			}
			else
			{
				PIMAGE_IMPORT_BY_NAME pByName = (PIMAGE_IMPORT_BY_NAME)((POINTER_TYPE)pImageBase +
					(POINTER_TYPE)(pOriginalIAT[i].u1.AddressOfData));
				lpFunction = GetProcAddress(hDll, (LPCSTR)pByName->Name);
			}
			if (lpFunction != NULL)
			{
				pRealIAT[i].u1.Function = (POINTER_TYPE)lpFunction;
			}
			else
				return FALSE;
		}
		pID++;
	}
	return TRUE;
}






BOOL CheckDataValide(void* lpFileData, int nFileDataLength)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	pDosHeader = (PIMAGE_DOS_HEADER)lpFileData;
	pNTHeader = (PIMAGE_NT_HEADERS)(( char*)lpFileData + pDosHeader->e_lfanew);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((int)pNTHeader + sizeof(IMAGE_NT_HEADERS));

	if (nFileDataLength < sizeof(IMAGE_DOS_HEADER))
		return FALSE;

	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		MessageBoxA(0, "not dos", "not dos", MB_OK);
		return FALSE;
	}
	else {
		
	}

	if ((DWORD)nFileDataLength < (pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS)))
		return FALSE;

	if (pNTHeader == NULL)
		return FALSE;

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) {
		MessageBoxA(0, "not pe", "not pe", MB_OK);
		return FALSE;
	}
	else {
		
	}

	if (!((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0 || (pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) != 0))
		return FALSE;

	if ((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0)
		return FALSE;

	if (pNTHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER))
		return FALSE;

	for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if ((pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData) > (DWORD)nFileDataLength)
			return FALSE;
	}

	return TRUE;
}

int GetAlignedSize(int Origin, int Alignment)
{
	return (Origin + Alignment - 1) / Alignment * Alignment;
}

void CopyPEData(void* pDestImageBase, void* lpFileData)
{
	PIMAGE_DOS_HEADER pDosHeader = NULL;
	PIMAGE_NT_HEADERS pNTHeader = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	pDosHeader = (PIMAGE_DOS_HEADER)(POINTER_TYPE)lpFileData;
	pNTHeader = (PIMAGE_NT_HEADERS)((POINTER_TYPE)lpFileData + (POINTER_TYPE)pDosHeader->e_lfanew);
	pSectionHeader = (PIMAGE_SECTION_HEADER)((POINTER_TYPE)pNTHeader + sizeof(IMAGE_NT_HEADERS));

	int HeaderSize = pNTHeader->OptionalHeader.SizeOfHeaders;

	int SectionSize = pNTHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);

	int TotalSize = HeaderSize + SectionSize;

	memcpy(pDestImageBase, lpFileData, TotalSize);

	for (int i = 0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
	{
		if (pSectionHeader[i].VirtualAddress == 0 || pSectionHeader[i].SizeOfRawData == 0)
			continue;
		void* pSectionAddress = (void*)((POINTER_TYPE)pDestImageBase + pSectionHeader[i].VirtualAddress);

		memcpy((void*)pSectionAddress, (void*)((POINTER_TYPE)lpFileData + pSectionHeader[i].PointerToRawData), pSectionHeader[i].SizeOfRawData);
	}
	return;
}


BOOL MemRunPE(void* lpFileData, int nDataLength)
{

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)(POINTER_TYPE)lpFileData;
	PIMAGE_NT_HEADERS pNTHeader = (PIMAGE_NT_HEADERS)((POINTER_TYPE)lpFileData + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	char  info[1024];
	wsprintfA(info, "data:%x,size:%d", lpFileData, nDataLength);
	MessageBoxA(0, info, info, MB_OK);

	if (!CheckDataValide(lpFileData, nDataLength)) {
		//return FALSE;
	}

	int imageSize = pNTHeader->OptionalHeader.SizeOfImage;
	if (imageSize == 0) {
		
		return FALSE;
	}

	g_pMemoryAddress = VirtualAlloc((LPVOID)NULL, imageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (g_pMemoryAddress == NULL)
	{
		return FALSE;
	}
	else
	{
		CopyPEData(g_pMemoryAddress, lpFileData);

		if (!FillRavAddress(g_pMemoryAddress))
		{
			MessageBoxA(0, "FillRavAddress", "FillRavAddress", MB_OK);
			VirtualFree(g_pMemoryAddress, 0, MEM_RELEASE);
			return FALSE;
		}

		if (pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress > 0 && 
			pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size > 0)
		{
			DoRelocation(g_pMemoryAddress);
		}

		unsigned long old = 0;
		VirtualProtect(g_pMemoryAddress, imageSize, PAGE_EXECUTE_READWRITE, &old);
	}

	pExeMain = (ProcExeMain)(pNTHeader->OptionalHeader.AddressOfEntryPoint + (char*)g_pMemoryAddress);
	pExeMain(GetModuleHandleA(0), GetModuleHandleA(0), 0,0);

	g_bIsLoadOk = TRUE;
	return TRUE;
}
