
#include "RVA.h"


DWORD RVA::rvaInFile(DWORD module,DWORD rva) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((DWORD)dos + dos->e_lfanew);
	int optsize = nt->FileHeader.SizeOfOptionalHeader;
	PIMAGE_SECTION_HEADER sections = (PIMAGE_SECTION_HEADER)((DWORD)module +
		dos->e_lfanew + sizeof(nt->Signature) + sizeof(IMAGE_FILE_HEADER) + optsize);

	int seccnt = nt->FileHeader.NumberOfSections;
	for (int i = 0;i < seccnt; i ++)
	{
		DWORD end = sections[i].Misc.VirtualSize + sections[i].VirtualAddress;
		DWORD start = sections[i].VirtualAddress;
		if (rva >= start && rva <= end)
		{
			DWORD offset = rva - start;
			DWORD fileoffset = sections[i].PointerToRawData + offset;
			return fileoffset;
		}
	}

	return -1;
}


const unsigned char * RVA::getRvaSectionName(DWORD module, DWORD rva) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((DWORD)dos + dos->e_lfanew);
	int optsize = nt->FileHeader.SizeOfOptionalHeader;
	PIMAGE_SECTION_HEADER sections = (PIMAGE_SECTION_HEADER)((DWORD)module +
		dos->e_lfanew + sizeof(nt->Signature) + sizeof(IMAGE_FILE_HEADER) + optsize);

	int seccnt = nt->FileHeader.NumberOfSections;
	for (int i = 0; i < seccnt; i++)
	{
		DWORD end = sections[i].Misc.VirtualSize + sections[i].VirtualAddress;
		DWORD start = sections[i].VirtualAddress;
		if (rva >= start && rva <= end)
		{
			return sections[i].Name;
		}
	}

	return 0;
}