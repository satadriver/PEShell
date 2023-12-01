
#include "Debug.h"
#include <windows.h>
#include "utils.h"
#include "api.h"




bool Debug::isDebugged()
{
	int result = 0;
#ifdef _DEBUG
		return FALSE;
#endif

#ifndef _WIN64
		
		__asm
		{
			mov eax, fs: [30h]
			// 控制堆操作函数的工作方式的标志位
			mov eax, [eax + 68h]
			// 操作系统会加上这些标志位:FLG_HEAP_ENABLE_TAIL_CHECK, FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS
			// 并集是x70
			and eax, 0x70
			mov result, eax
		}

		return result != 0;
#else
		BOOL isDebuggeExist = false;
		result = lpCheckRemoteDebuggerPresent(lpGetCurrentProcess(), &isDebuggeExist);
		return lpIsDebuggerPresent()| isDebuggeExist;
#endif

}



int __stdcall attachSelf(VOID *param) {
	DWORD pid = lpGetCurrentProcessId();
	lpDebugActiveProcess((DWORD)pid);
	int e = TRUE;
	while (e)
	{
		DEBUG_EVENT MyDebugInfo;
		e = lpWaitForDebugEvent(&MyDebugInfo, INFINITE);
		switch (MyDebugInfo.dwDebugEventCode)
		{
		case EXIT_PROCESS_DEBUG_EVENT:
		{
			e = FALSE;
			break;
		}
		}
		if (e) {
			lpContinueDebugEvent(MyDebugInfo.dwProcessId, MyDebugInfo.dwThreadId, DBG_CONTINUE);
		}
	}
	return 0;
}

int __stdcall Debug::attach() {
	HANDLE ht = lpCreateThread(0, 0, (LPTHREAD_START_ROUTINE)attachSelf, 0, 0, 0);
	if (ht)
	{
		lpCloseHandle(ht);
	}
	return TRUE;
}








