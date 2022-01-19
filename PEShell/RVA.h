#pragma once


#include <windows.h>

//rva is offset from dos header

class RVA {
public:
	static DWORD rvaInFile(DWORD module,DWORD rva);
	static const unsigned char * getRvaSectionName(DWORD module, DWORD rva);
};