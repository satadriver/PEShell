
#include "Section.h"
#include "FileHelper.h"
#include "compress.h"
#include "crypto.h"

#include <iostream>

using namespace std;

#define MEM_RUN_EXE				1
#define MEM_RUN_DLL				2
#define BIND_RELEASE_EXE		3
#define BIND_RELEASE_DLL		4


int Section::unshellSection(char* module, const char* secname) {
	int ret = 0;

	char szinfo[1024];

	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)dos + dos->e_lfanew);
	int segoffset = nt->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + sizeof(nt->Signature);
	PIMAGE_SECTION_HEADER sections = (PIMAGE_SECTION_HEADER)((char*)dos + dos->e_lfanew + segoffset);
	int secscnt = nt->FileHeader.NumberOfSections;

	for (int i = secscnt; i >= 0; i--)
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