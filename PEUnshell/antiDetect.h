#pragma once

#include <windows.h>

class AntiDetect {

public:
	AntiDetect();
	~AntiDetect();
	

	static int checkHook();

};




LONG __stdcall expHandler(_EXCEPTION_POINTERS* ExceptionInfo);

int exceptTest();