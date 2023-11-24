
#include "Debug.h"
#include <windows.h>


bool Debug::PEB_BegingDebugged()
{
#ifndef _WIN64
	bool BegingDebugged = false;
	__asm
	{
		mov eax, fs:[0x30];					//获取PEB
		mov al, byte ptr ds: [eax + 0x2]	//获取Peb.BegingDebugged
		mov BegingDebugged, al
	}
	return BegingDebugged;					//如果为1则说明正在被调试
#else
	return IsDebuggerPresent();
#endif
}
