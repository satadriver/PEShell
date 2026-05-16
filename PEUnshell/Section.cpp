
#include "Section.h"
#include "FileHelper.h"
#include "compress.h"
#include "crypto.h"
#include "LoadPE.h"
#include <iostream>

using namespace std;




int Section::unshellSection(char* module, const char* secname) {
	int ret = 0;

	int seccnt = 0;
	PIMAGE_SECTION_HEADER sections = 0;
	int bits = GetPeBits(module);
	if (bits == 32) {
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
		int segoffset = nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt->Signature);
		sections = (PIMAGE_SECTION_HEADER)((char*)dos + dos->e_lfanew + segoffset);
		seccnt = nt->FileHeader.NumberOfSections;
	}
	else if (bits == 64) {
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
		PIMAGE_NT_HEADERS64 nt = (PIMAGE_NT_HEADERS64)((char*)dos + dos->e_lfanew);
		int segoffset = nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt->Signature);
		sections = (PIMAGE_SECTION_HEADER)((char*)dos + dos->e_lfanew + segoffset);
		seccnt = nt->FileHeader.NumberOfSections;
	}
	else {
		return 0;
	}

	for (int i = seccnt; i >= 0; i--)
	{
		if (lstrcmpiA((char*)sections[i].Name, secname) == 0)
		{
			char* data = sections[i].VirtualAddress + module;
			DWORD size = sections[i].Misc.VirtualSize;

			DWORD type = *(DWORD*)data;

			if (type == MEM_RUN_EXE)
			{
				ret = Crypto::reloadPE((char*)data, size);
			}
			else if (type == MEM_RUN_DLL)
			{
				ret = Crypto::reloadPE((char*)data, size);
			}
			else if (type == BIND_RELEASE_EXE)
			{
				ret = Crypto::releaseFiles((char*)data, size);
			}
			else if (type == BIND_RELEASE_DLL)
			{
				ret = Crypto::releaseFiles((char*)data, size);
			}

			return TRUE;
		}
	}
	return 0;
}