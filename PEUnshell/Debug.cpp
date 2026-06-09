
#include "Debug.h"
#include <windows.h>
#include "utils.h"
#include "api.h"
#include <intrin.h>
#include <stdio.h>


bool Debug::isDebugged()
{
	int result = 0;
#ifdef _DEBUG
		return FALSE;
#endif

#ifndef _WIN64
		//return IsDebuggerPresent();

		char* pPeb = (char*)__readfsdword(0x30); // 获取PEB地址
		// 检查 Heap Flags 和 ForceFlags
		// 典型调试标志组合: (FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS) = 0x70
		// ForceFlags 正常为 0, 被调试时常为 0x40000060
		if (*(PDWORD)((PBYTE)pPeb + 0x68) & 0x70) { // 检查 Heap->Flags
			return TRUE;
		}
		if (*(PDWORD)((PBYTE)pPeb + 0x6C) & 0x70) { // 检查 Heap->ForceFlags
			return TRUE;
		}
		return FALSE;
#else
		BOOL isDebuggeExist = false;
		result = lpCheckRemoteDebuggerPresent(lpGetCurrentProcess(), &isDebuggeExist);
		return lpIsDebuggerPresent()| isDebuggeExist;
#endif

}



int __stdcall attachSelf(VOID *param) {
	int ret = 0;
    ElevationPrivilege();

	DWORD pid = lpGetCurrentProcessId();
#ifdef _DEBUG
	return 0;
#endif
	ret = lpDebugActiveProcess((DWORD)pid);
	if (ret == 0) {
		runLog("lpDebugActiveProcess \r\n");
		suicide();
	}

	while (1)
	{
		DEBUG_EVENT MyDebugInfo;
		ret = lpWaitForDebugEvent(&MyDebugInfo, INFINITE);
		switch (MyDebugInfo.dwDebugEventCode)
		{
			case EXIT_PROCESS_DEBUG_EVENT:
			{
				ret = FALSE;
				break;
			}
		}

		ret = lpContinueDebugEvent(MyDebugInfo.dwProcessId, MyDebugInfo.dwThreadId, DBG_CONTINUE);	
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




BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnable = TRUE)
{

    BOOL bRet = FALSE;
    HANDLE hToken = NULL;
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ::GetCurrentProcessId());
    if (!::OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        goto __EXIT;
    }
    LUID Luid;
    if (!::LookupPrivilegeValue(NULL, lpszPrivilege, &Luid))
    {
        goto __EXIT;
    }
    TOKEN_PRIVILEGES newPrivilege;
    newPrivilege.PrivilegeCount = 1;
    newPrivilege.Privileges[0].Luid = Luid;
    newPrivilege.Privileges[0].Attributes = //设置特权属性
        bEnable ?
        SE_PRIVILEGE_ENABLED :
        SE_PRIVILEGE_ENABLED_BY_DEFAULT;
    if (!::AdjustTokenPrivileges(hToken, FALSE, &newPrivilege,
        sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        TCHAR s[64] = { 0 };

        goto __EXIT;
    }
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)//查看是否真的设置成功了
    {
        OutputDebugStringA("The token does not have the specified privilege. \n");
        goto __EXIT;
    }
    bRet = TRUE;

__EXIT:
    if (hProcess)
    {
        ::CloseHandle(hProcess);
    }
    if (hToken)
    {
        ::CloseHandle(hToken);
    }
    return bRet;
}

VOID ElevationPrivilege()
{

    SetPrivilege(SE_DEBUG_NAME);
    /*
    SetPrivilege(SE_CREATE_TOKEN_NAME);
    SetPrivilege(SE_ASSIGNPRIMARYTOKEN_NAME);
    SetPrivilege(SE_LOCK_MEMORY_NAME);
    SetPrivilege(SE_INCREASE_QUOTA_NAME);
    SetPrivilege(SE_UNSOLICITED_INPUT_NAME);
    SetPrivilege(SE_MACHINE_ACCOUNT_NAME);
    SetPrivilege(SE_TCB_NAME);
    SetPrivilege(SE_SECURITY_NAME);
    SetPrivilege(SE_TAKE_OWNERSHIP_NAME);
    SetPrivilege(SE_LOAD_DRIVER_NAME);
    SetPrivilege(SE_SYSTEM_PROFILE_NAME);
    SetPrivilege(SE_SYSTEMTIME_NAME);
    SetPrivilege(SE_PROF_SINGLE_PROCESS_NAME);
    SetPrivilege(SE_INC_BASE_PRIORITY_NAME);
    SetPrivilege(SE_CREATE_PAGEFILE_NAME);
    SetPrivilege(SE_CREATE_PERMANENT_NAME);
    SetPrivilege(SE_BACKUP_NAME);
    SetPrivilege(SE_RESTORE_NAME);
    SetPrivilege(SE_SHUTDOWN_NAME);
    SetPrivilege(SE_AUDIT_NAME);
    SetPrivilege(SE_SYSTEM_ENVIRONMENT_NAME);
    SetPrivilege(SE_CHANGE_NOTIFY_NAME);
    SetPrivilege(SE_REMOTE_SHUTDOWN_NAME);
    SetPrivilege(SE_UNDOCK_NAME);
    SetPrivilege(SE_SYNC_AGENT_NAME);
    SetPrivilege(SE_ENABLE_DELEGATION_NAME);
    SetPrivilege(SE_MANAGE_VOLUME_NAME);
    SetPrivilege(SE_IMPERSONATE_NAME);
    SetPrivilege(SE_CREATE_GLOBAL_NAME);
    SetPrivilege(SE_TRUSTED_CREDMAN_ACCESS_NAME);
    SetPrivilege(SE_RELABEL_NAME);
    SetPrivilege(SE_INC_WORKING_SET_NAME);
    SetPrivilege(SE_TIME_ZONE_NAME);
    SetPrivilege(SE_CREATE_SYMBOLIC_LINK_NAME);
    */
}


EXCEPTION_DISPOSITION NTAPI my_EXCEPTION_ROUTINE(EXCEPTION_RECORD* er, PVOID ef, CONTEXT* c, PVOID dc) {

#ifndef _WIN64
	//__debugbreak();
	runLog("%s %d\r\n", __FUNCTION__, __LINE__);
	MY_EXCEPTION_STRUCT* exp = (MY_EXCEPTION_STRUCT*)__readfsdword(0);

	DWORD regebp = exp->ebp;
	DWORD regesp = exp->esp;
	DWORD regebx = exp->ebx;
	DWORD regesi = exp->esi;
	DWORD regedi = exp->edi;

	c->Eip = (DWORD)exp->retaddr;
	c->Esi = exp->esi;
	c->Edi = exp->edi;
	c->Esp = exp->esp;
	c->Ebp = exp->ebp;
	c->Ebx = exp->ebx;

	__writefsdword(0, (DWORD)exp->exp.Next);

	delete exp;

	char buf[1024];
	sprintf(buf, "exception tag:%s,address:%p,code:%x\r\n", exp->tag, er->ExceptionAddress, er->ExceptionCode);
	//MessageBoxA(0, buf, buf, 0);
	runLog(buf);
#endif

	return ExceptionContinueExecution;
}



int Try(char* tag, char* retaddr) {
#ifndef _WIN64
	MY_EXCEPTION_STRUCT* exp = new MY_EXCEPTION_STRUCT;
	exp->exp.Next = (_EXCEPTION_REGISTRATION_RECORD*)__readfsdword(0);
	exp->exp.Handler = my_EXCEPTION_ROUTINE;

	DWORD regesp = 0;
	DWORD regebp = 0;
	DWORD regebx = 0;
	DWORD regesi = 0;
	DWORD regedi = 0;

	__asm {
		mov eax, [ebp]
		mov[regebp], eax

		mov eax, ebp
		add eax, 16
		mov[regesp], eax

		mov[regebx], ebx
		mov[regesi], esi
		mov[regedi], edi
	}

	exp->ebp = regebp;
	exp->esp = regesp;
	exp->ebx = regebx;
	exp->esi = regesi;
	exp->edi = regedi;
	exp->retaddr = retaddr;

	__writefsdword(0, (DWORD)exp);

	MY_EXCEPTION_STRUCT* nv = (MY_EXCEPTION_STRUCT*)__readfsdword(0);
	HANDLE h = GetModuleHandleA(0);
	runLog("%s %d new handler:%x,old handler:%x,base:%x\r\n", __FUNCTION__, __LINE__, nv->exp.Handler,exp->exp.Next->Handler,h);
#endif
	return 0;
}