#pragma once


#include <windows.h>
#include <iostream>

using namespace std;

class FileHelper {
public:

	static int CheckPathExist(string path);

	static int CheckFileExist(string filename);

	static int FileHelper::fileReader(string filename, char** lpbuf, int* bufsize);
	static int FileHelper::fileWriter(string filename, const char* lpdate, int datesize);
	static int FileHelper::fileWriter(string filename, const char* lpdate, int datesize, int cover);


	static int FileHelper::fileReader_c(string filename, char** lpbuf, int* bufsize);

	static int FileHelper::fileWriter_c(string filename, const char* lpdate, int datesize, int cover);

	static string FileHelper::getReleasePath(const char* path);
	static string getRunPath();
};
