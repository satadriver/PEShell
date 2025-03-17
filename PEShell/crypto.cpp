
#include "Public.h"
#include "crypto.h"
#include "FileHelper.h"
#include "compress.h"

#include "Section.h"

#include <Shlwapi.h>
#pragma comment( lib, "Shlwapi.lib")

#include "md5.h"

#pragma comment(lib,"../lib\\libcrypto.lib")
#pragma comment(lib,"../lib\\libssl.lib")
#pragma comment(lib,"../lib\\openssl.lib")




unsigned char* Crypto::makeDataBlock(int type, const char filename[MAX_FILE_COUNT][MAX_PATH], int cnt, const char * outpath,int& dstdatasize)
{

	int ret = 0;

	int dstbufsize = 0;
	int allfSize[MAX_FILE_COUNT];
	for (int i = 0; i < cnt; i++)
	{
		char* fn = (char*)filename[i];
		int fz = FileHelper::getfilesize(fn);
		dstbufsize += fz;
		allfSize[i] = fz;
		printf("file name:%s size:%d\r\n", filename[i], fz);
	}
	dstbufsize += 0x100000;

	unsigned char* dstblock = new unsigned char[dstbufsize];
	FILE_DATA_BLOCK* blk = (FILE_DATA_BLOCK*)dstblock;
	blk->type = type;
	getkey(blk->key);
	blk->cfd.cnt = cnt;
	FILE_DATA* fd = (FILE_DATA*)& (blk->cfd.fd);

	int dstbuflimit = dstbufsize - sizeof(FILE_DATA_BLOCK) + sizeof(FILE_DATA);

	for (int i = 0; i < cnt; i++)
	{
		lstrcpyA((char*)fd->filename, filename[i]);
		PathStripPathA((char*)fd->filename);

		lstrcpyA(fd->outpath, outpath);

		dstbuflimit -=  sizeof(FILE_DATA);

		char* lpdata = 0;
		int filesize = 0;
		ret = FileHelper::fileReader(filename[i], &lpdata, &filesize);
		if (ret > 0)
		{
			unsigned long compsize = dstbuflimit ;
			ret = Compress::compressdata((unsigned char*)lpdata, filesize, (unsigned char*)fd + sizeof(FILE_DATA), &compsize);
			delete[] lpdata;
			if (ret != 0)
			{
				delete []dstblock;
				printf("compress file:%s error:%u\r\n", filename[i], GetLastError());
				return 0;
			}
			fd->compSize = compsize;
			fd->fsize = allfSize[i];
			dstbuflimit -= compsize;
			fd = (FILE_DATA*)((unsigned char*)fd + sizeof(FILE_DATA) + compsize);
		}
		else {
			delete[] dstblock;
			printf("%s read file:%s error\r\n",__FUNCTION__, filename[i]);
			return 0;
		}
	}

	dstdatasize = (unsigned char*)fd - dstblock;

	CryptData(dstblock + 4 + CRYPT_KEY_SIZE, dstdatasize - 4 - CRYPT_KEY_SIZE, blk->key, CRYPT_KEY_SIZE);

	//revertkey(key);

	return dstblock;
}

void Crypto::getkey(unsigned char* key) {
	SYSTEMTIME sttime = { 0 };
	GetLocalTime(&sttime);

	Crypto::getDataMd5((char*)&sttime, sizeof(SYSTEMTIME), (char*)key, FALSE);

	return;
}

void Crypto::revertkey(unsigned char* key) {
	for (int i = 0; i < CRYPT_KEY_SIZE; i++)
	{
		key[i] = ~key[i];
	}
}


void Crypto::CryptData(unsigned char* pdata, int size, unsigned char* psrckey, int keylen) {

	unsigned char* pkey = new unsigned char[keylen];
	for (int i = 0; i < keylen; i++)
	{
		pkey[i] = (~psrckey[i]) ^ 0x2b;
	}

	for (int i = 0, j = 0; i < size;)
	{
		pdata[i] ^= pkey[j];
		j++;
		if (j == keylen)
		{
			j = 0;
		}
		i++;
	}

	delete []pkey;
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

	memcpy(szmd5, hexmd5, 16);
	return TRUE;

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




void Crypto::cryptPayloadFile(string srcfn, string dstfn) {
	unsigned char key[CRYPT_KEY_SIZE] = { 0 };
	Crypto::getkey(key);

	char* lpdata = 0;
	int fs = 0;
	int ret = 0;

	ret = FileHelper::fileReader(srcfn, &lpdata, &fs);

	memcpy(lpdata + fs, key, CRYPT_KEY_SIZE);

	Crypto::CryptData((unsigned char*)lpdata, fs, key, CRYPT_KEY_SIZE);

	FileHelper::fileWriter(dstfn, lpdata, fs + CRYPT_KEY_SIZE, TRUE);
}




