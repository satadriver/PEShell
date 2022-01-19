
#include "ImportFunTable.h"
#include "ExportFunTable.h"

HMODULE hThisModule = 0;

HMODULE lpGetModuleHandle(const char * name) {
	if (name == 0)
	{
		return (HMODULE)hThisModule;
	}
	else {
		return (HMODULE)GetModuleHandleA(name);
	}
}

//每个dll有一个IMAGE_IMPORT_DESCRIPTOR
//2个IMAGE_THUNK_DATA
//IMAGE_THUNK_DATA指向IMAGE_IMPORT_BY_NAME
//导入地址数组是IAT
//IMAGE_DIRECTORY_ENTRY_IAT = IAT
DWORD ImportFunTable::recover(DWORD module) {

	char szGetModuleHandleA[] = { 'G','e','t','M','o','d','u','l','e','H','a','n','d','l','e','A',0 };
	char szInitializeSListHead[] = { 'I','n','i','t','i','a','l','i','z','e','S','L','i','s','t','H','e','a','d',0 };
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((DWORD)dos + dos->e_lfanew);
	DWORD rva = nt->OptionalHeader.DataDirectory[1].VirtualAddress;
	DWORD size = nt->OptionalHeader.DataDirectory[1].Size;

	PIMAGE_IMPORT_DESCRIPTOR impd = (PIMAGE_IMPORT_DESCRIPTOR)(rva + module);
	
	while (1)
	{
		if (impd->FirstThunk == 0 && impd->ForwarderChain == 0 && impd->Name ==0 &&
			impd->OriginalFirstThunk == 0 && impd->TimeDateStamp == 0)
		{
			break;
		}

		const char * dllname = (const char *)(module + impd->Name);
		HMODULE dll = GetModuleHandleA(dllname);
		if (NULL == dll)
		{
			dll = LoadLibraryA(dllname);
			if (NULL == dll)
			{
				impd++;
				continue;
			}
		}

		PIMAGE_THUNK_DATA org = (PIMAGE_THUNK_DATA)(impd->OriginalFirstThunk + module);
		PIMAGE_THUNK_DATA first = (PIMAGE_THUNK_DATA)(impd->FirstThunk + module);
		while (1)
		{
			if (org->u1.Ordinal == 0 || first->u1.Ordinal == 0)
			{
				break;
			}

			DWORD addr = 0;
			if (org->u1.Ordinal & 0x80000000)
			{
				int ord = org->u1.Ordinal & 0xffff;
				addr = ExportFunTable::getAddrFromOrd((DWORD)dll, ord);
			}
			else {

				PIMAGE_IMPORT_BY_NAME impname = (PIMAGE_IMPORT_BY_NAME)(module + org->u1.AddressOfData);
				if (lstrcmpiA((char*)impname->Name, szGetModuleHandleA) == 0 /*|| lstrcmpiW(impname->Name, L"GetModuleHandleW")*/)
				{
					//hThisModule = (HMODULE)module;
					//addr = (DWORD)lpGetModuleHandle;
					addr = (DWORD)GetProcAddress(dll, (LPCSTR)impname->Name);
				}
				else if (lstrcmpiA((char*)impname->Name, szInitializeSListHead) == 0)
				{
					addr = (DWORD)GetProcAddress(dll, (LPCSTR)impname->Name);
				}
				else {
					addr = ExportFunTable::getAddrFromName((DWORD)dll, (char*)impname->Name);
				}
			}

			if (addr <= 0)
			{
				break;
			}
			first->u1.Function = addr;

			org++;
			first++;
		}

		impd++;
	}
	return 0;
}


