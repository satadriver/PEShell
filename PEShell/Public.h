#pragma once
#pragma warning(disable : 4200)
#include <Windows.h>
#include <iostream>

using namespace std;
#pragma warning(disable : 4200)
#define LOG_FILE_NAME "log.log"

#define ONLY_ONE_EXE		1
#define ONLY_ONE_DLL		2
#define ONE_EXE_AND_ONE_DLL 3
#define SOME_OTHER_FILES	4

#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8

//char* gMyCryptKey = "PuppyBearKittenBunny";

#pragma  pack(1)

typedef struct
{
	char username[32];
	char ip[32];
	int mode;
	unsigned char key[16];
	char unused[44];
}ATTACK_RUN_PARAM, * LPATTACK_RUN_PARAM;


#pragma pack()

#define MAX_INPUT_FILE	16

#define FILENAME_LEN	64

#define CRYPT_KEY_SIZE	16


#pragma pack(1)

typedef struct {
	char filename[FILENAME_LEN];
	char outpath[FILENAME_LEN];
	ULONG fsize;
	ULONG compSize;
	unsigned char filedata[0];
}FILE_DATA;


typedef struct {
	int cnt;
	FILE_DATA fd;
}CRYPT_FILE_DATA;

typedef struct
{
	int type;
	unsigned char key[CRYPT_KEY_SIZE];
	CRYPT_FILE_DATA cfd;
}FILE_DATA_BLOCK;


#pragma pack()

//#define offsetof(s, m) ((size_t)&((*s)0)->m)

int GetPathFromFullName(const char* strFullName, char* strDst);



class Public {
public:
	static string getusername();
	static int getSysVer();

	static int writelog(string log);

	static int prepareCfg(char* cfgfn, string dstfn);

	static int Public::prepareParams(string ip, string username,int mode, unsigned char* key, string dstfn);

	static string Public::getCurPath();

	static int Public::writelog(char* fmt, ...);
};
