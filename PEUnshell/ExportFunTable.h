#pragma once

#include <windows.h>

class ExportFunTable {
public:
	static DWORD getAddrFromName(DWORD module,const char * funname);
	static DWORD getAddrFromOrd(DWORD module,DWORD ord);
};