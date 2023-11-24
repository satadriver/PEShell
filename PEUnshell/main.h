#pragma once

#include <windows.h>

typedef int(__stdcall *ptrDllMainEntry)(char*, DWORD, DWORD);
typedef int (__stdcall *ptrWinMain)(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd);
typedef int (*ptrmain)(int argc, char ** argv);

extern int gType;
extern HMODULE ghPEModule;
extern int gPEImageSize;

extern char * ghThisHandle;
extern HINSTANCE ghprevInstance;
extern LPSTR glpCmdLine;
extern int gnShowCmd;

extern ptrmain glpmain;
extern ptrDllMainEntry glpDllMainEntry;
extern ptrWinMain glpWinMain;

extern DWORD gRegEax;
extern DWORD gRegEcx;
extern DWORD gRegEdx;
extern DWORD gRegEbx;
extern DWORD gRegEsp;
extern DWORD gRegEbp;
extern DWORD gRegEsi;
extern DWORD gRegEdi;