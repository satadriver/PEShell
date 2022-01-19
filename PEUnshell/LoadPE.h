#pragma once


#include <windows.h>

#define EXE_MODULE_ALIGNBLOCK		0x10000

#define DEFAULT_PE_BASE_ADDRESS		0x400000

class LoadPE {
public:
	static int __stdcall LoadPE::load(const char *szFileName);

	static int LoadPE::CallExeEntry(char* chBaseAddress);

	static int LoadPE::CallDllEntry(char* chBaseAddress);

	static int LoadPE::CallConsoleEntry(char* chBaseAddress);

	static bool LoadPE::SetImageBase(char* chBaseAddress);

	static bool LoadPE::ImportTable(char* chBaseAddress);

	static bool LoadPE::RelocationTable(char* chBaseAddress);

	static DWORD LoadPE::GetSizeOfImage(char* pFileBuff);

	static DWORD LoadPE::GetImageBase(char* pFileBuff);

	static bool LoadPE::MapFile(char* pFileBuff, char* chBaseAddress);

	static int LoadPE::RunPE(char* pFileBuff, DWORD dwSize);
};
