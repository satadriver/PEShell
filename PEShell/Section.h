#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

#define MAX_FILE_COUNT		16

class Section {
public:
	static string insertSection(int flag, const char* secname, const char srcfiles[MAX_FILE_COUNT][256], int srcfilescnt, char* outname);
};