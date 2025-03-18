
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
			// ���ƶѲ��������Ĺ�����ʽ�ı�־λ
			mov eax, [eax + 68h]
			// ����ϵͳ�������Щ��־λ:FLG_HEAP_ENABLE_TAIL_CHECK, FLG_HEAP_ENABLE_FREE_CHECK and FLG_HEAP_VALIDATE_PARAMETERS
			// ������x70
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








//�����ȡ��Ȩ
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
    newPrivilege.Privileges[0].Attributes = //������Ȩ����
        bEnable ?
        SE_PRIVILEGE_ENABLED :
        SE_PRIVILEGE_ENABLED_BY_DEFAULT;
    if (!::AdjustTokenPrivileges(hToken, FALSE, &newPrivilege,
        sizeof(TOKEN_PRIVILEGES), NULL, NULL))
    {
        TCHAR s[64] = { 0 };

        goto __EXIT;
    }
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)//�鿴�Ƿ�������óɹ���
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
//�������е���Ȩ���û�Ȩ���´󲿷ֶ��ǻ�ȡ������
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