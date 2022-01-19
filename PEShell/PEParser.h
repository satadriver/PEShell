#pragma once
#include <windows.h>

class PEParser {
public:
	static int isPE(const char * data);

	static int getMachine(const char * pedata);
	static int getNumberOfSections(const char * pedata);
	static int getTimeDateStamp(const char * pedata);
	static int getPointerToSymbolTable(const char * pedata);
	static int getNumberOfSymbols(const char * pedata);
	static int getSizeOfOptionalHeader(const char * pedata);
	static int getCharacteristics(const char * pedata);


	static PIMAGE_OPTIONAL_HEADER getOptionalHeader(const char * pedata);
	static PIMAGE_FILE_HEADER getNtHeader(const char * pedata);

	static int getBaseApi(HMODULE & kernel32, DWORD & getprocaddr, DWORD & loadlib);
	static HMODULE PEParser::GetDllKernel32Base();

	static DWORD PEParser::checksumPE(unsigned char * data, int datasize);

	static FARPROC PEParser::getProcAddr(HMODULE pmodule, const char * funname, int funnamelen);
};