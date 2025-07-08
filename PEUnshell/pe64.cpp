
#include <windows.h>
#include "pe64.h"
#include <stdio.h>


#define __strcmp strcmp

// �ض�λ���Ͷ��壨64λPE��Ҫʹ�� IMAGE_REL_BASED_DIR64��
#define IMAGE_REL_BASED_ABSOLUTE      0  // �޲���
#define IMAGE_REL_BASED_HIGHLOW       3  // 32λPEʹ��
#define IMAGE_REL_BASED_DIR64         10 // 64λPEʹ��

char* getAddrFromName64(char* module, const char* funname) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)((QWORD)dos + dos->e_lfanew);
	DWORD exptrva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	//DWORD size = nt->OptionalHeader.DataDirectory[0].Size;

	PIMAGE_EXPORT_DIRECTORY exptable = (PIMAGE_EXPORT_DIRECTORY)(exptrva + module);

	const char** funnames = (const char**)(exptable->AddressOfNames + module);
	for (unsigned int i = 0; i < exptable->NumberOfNames; i++)
	{
		char* functionname = ((char*)funnames[i] + (QWORD)module);
		if (__strcmp((char*)funname, (char*)functionname) == 0)
		{
			WORD* ords = (WORD*)(exptable->AddressOfNameOrdinals + module);
			int idx = ords[i];
			DWORD* addrs = (DWORD*)(exptable->AddressOfFunctions + module);
			char* addr = addrs[idx] + module;
			return addr;
		}
	}


	printf( "getAddrFromName module:%p,name:%s error\n", module, funname);

	return 0;
}

QWORD getAddrFromOrd64(char* module, DWORD ord) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)((QWORD)dos + dos->e_lfanew);
	QWORD rva = nt->OptionalHeader.DataDirectory[0].VirtualAddress;
	QWORD size = nt->OptionalHeader.DataDirectory[0].Size;

	PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)(rva + module);

	unsigned int funidx = ord - exp->Base;
	if (funidx < 0 || funidx >= exp->NumberOfFunctions)
	{
		printf( "getAddrFromOrd module:%p,ord:%d error\n", module, ord);

		return 0;
	}

	DWORD* addrs = (DWORD*)(exp->AddressOfFunctions + module);
	QWORD addr = addrs[funidx] + (QWORD)module;
	return addr;
}



// ������Ӧ��PE�ļ����ض�λ��
bool relocTable64(char* pImageBase, ULONGLONG newBase) {
	//ULONGLONG newBase = (ULONGLONG)pImageBase;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		return false;
	}

	PIMAGE_NT_HEADERS64 pNtHeaders = (PIMAGE_NT_HEADERS64)((BYTE*)pImageBase + pDosHeader->e_lfanew);
	if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
		return false;
	}

	// ����Ƿ�Ϊ64λPE�ļ�
	if (pNtHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) {
		//printf("Not a 64-bit PE file.\n");
		return false;
	}

	// ��ȡ�ض�λ��Ŀ¼��
	IMAGE_DATA_DIRECTORY relocDir = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	if (relocDir.VirtualAddress == 0 || relocDir.Size == 0) {
		//printf("No relocation table found (likely a non-relocatable executable).\n");
		return true; // ���ض�λ������ʧ�ܣ�����ģ��̶���ַ��
	}

	// �����ض�λ�����ʼλ��
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)((BYTE*)pImageBase + relocDir.VirtualAddress);
	ULONGLONG delta = newBase - pNtHeaders->OptionalHeader.ImageBase;

	// ���������ض�λ��
	while (pReloc->VirtualAddress != 0 && pReloc->SizeOfBlock > 0) {
		// ���㵱ǰ����ض�λ������
		DWORD numEntries = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		PWORD pEntries = (PWORD)(pReloc + 1);

		// ����ÿ���ض�λ��
		for (DWORD i = 0; i < numEntries; i++) {
			WORD entry = pEntries[i];
			BYTE type = entry >> 12;      // ��4λ������
			WORD offset = entry & 0xFFF;  // ��12λ��ƫ��

			// ������64λ�ض�λ��
			if (type == IMAGE_REL_BASED_DIR64) {
				ULONGLONG* pAddr = (ULONGLONG*)((BYTE*)pImageBase + pReloc->VirtualAddress + offset);
				*pAddr += delta; // ������ַ
			}
			else if (type != IMAGE_REL_BASED_ABSOLUTE) {
				//printf("Unsupported relocation type: %d\n", type);
			}
		}

		// �ƶ�����һ���ض�λ��
		pReloc = (PIMAGE_BASE_RELOCATION)((BYTE*)pReloc + pReloc->SizeOfBlock);
	}

	//printf("Relocations applied successfully (Delta: 0x%llx).\n", delta);
	return true;
}




bool mapFileAttr64(char* pFileBuff, char* chBaseAddress, DWORD* cr3)
{

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pFileBuff + pDos->e_lfanew);

	int dwSizeOfHeaders = pNt->OptionalHeader.SizeOfHeaders;
	memcpy(chBaseAddress, pFileBuff, dwSizeOfHeaders);

	//PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((char*)pNt + sizeof(IMAGE_NT_HEADERS64));

	int nNumerOfSections = pNt->FileHeader.NumberOfSections;
	for (int i = 0; i < nNumerOfSections; i++, pSection++)
	{
		if ((0 == pSection->VirtualAddress) || (0 == pSection->SizeOfRawData))
		{
			continue;
		}

		char* chDestMem = (char*)((QWORD)chBaseAddress + pSection->VirtualAddress);
		char* chSrcMem = (char*)((QWORD)pFileBuff + pSection->PointerToRawData);
		int dwSizeOfRawData = pSection->SizeOfRawData;
		memcpy(chDestMem, chSrcMem, dwSizeOfRawData);
	}

	return TRUE;
}

bool mapFile64(char* pFileBuff, char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pFileBuff + pDos->e_lfanew);

	DWORD dwSizeOfHeaders = pNt->OptionalHeader.SizeOfHeaders;
	memcpy(chBaseAddress, pFileBuff, dwSizeOfHeaders);

	//PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNt);
	PIMAGE_SECTION_HEADER pSection = (PIMAGE_SECTION_HEADER)((char*)pNt + sizeof(IMAGE_NT_HEADERS64));

	int nNumerOfSections = pNt->FileHeader.NumberOfSections;
	for (int i = 0; i < nNumerOfSections; i++, pSection++)
	{
		if ((0 == pSection->VirtualAddress)&& (0 == pSection->SizeOfRawData))
		{
			continue;
		}

		char* chDestMem = (char*)((QWORD)chBaseAddress + pSection->VirtualAddress);
		char* chSrcMem = (char*)((QWORD)pFileBuff + pSection->PointerToRawData);
		int dwSizeOfRawData = pSection->SizeOfRawData;
		memcpy(chDestMem, chSrcMem, dwSizeOfRawData);
	}

	return TRUE;
}


QWORD getSizeOfImage64(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pFileBuff + pDos->e_lfanew);
	QWORD dwSizeOfImage = pNt->OptionalHeader.SizeOfImage;

	return dwSizeOfImage;
}


QWORD getEntry64(char* pe) {
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pe;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pe + pDos->e_lfanew);
	QWORD entry = pNt->OptionalHeader.AddressOfEntryPoint;

	return entry;
}




QWORD getImageBase64(char* pFileBuff)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pFileBuff + pDos->e_lfanew);
	QWORD imagebase = pNt->OptionalHeader.ImageBase;

	return imagebase;
}

//why need to modify imagebase��
int setImageBase64(char* chBaseAddress)
{
	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)chBaseAddress;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(chBaseAddress + pDos->e_lfanew);
	pNt->OptionalHeader.ImageBase = (QWORD)chBaseAddress;

	return TRUE;
}



int importTable64(char* pFileBuff) {
	PIMAGE_DATA_DIRECTORY pImportDir = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	PIMAGE_THUNK_DATA64 pThunk = NULL;
	PIMAGE_THUNK_DATA64 pOrigThunk = NULL;
	PIMAGE_IMPORT_BY_NAME pImportByName = NULL;
	HMODULE hModule = NULL;
	CHAR szDllName[1024] = { 0 };
	LPCSTR pFuncName = NULL;
	FARPROC pFunc = NULL;
	DWORD_PTR* pIatEntry = NULL;

	PIMAGE_DOS_HEADER pDos = (PIMAGE_DOS_HEADER)pFileBuff;
	PIMAGE_NT_HEADERS64 pNt = (PIMAGE_NT_HEADERS64)(pFileBuff + pDos->e_lfanew);

	// ��ȡ�����
	pImportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (!pImportDir->VirtualAddress || !pImportDir->Size)
		return TRUE; // û�е����Ҳ�ǿ��Ե�

	// ��ȡ��һ������������
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)pFileBuff + pImportDir->VirtualAddress);

	// �������е���������
	while (pImportDesc->Name) {
		// ��ȡDLL����
		strcpy_s(szDllName, sizeof(szDllName), (LPCSTR)((ULONG_PTR)pFileBuff + pImportDesc->Name));

		// ����DLL
		hModule = LoadLibraryA(szDllName);
		if (!hModule)
			return FALSE;

		// ��ȡԭʼThunk��Thunk
		pOrigThunk = (PIMAGE_THUNK_DATA64)((ULONG_PTR)pFileBuff + pImportDesc->OriginalFirstThunk);
		pThunk = (PIMAGE_THUNK_DATA64)((ULONG_PTR)pFileBuff + pImportDesc->FirstThunk);

		// �������е��뺯��
		while (pOrigThunk->u1.AddressOfData) {
			// ����ǰ����Ƶ��뻹�ǰ���ŵ���
			if (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
				// ����ŵ���
				pFuncName = (LPCSTR)(pOrigThunk->u1.Ordinal & 0xFFFF);
				pFunc = GetProcAddress(hModule, pFuncName);
			}
			else {
				// �����Ƶ���
				pImportByName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)pFileBuff + pOrigThunk->u1.AddressOfData);
				pFuncName = (LPCSTR)pImportByName->Name;
				pFunc = GetProcAddress(hModule, pFuncName);
			}

			if (!pFunc)
				return FALSE;

			// ���IAT
			pIatEntry = (DWORD_PTR*)&pThunk->u1.Function;
			*pIatEntry = (DWORD_PTR)pFunc;

			// �ƶ�����һ��Thunk
			pOrigThunk++;
			pThunk++;
		}

		// �ƶ�����һ������������
		pImportDesc++;
	}

	return TRUE;
	
}

QWORD MemLoadDll64(char* filedata, char* addr) {
	int ret = 0;
	mapFile64(filedata, addr);
	importTable64((char*)addr);

	relocTable64(addr, (unsigned long long)addr);
	setImageBase64(addr);
	return (QWORD)addr;
}