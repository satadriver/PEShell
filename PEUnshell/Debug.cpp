
#include "Debug.h"
#include <windows.h>


bool Debug::PEB_BegingDebugged()
{
#ifndef _WIN64
	bool BegingDebugged = false;
	__asm
	{
		mov eax, fs:[0x30];					//��ȡPEB
		mov al, byte ptr ds: [eax + 0x2]	//��ȡPeb.BegingDebugged
		mov BegingDebugged, al
	}
	return BegingDebugged;					//���Ϊ1��˵�����ڱ�����
#else
	return IsDebuggerPresent();
#endif
}
