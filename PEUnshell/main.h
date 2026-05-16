#pragma once

#include <windows.h>

typedef int(__stdcall *ptrDllMainEntry)(char*, DWORD, DWORD);
typedef int (__stdcall *ptrWinMain)(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd);
typedef int (*ptrmain)(int argc, char ** argv);

extern char * ghThisHandle;
extern HINSTANCE ghPrevInstance;
extern LPSTR glpCmdLine;
extern int gnShowCmd;



