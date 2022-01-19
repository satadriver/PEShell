
#include <stdio.h>
#include <windows.h>
#include "ExportFunTable.h"

typedef int(*ptrmytest)();

int main(int argc, char ** argv) {
	int ret = 0;

	char szdllfn[MAX_PATH] = { 0 };
	if (argc <=1)
	{
		//lstrcpyA(szdllfn, "D:\\vsproject\\PEShell\\work\\test.dll");
		printf("please input filename of dll to load:");
		scanf("%s", szdllfn);
	}
	else {
		lstrcpyA(szdllfn, argv[1]);
	}

	

	HMODULE hthis = GetModuleHandleA(0);


	HMODULE h = LoadLibraryA(szdllfn);
	if (h)
	{
		PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)h;
		PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((char*)h + dos->e_lfanew);

		PIMAGE_DATA_DIRECTORY dir = (PIMAGE_DATA_DIRECTORY)(nt->OptionalHeader.DataDirectory);
		PIMAGE_SECTION_HEADER secs = (PIMAGE_SECTION_HEADER)((DWORD)nt + sizeof(IMAGE_NT_HEADERS));

		//mytest = (ptrmytest)ExportFunTable::getAddrFromName((DWORD)h, "mytest");
		ptrmytest mytest = 0;
		mytest  = (ptrmytest)GetProcAddress(h, "mytest");
		//
		//mytest = (ptrmytest)GetProcAddress(h, (char*)1);
		if (mytest > 0)
		{
			ret = mytest();
		}
		else {
			ret = GetLastError();
		}

		mytest = (ptrmytest)GetProcAddress(h, "test");
		ret = mytest();
	}
	else {
		ret = GetLastError();
		printf("LoadLibraryA error:%u\r\n", ret);
	}

	return 0;
}