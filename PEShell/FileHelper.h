#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

class FileHelper {
public:
	static int FileHelper::fileReader(string filename, char** lpbuf, int* bufsize);
	static int FileHelper::fileWriter(string filename, const char* lpdata, int datasize);
	static int FileHelper::fileWriter(string filename, const char* lpdata, int datasize, int cover);

	static int getfilesize(string filename);
};