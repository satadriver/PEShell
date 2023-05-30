#pragma once

#include <iostream>

using namespace std;



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

#pragma  pack(1)

typedef struct
{
	char username[32];
	char ip[32];
	char unused[64];
}ATTACK_RUN_PARAM, * LPATTACK_RUN_PARAM;


#pragma pack()


class Public {
public:
	static string getusername();
	static int getSysVer();

	static int writelog(string log);

	static int prepareCfg(char* cfgfn, string dstfn);

	static int Public::prepareParams(string ip, string username, string dstfn);

	static string Public::getCurPath();

	static int Public::writelog(char* fmt, ...);
};
