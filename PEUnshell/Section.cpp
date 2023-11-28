
#include "Section.h"
#include "FileHelper.h"
#include "compress.h"
#include "crypto.h"

#include <iostream>

using namespace std;

#define ONLY_ONE_EXE		1
#define ONLY_ONE_DLL		2
#define BIND_MODE_EXE		3
#define BIND_MODE_DLL		4


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

			if (type == ONLY_ONE_EXE)
			{
				ret = Crypto::reloadPE((char*)data, size);
			}
			else if (type == ONLY_ONE_DLL)
			{
				ret = Crypto::reloadPE((char*)data, size);
			}
			else if (type == BIND_MODE_EXE)
			{
				ret = Crypto::getoutFiles((char*)data, size);
			}
			else if (type == BIND_MODE_DLL)
			{
				ret = Crypto::getoutFiles((char*)data, size);
			}

			return TRUE;
		}
	}
	return 0;
}