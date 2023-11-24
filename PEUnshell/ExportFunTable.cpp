#include "ExportFunTable.h"


char* ExportFunTable::getAddrFromName(char* module, const char * funname) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
	DWORD rva = nt->OptionalHeader.DataDirectory[0].VirtualAddress;
	//DWORD size = nt->OptionalHeader.DataDirectory[0].Size;

	PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)(rva + module);

	// const char * name = (const char*)(exp->Name + module);

	WORD * ords = (WORD*)(exp->AddressOfNameOrdinals + module);

	const char ** funs = (const char **)(exp->AddressOfNames + module);
	for (int i = 0 ; i < exp->NumberOfNames; i ++)
	{
		const char * functionname = (funs[i] + (ULONGLONG)module);
		if (lstrcmpiA(funname, functionname)== 0)
		{
			int idx = ords[i];
			DWORD * addrs = (DWORD *)(exp->AddressOfFunctions + module);
			char* addr = addrs[idx] + module;
			return addr;
		}
	}

	return 0;
}

char* ExportFunTable::getAddrFromOrd(char* module, DWORD ord) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
	DWORD rva = nt->OptionalHeader.DataDirectory[0].VirtualAddress;
	DWORD size = nt->OptionalHeader.DataDirectory[0].Size;

	PIMAGE_EXPORT_DIRECTORY exp = (PIMAGE_EXPORT_DIRECTORY)(rva + module);

	int funidx = ord - exp->Base;
	if (funidx < 0 || funidx >= exp->NumberOfFunctions)
	{
		return 0;
	}

	DWORD * addrs = (DWORD *)(exp->AddressOfFunctions + module);
	char* addr = addrs[funidx] + module;
	return addr;
}