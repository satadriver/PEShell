
#include <windows.h>
#include "pe64.h"
#include <stdio.h>


#define __strcmp strcmp

// 重定位类型定义（64位PE主要使用 IMAGE_REL_BASED_DIR64）
#define IMAGE_REL_BASED_ABSOLUTE      0  // 无操作
#define IMAGE_REL_BASED_HIGHLOW       3  // 32位PE使用
#define IMAGE_REL_BASED_DIR64         10 // 64位PE使用

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



// 函数：应用PE文件的重定位表
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

	// 检查是否为64位PE文件
	if (pNtHeaders->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) {
		//printf("Not a 64-bit PE file.\n");
		return false;
	}

	// 获取重定位表目录项
	IMAGE_DATA_DIRECTORY relocDir = pNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	if (relocDir.VirtualAddress == 0 || relocDir.Size == 0) {
		//printf("No relocation table found (likely a non-relocatable executable).\n");
		return true; // 无重定位表不代表失败（如主模块固定基址）
	}

	// 计算重定位表的起始位置
	PIMAGE_BASE_RELOCATION pReloc = (PIMAGE_BASE_RELOCATION)((BYTE*)pImageBase + relocDir.VirtualAddress);
	ULONGLONG delta = newBase - pNtHeaders->OptionalHeader.ImageBase;

	// 遍历所有重定位块
	while (pReloc->VirtualAddress != 0 && pReloc->SizeOfBlock > 0) {
		// 计算当前块的重定位项数量
		DWORD numEntries = (pReloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);
		PWORD pEntries = (PWORD)(pReloc + 1);

		// 处理每个重定位项
		for (DWORD i = 0; i < numEntries; i++) {
			WORD entry = pEntries[i];
			BYTE type = entry >> 12;      // 高4位是类型
			WORD offset = entry & 0xFFF;  // 低12位是偏移

			// 仅处理64位重定位项
			if (type == IMAGE_REL_BASED_DIR64) {
				ULONGLONG* pAddr = (ULONGLONG*)((BYTE*)pImageBase + pReloc->VirtualAddress + offset);
				*pAddr += delta; // 修正地址
			}
			else if (type != IMAGE_REL_BASED_ABSOLUTE) {
				//printf("Unsupported relocation type: %d\n", type);
			}
		}

		// 移动到下一个重定位块
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

//why need to modify imagebase？
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

	// 获取导入表
	pImportDir = &pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	if (!pImportDir->VirtualAddress || !pImportDir->Size)
		return TRUE; // 没有导入表也是可以的

	// 获取第一个导入描述符
	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)pFileBuff + pImportDir->VirtualAddress);

	// 遍历所有导入描述符
	while (pImportDesc->Name) {
		// 获取DLL名称
		strcpy_s(szDllName, sizeof(szDllName), (LPCSTR)((ULONG_PTR)pFileBuff + pImportDesc->Name));

		// 加载DLL
		hModule = LoadLibraryA(szDllName);
		if (!hModule)
			return FALSE;

		// 获取原始Thunk和Thunk
		pOrigThunk = (PIMAGE_THUNK_DATA64)((ULONG_PTR)pFileBuff + pImportDesc->OriginalFirstThunk);
		pThunk = (PIMAGE_THUNK_DATA64)((ULONG_PTR)pFileBuff + pImportDesc->FirstThunk);

		// 遍历所有导入函数
		while (pOrigThunk->u1.AddressOfData) {
			// 检查是按名称导入还是按序号导入
			if (pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG64) {
				// 按序号导入
				pFuncName = (LPCSTR)(pOrigThunk->u1.Ordinal & 0xFFFF);
				pFunc = GetProcAddress(hModule, pFuncName);
			}
			else {
				// 按名称导入
				pImportByName = (PIMAGE_IMPORT_BY_NAME)((ULONG_PTR)pFileBuff + pOrigThunk->u1.AddressOfData);
				pFuncName = (LPCSTR)pImportByName->Name;
				pFunc = GetProcAddress(hModule, pFuncName);
			}

			if (!pFunc)
				return FALSE;

			// 填充IAT
			pIatEntry = (DWORD_PTR*)&pThunk->u1.Function;
			*pIatEntry = (DWORD_PTR)pFunc;

			// 移动到下一个Thunk
			pOrigThunk++;
			pThunk++;
		}

		// 移动到下一个导入描述符
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