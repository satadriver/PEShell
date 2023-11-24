
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

//#define MAX_BUF_SIZE 0x200000

#define FILENAME_LEN	64

#define CRYPT_KEY_SIZE	16



#pragma pack(1)
typedef struct
{
	int type;
	unsigned char key;
	int cnt;
}BLOCK_FILE_HEADER, * LPBLOCK_FILE_HEADER;
#pragma pack()


unsigned char* Crypto::makeDataBlock(int flag, const char filename[MAX_FILE_COUNT][256], int cnt, int& dstdatasize) {

	int ret = 0;

	int filesize = 0;
	for (int i = 0; i < cnt; i++)
	{
		int fz = FileHelper::getfilesize(filename[i]);
		filesize += fz;
		printf("file name:%s size:%d\r\n", filename[i], fz);
	}

	int dstbufsize = filesize + 0x1000;

	unsigned char* dstblock = new unsigned char[dstbufsize];

	*(int*)dstblock = flag;

	// 	if (cnt == 1 && strstr((char*)filename[0],".exe") )
	// 	{
	// 		*(int*)dstblock = ONLY_ONE_EXE;
	// 	}
	// 	else if (cnt == 1 && strstr((char*)filename[0], ".dll"))
	// 	{
	// 		*(int*)dstblock = ONLY_ONE_DLL;
	// 	}
	// 	else if (cnt > 1 )
	// 	{
	// 		int flagexe = 0;
	// 		int flagdll = 0;
	// 		for (int i = 0;i < cnt; i ++)
	// 		{
	// 			if (strstr((char*)filename[i], ".dll")) {
	// 				flagdll = 1;
	// 			}else if (strstr(filename[i],".exe"))
	// 			{
	// 				flagexe = 1;
	// 			}
	// 		}
	// 
	// 		if (flagexe && flagdll)
	// 		{
	// 			*(int*)dstblock = ONE_EXE_AND_ONE_DLL;
	// 		}
	// 		else {
	// 			*(int*)dstblock = SOME_OTHER_FILES;
	// 		}
	// 	}
	// 	else {
	// 		return 0;
	// 	}

	unsigned char* key = dstblock + 4;

	getkey(key);

	*(int*)(dstblock + 4 + CRYPT_KEY_SIZE) = cnt;

	unsigned char* dstbuf = dstblock + 4 + CRYPT_KEY_SIZE + 4;

	int dstbuflimit = dstbufsize - 4 - CRYPT_KEY_SIZE - 4;

	for (int i = 0; i < cnt; i++)
	{
		lstrcpyA((char*)dstbuf, filename[i]);
		PathStripPathA((char*)dstbuf);
		dstbuf += FILENAME_LEN;
		dstbuflimit -= FILENAME_LEN;

		char* lpdata = 0;

		ret = FileHelper::fileReader(filename[i], &lpdata, &filesize);
		if (ret > 0)
		{
			unsigned long cmpresssize = dstbuflimit - 4;
			ret = Compress::compressdata((unsigned char*)lpdata, filesize, dstbuf + 4, &cmpresssize);
			delete[] lpdata;
			if (ret != 0)
			{
				delete dstblock;
				printf("compress file:%s error:%u\r\n", filename[i], GetLastError());
				return 0;
			}

			*(int*)(dstbuf) = cmpresssize;
			dstbuf += 4;
			dstbuf += cmpresssize;
			dstbuflimit -= 4;
			dstbuflimit -= cmpresssize;
		}
		else {
			delete dstblock;
			printf("read file:%s error\r\n", filename[i]);
			return 0;
		}
	}

	dstdatasize = dstbuf - dstblock;

	CryptData(dstblock + 4 + CRYPT_KEY_SIZE, dstdatasize - 4 - CRYPT_KEY_SIZE, key, CRYPT_KEY_SIZE);

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

	delete pkey;
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




