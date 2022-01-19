#pragma once

#include <windows.h>

class Section {
public:
	static int unshellSection(DWORD module,const char * secname);

};