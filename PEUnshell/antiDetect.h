#pragma once

#include <windows.h>

class AntiDetect {

public:
	AntiDetect();
	~AntiDetect();
	

	static int checkHook();

};


int ExceptionDetect();

LONG __stdcall expHandler(_EXCEPTION_POINTERS* ExceptionInfo);

int DivideZero();