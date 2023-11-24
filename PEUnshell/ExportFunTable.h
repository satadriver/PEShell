#pragma once

#include <windows.h>

class ExportFunTable {
public:
	static char* getAddrFromName(char* module,const char * funname);
	static char* getAddrFromOrd(char* module,DWORD ord);
};