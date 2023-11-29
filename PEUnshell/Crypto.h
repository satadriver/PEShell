#pragma once

#include <windows.h>

#include <iostream>

using namespace std;

typedef unsigned int uint;

class Crypto {
public:

	static void Crypto::CryptData(unsigned char * src, int size, unsigned char * pkey, int keylen, unsigned char * dst, int dstsize);

	static int getDataMd5(char * lpdata, int size, char * szmd5, int lowercase);

	static void Crypto::revertkey(unsigned char * key);
	static void Crypto::getkey(unsigned char * key);

	static int Crypto::releaseFiles(const char * data, int size);

	static int Crypto::reloadPE(const char * data, int datasize);


};
