#pragma once

#include <windows.h>

class ExportFunTable {
public:
	static unsigned long long getAddrFromName(char* module,const char * funname);
	static unsigned long long getAddrFromOrd(char* module,DWORD ord);
};