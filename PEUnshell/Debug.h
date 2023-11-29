#pragma once

#include <windows.h>

int __stdcall attachSelf(VOID* param);

class Debug {
public:
	static bool Debug::isDebugged();

	static int __stdcall attach();
};