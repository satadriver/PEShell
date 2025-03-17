#include "FileHelper.h"
#include "public.h"
#include <dbghelp.h>

#pragma comment(lib,"Dbghelp.lib")

int FileHelper::CheckFileExist(string filename) {
	HANDLE hFile = CreateFileA((char*)filename.c_str(), GENERIC_WRITE | GENERIC_READ, 0, 0, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, 0);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		if (err == 32 || err == 183)
		{
			return TRUE;
		}
		return FALSE;
	}
	else {
		CloseHandle(hFile);
		return TRUE;
	}
}


int FileHelper::CheckPathExist(string path) {
	if (path.back() != '\\')
	{
		path.append("\\");
	}
	path.append("*.*");

	WIN32_FIND_DATAA stfd;
	HANDLE hFind = FindFirstFileA((char*)path.c_str(), &stfd);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		int err = GetLastError();
		if (err == 32 || err == 183)
		{
			return TRUE;
		}
		return FALSE;
	}
	else {
		FindClose(hFind);
		return TRUE;
	}
}

int FileHelper::getfilesize(string filename) {
	int ret = 0;

	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		printf("getfilesize fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fseek(fp, 0, FILE_END);

	int filesize = ftell(fp);

	fclose(fp);

	return filesize;
}

int FileHelper::fileReader(string filename, char** lpbuf, int* bufsize) {
	int ret = 0;

	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		printf("fileReader fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fseek(fp, 0, FILE_END);

	int filesize = ftell(fp);

	ret = fseek(fp, 0, FILE_BEGIN);

	*bufsize = filesize;

	*lpbuf = new char[filesize + 0x1000];

	ret = fread(*lpbuf, 1, filesize, fp);
	fclose(fp);
	if (ret <= FALSE)
	{
		delete* lpbuf;
		return FALSE;
	}

	*(*lpbuf + filesize) = 0;
	return filesize;
}



int FileHelper::fileWriter(string filename, const char* lpdata, int datasize) {

	return fileWriter(filename, lpdata, datasize, FALSE);
}


int fileWriter_old(string filename, const char* lpdata, int datasize, int cover) {
	int ret = 0;

	FILE* fp = 0;
	if (cover) {
		fp = fopen(filename.c_str(), "wb");
	}
	else {
		fp = fopen(filename.c_str(), "ab+");
	}

	if (fp <= 0)
	{
		printf("fileWriter fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	ret = fwrite(lpdata, 1, datasize, fp);
	fclose(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datasize;
}


int FileHelper::fileWriter(string filename, const char* lpdata, int datasize, int cover) {
	int ret = 0;

	HANDLE fp = 0;
	if (cover) {

		char path[MAX_PATH];
		int len = GetPathFromFullName((char*)filename.c_str(), path);
		ret = MakeSureDirectoryPathExists(path);
		fp = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, 0,
			CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	}
	else {
		fp = CreateFileA(filename.c_str(), GENERIC_WRITE,  0, 0,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (fp != INVALID_HANDLE_VALUE) {
			LONG high = 0;
			ret = SetFilePointer(fp, 0, &high, FILE_END);
		}
	}

	if (fp == INVALID_HANDLE_VALUE)
	{
		printf("fileWriter fopen file:%s error\r\n", filename.c_str());
		return FALSE;
	}

	DWORD cnt = 0;
	ret = WriteFile(fp,lpdata, datasize, &cnt,0);
	CloseHandle(fp);
	if (ret == FALSE)
	{
		return FALSE;
	}

	return datasize;
}