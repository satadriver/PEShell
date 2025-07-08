#pragma once


#include <windows.h>

#define QWORD unsigned long long

#define EXE_MODULE_ALIGNBLOCK		0x10000

#define DEFAULT_PE_BASE_ADDRESS		0x400000

class LoadPE {
public:
	static int __stdcall LoadPE::load(const char *szFileName);

	static int LoadPE::CallExeEntry(char* chBaseAddress);

	static int LoadPE::CallDllEntry(char* chBaseAddress);

	static int LoadPE::CallConsoleEntry(char* chBaseAddress);

	static int LoadPE::SetImageBase(char* chBaseAddress);

	static int LoadPE::ImportTable(char* chBaseAddress);

	static int LoadPE::RelocationTable(char* chBaseAddress);

	static QWORD LoadPE::GetSizeOfImage(char* pFileBuff);

	static ULONGLONG LoadPE::GetImageBase(char* pFileBuff);

	static int LoadPE::MapFile(char* pFileBuff, char* chBaseAddress);

	static int LoadPE::RunPE(char* pFileBuff, DWORD dwSize);
};
