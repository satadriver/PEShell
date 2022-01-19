#include <windows.h>
#include "zlib.h"
#include "zconf.h"
#include "compress.h"

#pragma comment(lib,"lib/zlib.lib")


typedef DWORD(__stdcall *RtlCompressBuffer_Fn)(
	IN ULONG   CompressionFormat,
	IN PVOID   SourceBuffer,
	IN ULONG   SourceBufferLength,
	OUT PVOID   DestinationBuffer,
	IN ULONG   DestinationBufferLength,
	IN ULONG   Unknown,
	OUT PULONG   pDestinationSize,
	IN PVOID   WorkspaceBuffer);

typedef DWORD(__stdcall *RtlDecompressBuffer_Fn)(
	IN ULONG   CompressionFormat,
	OUT PVOID   DestinationBuffer,
	IN ULONG   DestinationBufferLength,
	IN PVOID   SourceBuffer,
	IN ULONG   SourceBufferLength,
	OUT PULONG   pDestinationSize);

typedef DWORD(__stdcall *RtlGetCompressionWorkSpaceSize_Fn)(
	IN ULONG   CompressionFormat,
	OUT PULONG   pNeededBufferSize,
	OUT PULONG   pUnknown);


int Compress::uncompressdata(unsigned char * src, unsigned long srcsize, unsigned char * dstbuf, unsigned long *dstsize) {
	int ret = 0;
	ret = uncompress(dstbuf, dstsize, src, srcsize);

// 	HMODULE hntdll = LoadLibraryA("ntdll.dll");
// 	if (hntdll != NULL)
// 	{
// 		RtlCompressBuffer_Fn fcmp = (RtlCompressBuffer_Fn)GetProcAddress(hntdll, "RtlCompressBuffer");
// 		RtlDecompressBuffer_Fn fdcp = (RtlDecompressBuffer_Fn)GetProcAddress(hntdll, "RtlDecompressBuffer");
// 		RtlGetCompressionWorkSpaceSize_Fn fgcw = (RtlGetCompressionWorkSpaceSize_Fn)GetProcAddress(hntdll, "RtlGetCompressionWorkSpaceSize");
// 
// 		if (fcmp && fdcp && fgcw)
// 		{
// 			DWORD resultSize = 0;
// 			unsigned long x = 0;
// 			ret = (*fgcw)(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM, &resultSize, &x);
// 			void * tmp = LocalAlloc(LPTR, resultSize);
// 
// 			ret = (*fdcp)(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,  dstbuf, *dstsize, src, srcsize, &resultSize);
// 
// 			LocalFree(tmp);
// 
// 			*dstsize = resultSize;
// 
// 		}
// 	}

	return ret;
}

int Compress::compressdata(unsigned char * src, unsigned long srcsize, unsigned char * dst, unsigned long *dstsize) {
	int ret = 0;
	ret = compress(dst, dstsize, src, srcsize);

	return ret;
}




