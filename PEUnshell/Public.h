#pragma once

#include <iostream>
#include <windows.h>

using namespace std;

#define LOG_FILE_NAME "log.log"

#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8


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


class Public {
public:
	static string getusername();
	static int getSysVer();

	static int writelog(string log);

	static int Public::writelog(char * fmt, ...);
};