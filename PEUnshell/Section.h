#pragma once

#include <windows.h>


#define MEM_RUN_EXE				1
#define MEM_RUN_DLL				2
#define BIND_RELEASE_EXE		3
#define BIND_RELEASE_DLL		4

class Section {
public:
	static int unshellSection(char* module,const char * secname);

};