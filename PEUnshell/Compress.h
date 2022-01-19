#pragma once


class Compress {
public:
	static int uncompressdata(unsigned char * src, unsigned long srcsize, unsigned char * dst, unsigned long  *dstsize);
	static int compressdata(unsigned char * src, unsigned long srcsize, unsigned char * dst, unsigned long  *dstsize);
};