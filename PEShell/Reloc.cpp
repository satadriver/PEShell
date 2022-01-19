#include "Reloc.h"

int Reloc::recovery(DWORD module) {
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)module;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)(module + dos->e_lfanew);
	if (nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress == 0 ||
		nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size == 0)
	{
		return 0;
	}

	PIMAGE_BASE_RELOCATION reloc = (PIMAGE_BASE_RELOCATION)(module +
		nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	while (reloc->VirtualAddress && reloc->SizeOfBlock ) 
	{
		WORD * relocdate = (WORD *)((PBYTE)reloc + sizeof(IMAGE_BASE_RELOCATION));
		int reloccnt = (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD);

		for (int i = 0; i < reloccnt; i++)
		{
			if ( (relocdate[i] & 0x0000F000) == 0x00003000)
			{
				DWORD * address = (DWORD *)(module + reloc->VirtualAddress + (relocdate[i] & 0x0FFF));
				DWORD offset = module - nt->OptionalHeader.ImageBase;
				*address = *address + offset;
			}
		}

		reloc = (PIMAGE_BASE_RELOCATION)((PBYTE)reloc + reloc->SizeOfBlock);
	}

	return TRUE;
}