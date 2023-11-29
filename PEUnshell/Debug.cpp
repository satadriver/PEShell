
#include "Debug.h"
#include <windows.h>
#include "utils.h"

bool Debug::isDebugged()
{

#ifdef _DEBUG
		return FALSE;
#endif

#ifndef _WIN64
		int result = 0;
		__asm
		{
			mov eax, fs: [30h]
			// ���ƶѲ��������Ĺ�����ʽ�ı�־λ
			mov eax, [eax + 68h]
			// ����ϵͳ�������Щ��־λ:FLG_HEAP_ENABLE_TAIL_CHECK, FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS
			// ������x70
			and eax, 0x70
			mov result, eax
		}

		return result != 0;
#else
		return IsDebuggerPresent();
#endif

}



int __stdcall attachSelf(VOID *param) {
	DWORD pid = GetCurrentProcessId();
	DebugActiveProcess((DWORD)pid);
	int e = TRUE;
	while (e)
	{
		DEBUG_EVENT MyDebugInfo;
		e = WaitForDebugEvent(&MyDebugInfo, INFINITE);
		switch (MyDebugInfo.dwDebugEventCode)
		{
		case EXIT_PROCESS_DEBUG_EVENT:
		{
			e = FALSE;
			break;
		}
		}
		if (e) {
			ContinueDebugEvent(MyDebugInfo.dwProcessId, MyDebugInfo.dwThreadId, DBG_CONTINUE);
		}
	}
	return 0;
}

int __stdcall Debug::attach() {
	HANDLE ht = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)attachSelf, 0, 0, 0);
	if (ht)
	{
		CloseHandle(ht);
	}
	return TRUE;
}








