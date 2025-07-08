#pragma once


#define QWORD unsigned long long

QWORD MemLoadDll64(char* filedata, char* addr);

char* getAddrFromName64(char* module, const char* funname);

QWORD getAddrFromOrd64(char* module, DWORD ord);

bool relocTable64(char* pImageBase, ULONGLONG newBase);

bool mapFile64(char* pFileBuff, char* chBaseAddress);

int importTable64(char* pImageBase);

bool relocTable64(char* pImageBase, ULONGLONG newBase);

int setImageBase64(char* chBaseAddress);

