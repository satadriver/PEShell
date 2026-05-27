#pragma once

#include <windows.h>
#include <iostream>

using namespace std;

class FileHelper {
public:
	static int CheckPathExist(string path);

	static int CheckFileExist(string filename);
	static int FileHelper::FileSearchSet(string fn, char* tag, int taglen, char* data, int size);
	static int FileHelper::fileReader(string filename, char** lpbuf, int* bufsize);
	static int FileHelper::fileWriter(string filename, const char* lpdata, int datasize);
	static int FileHelper::fileWriter(string filename, const char* lpdata, int datasize, int cover);

	static int GetFileSize(string filename);
};