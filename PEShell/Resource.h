#pragma once

#include <windows.h>

class Resource {
public:
	static int getResource(DWORD module,int id);
	static int getResource(DWORD module,const char *name);
};