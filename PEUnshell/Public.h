#pragma once

#include <iostream>

using namespace std;


#define SYSTEM_VERSION_WIN9X	1
#define SYSTEM_VERSION_WIN2000	2
#define SYSTEM_VERSION_XP		3
#define SYSTEM_VERSION_VISTA	4
#define SYSTEM_VERSION_WIN7		5
#define SYSTEM_VERSION_WIN8		6
#define SYSTEM_VERSION_WIN10	7
#define SYSTEM_VERSION_UNKNOW	8


class Public {
public:
	static string getusername();
	static int getSysVer();

	static int writelog(string log);

	static int Public::writelog(char * fmt, ...);
};