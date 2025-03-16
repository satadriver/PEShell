#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

#define MAX_FILE_COUNT			16

#define MEM_RUN_EXE				1
#define MEM_RUN_DLL				2
#define BIND_RELEASE_EXE		3
#define BIND_RELEASE_DLL		4

class Section {
public:
	static string insertSection(int type, int cpuarch, const char* secname, const char srcfiles[MAX_FILE_COUNT][MAX_PATH],
		int srcfilescnt, string path, char* outname);
};