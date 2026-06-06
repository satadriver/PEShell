#pragma once

#include <windows.h>

int __stdcall attachSelf(VOID* param);

VOID ElevationPrivilege();

int Try(char* tag, char* retaddr);
EXCEPTION_DISPOSITION NTAPI my_EXCEPTION_ROUTINE(EXCEPTION_RECORD* er, PVOID ef, CONTEXT* c, PVOID dc);

#pragma pack(1)



typedef struct {
	EXCEPTION_REGISTRATION_RECORD exp;
	DWORD esp;
	DWORD ebp;
	DWORD ebx;
	DWORD esi;
	DWORD edi;
	char* tag;
	char* retaddr;
}MY_EXCEPTION_STRUCT;

#pragma pack()

class Debug {
public:
	static bool Debug::isDebugged();

	static int __stdcall attach();
};