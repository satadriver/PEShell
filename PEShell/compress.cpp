
#include "zlib.h"
#include "zconf.h"
#include "compress.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>


  

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

#pragma comment(lib,"lib/zlib.lib")

int Compress::compressdata(unsigned char * src, unsigned long srcsize, unsigned char * dstbuf, unsigned long *dstsize) {
	int ret = 0;
	ret = compress(dstbuf, dstsize, src, srcsize);

// 	HMODULE hntdll = LoadLibraryA("ntdll.dll");
// 	if (hntdll != NULL)
// 	{
// 		RtlCompressBuffer_Fn fcmp = (RtlCompressBuffer_Fn)GetProcAddress(hntdll, "RtlCompressBuffer");
// 		RtlDecompressBuffer_Fn fdcp = (RtlDecompressBuffer_Fn)GetProcAddress(hntdll, "RtlDecompressBuffer");
// 		RtlGetCompressionWorkSpaceSize_Fn fgcw = (RtlGetCompressionWorkSpaceSize_Fn)GetProcAddress(hntdll, "RtlGetCompressionWorkSpaceSize");
// 
// 
// 		if (fcmp && fdcp && fgcw)
// 		{
// 			DWORD resultSize = 0;
// 			unsigned long x = 0;
// 			ret = (*fgcw)(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM, &resultSize, &x);
// 			void * tmp = LocalAlloc(LPTR, resultSize);
// 
// 			ret = (*fcmp)(COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM, src, srcsize, dstbuf, *dstsize, x, &resultSize, tmp);
// 
// 			LocalFree(tmp);
// 
// 			*dstsize = resultSize;
// 
// 		}
// 	}

	return ret;
}
