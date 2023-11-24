#pragma once



#define  _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <WinNT.h>
#include <stdio.h>

#ifdef _WIN64
#define POINTER_TYPE DWORD64
#else
#define POINTER_TYPE DWORD
#endif

BOOL MemRunPE(void* lpFileData, int DataLength);
BOOL CheckDataValide(void* lpFileData, int DataLength);
BOOL FillRavAddress(void* pBase);
int GetAlignedSize(int Origin, int Alignment);

void CopyPEData(void* pDest, void* pSrc);
void DoRelocation(void* pNewImageBase);
void DoNewRelocation(void* lpNewImageBase, void* lpOldImageBase);