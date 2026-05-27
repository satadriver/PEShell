#include "FileHelper.h"
#include "public.h"
#include <dbghelp.h>
#include "Public.h"

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

int FileHelper::GetFileSize(string filename) {
	int ret = 0;

	FILE* fp = fopen(filename.c_str(), "rb");
	if (fp <= 0)
	{
		log("%s %d fopen file:%s error\r\n", __FUNCTION__, __LINE__, filename.c_str());
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
		log("%s %d fopen file:%s error\r\n", __FUNCTION__, __LINE__, filename.c_str());
		return FALSE;
	}

	ret = fseek(fp, 0, FILE_END);

	unsigned long filesize = ftell(fp);

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
		log("fileWriter fopen file:%s error\r\n", filename.c_str());
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
		log("%s %d fopen file:%s error\r\n", __FUNCTION__, __LINE__, filename.c_str());
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


int FileHelper::FileSearchSet(string fn, char* tag, int taglen, char* data, int size) {

	char* file = 0;
	int fs = 0;
	int ret = 0;
	int result = 0;
	ret = fileReader(fn, &file, &fs);
	if (ret == 0) {
		return 0;
	}

	char* ptr = SearchBinary(file, fs, tag, taglen);
	if (ptr) {
		memcpy(ptr, data, size);
		ret = fileWriter(fn, file, fs, 1);
		result = TRUE;
	}
	delete file;
	return result;
}