#pragma once


#include "../include/openssl/md5.h"

#include <windows.h>

#include <iostream>

#include "Section.h"

using namespace std;

typedef unsigned int uint;

class Crypto {
public:

	static void Crypto::CryptData(unsigned char* pdata, int size, unsigned char* pkey, int keylen);
	static int getDataMd5(char* lpdata, int size, char* szmd5, int lowercase);

	static void Crypto::getkey(unsigned char* key);

	static void Crypto::revertkey(unsigned char* key);

	static unsigned char* Crypto::makeDataBlock(int type, const char filename[MAX_FILE_COUNT][256], int cnt,
		const char * outpath, int& dstdatasize);

	static void Crypto::cryptPayloadFile(string srcfn, string dstfn);

};